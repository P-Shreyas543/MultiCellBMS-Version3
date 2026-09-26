/**
 * @file ds3231.c
 * @brief Automotive/Industrial Driver implementation for Maxim DS3231 RTC/TCXO.
 * 
 * Target: NXP S32K1xx / Simulink Model-Based Design (MBDT)
 * Dual-Mode: Simulink Desktop Simulation and S32K hardware execution.
 */

#include "ds3231.h"
#include <string.h>

#if defined(MATLAB_MEX_FILE) || defined(SIMULINK_SIM)
#define DS3231_IS_SIMULATION 1
#else
#define DS3231_IS_SIMULATION 0
#include "lpi2c_driver.h"
#include "osif.h"
#endif

/* Configurable LPI2C instance index (0 = LPI2C0, 1 = LPI2C1) */
static uint32_t s_ds3231_i2c_instance = DS3231_DEFAULT_I2C_INSTANCE;

/* ========================================================================== */
/* BCD Conversion Utilities                                                   */
/* ========================================================================== */
static inline uint8_t ds3231_dec_to_bcd(uint8_t val)
{
    return (uint8_t)(((val / 10U) << 4U) | (val % 10U));
}

static inline uint8_t ds3231_bcd_to_dec(uint8_t val)
{
    return (uint8_t)(((val >> 4U) * 10U) + (val & 0x0FU));
}

static inline uint8_t ds3231_hours_to_reg(uint8_t h)
{
    /* 24-hour mode: bit 6 = 0 */
    return (uint8_t)((((h / 10U) & 0x03U) << 4U) | (h % 10U));
}

static inline uint8_t ds3231_reg_to_hours(uint8_t reg)
{
    /* 24-hour mode decoding (mask out 12/24 flag in bit 6) */
    return (uint8_t)((((reg >> 4U) & 0x03U) * 10U) + (reg & 0x0FU));
}

/* ========================================================================== */
/* Simulation Mode (Desktop PC Simulation)                                    */
/* ========================================================================== */
#if DS3231_IS_SIMULATION
static uint8_t s_ds3231_sim_regs[DS3231_REG_MAP_SIZE];
static bool s_ds3231_sim_initialized = false;

static void DS3231_SimInitIfNeeded(void)
{
    if (!s_ds3231_sim_initialized) {
        memset(s_ds3231_sim_regs, 0x00, sizeof(s_ds3231_sim_regs));
        /* Default simulated initial time: 2026-09-26 12:00:00, Day 7 (Saturday) */
        s_ds3231_sim_regs[DS3231_REG_SECONDS] = ds3231_dec_to_bcd(0U);
        s_ds3231_sim_regs[DS3231_REG_MINUTES] = ds3231_dec_to_bcd(0U);
        s_ds3231_sim_regs[DS3231_REG_HOURS]   = ds3231_hours_to_reg(12U);
        s_ds3231_sim_regs[DS3231_REG_DAY]     = 7U; /* Saturday */
        s_ds3231_sim_regs[DS3231_REG_DATE]    = ds3231_dec_to_bcd(26U);
        s_ds3231_sim_regs[DS3231_REG_MONTH]   = ds3231_dec_to_bcd(9U);
        s_ds3231_sim_regs[DS3231_REG_YEAR]    = ds3231_dec_to_bcd(26U);
        /* Default simulated temperature: 25.25 degC (MSB=25, LSB=0x40) */
        s_ds3231_sim_regs[DS3231_REG_TEMP_MSB] = 25U;
        s_ds3231_sim_regs[DS3231_REG_TEMP_LSB] = 0x40U;
        s_ds3231_sim_initialized = true;
    }
}

static void DS3231_SimAdvanceTime(void)
{
    /* Increment simulated clock by 1 second on each read call */
    uint8_t sec = ds3231_bcd_to_dec(s_ds3231_sim_regs[DS3231_REG_SECONDS]) + 1U;
    if (sec >= 60U) {
        sec = 0U;
        uint8_t min = ds3231_bcd_to_dec(s_ds3231_sim_regs[DS3231_REG_MINUTES]) + 1U;
        if (min >= 60U) {
            min = 0U;
            uint8_t hr = ds3231_reg_to_hours(s_ds3231_sim_regs[DS3231_REG_HOURS]) + 1U;
            if (hr >= 24U) {
                hr = 0U;
            }
            s_ds3231_sim_regs[DS3231_REG_HOURS] = ds3231_hours_to_reg(hr);
        }
        s_ds3231_sim_regs[DS3231_REG_MINUTES] = ds3231_dec_to_bcd(min);
    }
    s_ds3231_sim_regs[DS3231_REG_SECONDS] = ds3231_dec_to_bcd(sec);
}
#endif

/* ========================================================================== */
/* Hardware Low-Level Bus Helpers                                             */
/* ========================================================================== */
#if !DS3231_IS_SIMULATION
static status_t DS3231_I2C_Write(uint8_t reg_addr, const uint8_t *data, uint16_t length)
{
    uint8_t tx_buf[DS3231_REG_MAP_SIZE + 1U];
    if (length > DS3231_REG_MAP_SIZE) {
        return STATUS_ERROR;
    }

    tx_buf[0] = reg_addr;
    if ((data != NULL) && (length > 0U)) {
        memcpy(&tx_buf[1], data, length);
    }

    LPI2C_DRV_MasterSetSlaveAddr(s_ds3231_i2c_instance, (uint16_t)DS3231_I2C_ADDR, false);
    return LPI2C_DRV_MasterSendDataBlocking(
        s_ds3231_i2c_instance, tx_buf, (uint32_t)(length + 1U), true, DS3231_I2C_TIMEOUT_MS);
}

static status_t DS3231_I2C_Read(uint8_t reg_addr, uint8_t *data, uint16_t length)
{
    LPI2C_DRV_MasterSetSlaveAddr(s_ds3231_i2c_instance, (uint16_t)DS3231_I2C_ADDR, false);

    /* 1. Send register pointer without STOP (repeated START) */
    status_t status = LPI2C_DRV_MasterSendDataBlocking(
        s_ds3231_i2c_instance, &reg_addr, 1U, false, DS3231_I2C_TIMEOUT_MS);
    if (status != STATUS_SUCCESS) {
        return status;
    }

    /* 2. Read sequential register data with STOP */
    return LPI2C_DRV_MasterReceiveDataBlocking(
        s_ds3231_i2c_instance, data, (uint32_t)length, true, DS3231_I2C_TIMEOUT_MS);
}
#endif

/* ========================================================================== */
/* Public API Functions                                                       */
/* ========================================================================== */

void DS3231_Init(void)
{
#if DS3231_IS_SIMULATION
    DS3231_SimInitIfNeeded();
#else
    /* S32K target peripheral initialization handled via MBDT LPI2C_Config block */
#endif
}

void DS3231_SetI2CInstance(uint32_t instance)
{
    s_ds3231_i2c_instance = instance;
}

uint32_t DS3231_GetI2CInstance(void)
{
    return s_ds3231_i2c_instance;
}

uint8_t DS3231_SetTime(const ds3231_time_t *time)
{
    if (time == NULL) {
        return (uint8_t)DS3231_STATUS_PARAM_ERROR;
    }

    /* Range validation */
    if ((time->seconds > 59U) || (time->minutes > 59U) || (time->hours > 23U) ||
        (time->day_of_week < 1U) || (time->day_of_week > 7U) ||
        (time->date < 1U) || (time->date > 31U) ||
        (time->month < 1U) || (time->month > 12U) || (time->year > 99U)) {
        return (uint8_t)DS3231_STATUS_PARAM_ERROR;
    }

    uint8_t raw[7];
    raw[0] = ds3231_dec_to_bcd(time->seconds);
    raw[1] = ds3231_dec_to_bcd(time->minutes);
    raw[2] = ds3231_hours_to_reg(time->hours);
    raw[3] = time->day_of_week & 0x07U;
    raw[4] = ds3231_dec_to_bcd(time->date);
    raw[5] = ds3231_dec_to_bcd(time->month); /* century bit 7 cleared */
    raw[6] = ds3231_dec_to_bcd(time->year);

#if DS3231_IS_SIMULATION
    DS3231_SimInitIfNeeded();
    memcpy(&s_ds3231_sim_regs[DS3231_REG_SECONDS], raw, 7U);
    return (uint8_t)DS3231_STATUS_OK;
#else
    status_t status = DS3231_I2C_Write(DS3231_REG_SECONDS, raw, 7U);
    return (status == STATUS_SUCCESS) ? (uint8_t)DS3231_STATUS_OK : (uint8_t)DS3231_STATUS_ERROR;
#endif
}

uint8_t DS3231_GetTime(ds3231_time_t *time)
{
    if (time == NULL) {
        return (uint8_t)DS3231_STATUS_PARAM_ERROR;
    }

    uint8_t raw[7];

#if DS3231_IS_SIMULATION
    DS3231_SimInitIfNeeded();
    DS3231_SimAdvanceTime();
    memcpy(raw, &s_ds3231_sim_regs[DS3231_REG_SECONDS], 7U);
#else
    status_t status = DS3231_I2C_Read(DS3231_REG_SECONDS, raw, 7U);
    if (status != STATUS_SUCCESS) {
        return (uint8_t)DS3231_STATUS_ERROR;
    }
#endif

    time->seconds     = ds3231_bcd_to_dec(raw[0] & 0x7FU);
    time->minutes     = ds3231_bcd_to_dec(raw[1] & 0x7FU);
    time->hours       = ds3231_reg_to_hours(raw[2]);
    time->day_of_week = raw[3] & 0x07U;
    time->date        = ds3231_bcd_to_dec(raw[4] & 0x3FU);
    time->month       = ds3231_bcd_to_dec(raw[5] & 0x1FU);
    time->year        = ds3231_bcd_to_dec(raw[6]);

    return (uint8_t)DS3231_STATUS_OK;
}

uint8_t DS3231_SetTimeArray(const uint8_t *time_vec)
{
    if (time_vec == NULL) {
        return (uint8_t)DS3231_STATUS_PARAM_ERROR;
    }

    ds3231_time_t t;
    t.year        = time_vec[0];
    t.month       = time_vec[1];
    t.date        = time_vec[2];
    t.day_of_week = time_vec[3];
    t.hours       = time_vec[4];
    t.minutes     = time_vec[5];
    t.seconds     = time_vec[6];

    return DS3231_SetTime(&t);
}

uint8_t DS3231_GetTimeArray(uint8_t *time_vec)
{
    if (time_vec == NULL) {
        return (uint8_t)DS3231_STATUS_PARAM_ERROR;
    }

    ds3231_time_t t;
    uint8_t status = DS3231_GetTime(&t);
    if (status != (uint8_t)DS3231_STATUS_OK) {
        memset(time_vec, 0x00, DS3231_TIME_ARRAY_SIZE);
        return status;
    }

    time_vec[0] = t.year;
    time_vec[1] = t.month;
    time_vec[2] = t.date;
    time_vec[3] = t.day_of_week;
    time_vec[4] = t.hours;
    time_vec[5] = t.minutes;
    time_vec[6] = t.seconds;

    return (uint8_t)DS3231_STATUS_OK;
}

uint8_t DS3231_GetTemperature(float *temp_c)
{
    if (temp_c == NULL) {
        return (uint8_t)DS3231_STATUS_PARAM_ERROR;
    }

    int16_t temp_quarter;
    uint8_t status = DS3231_GetTemperatureFixed(&temp_quarter);
    if (status != (uint8_t)DS3231_STATUS_OK) {
        *temp_c = 0.0f;
        return status;
    }

    *temp_c = ((float)temp_quarter) * 0.25f;
    return (uint8_t)DS3231_STATUS_OK;
}

uint8_t DS3231_GetTemperatureFixed(int16_t *temp_quarter_deg)
{
    if (temp_quarter_deg == NULL) {
        return (uint8_t)DS3231_STATUS_PARAM_ERROR;
    }

    uint8_t raw[2];

#if DS3231_IS_SIMULATION
    DS3231_SimInitIfNeeded();
    raw[0] = s_ds3231_sim_regs[DS3231_REG_TEMP_MSB];
    raw[1] = s_ds3231_sim_regs[DS3231_REG_TEMP_LSB];
#else
    status_t status = DS3231_I2C_Read(DS3231_REG_TEMP_MSB, raw, 2U);
    if (status != STATUS_SUCCESS) {
        *temp_quarter_deg = 0;
        return (uint8_t)DS3231_STATUS_ERROR;
    }
#endif

    int8_t temp_msb = (int8_t)raw[0];
    uint8_t temp_lsb = (uint8_t)(raw[1] >> 6U); /* Top 2 bits represent 0.25 degC steps */

    *temp_quarter_deg = (int16_t)(((int16_t)temp_msb * 4) + (int16_t)temp_lsb);
    return (uint8_t)DS3231_STATUS_OK;
}

uint8_t DS3231_CheckOscillatorStopFlag(bool *osf_flag, bool clear_if_set)
{
    if (osf_flag == NULL) {
        return (uint8_t)DS3231_STATUS_PARAM_ERROR;
    }

    uint8_t status_reg = 0U;

#if DS3231_IS_SIMULATION
    DS3231_SimInitIfNeeded();
    status_reg = s_ds3231_sim_regs[DS3231_REG_STATUS];
    *osf_flag = ((status_reg & 0x80U) != 0U);
    if (*osf_flag && clear_if_set) {
        s_ds3231_sim_regs[DS3231_REG_STATUS] &= (uint8_t)(~0x80U);
    }
    return (uint8_t)DS3231_STATUS_OK;
#else
    status_t status = DS3231_I2C_Read(DS3231_REG_STATUS, &status_reg, 1U);
    if (status != STATUS_SUCCESS) {
        *osf_flag = false;
        return (uint8_t)DS3231_STATUS_ERROR;
    }

    *osf_flag = ((status_reg & 0x80U) != 0U);

    if (*osf_flag && clear_if_set) {
        uint8_t cleared_reg = (uint8_t)(status_reg & (uint8_t)(~0x80U));
        (void)DS3231_I2C_Write(DS3231_REG_STATUS, &cleared_reg, 1U);
    }

    return (uint8_t)DS3231_STATUS_OK;
#endif
}

uint8_t DS3231_GetTimeScalars(uint8_t *hours, uint8_t *minutes, uint8_t *seconds)
{
    if ((hours == NULL) || (minutes == NULL) || (seconds == NULL)) {
        return (uint8_t)DS3231_STATUS_PARAM_ERROR;
    }

    uint8_t raw[3];

#if DS3231_IS_SIMULATION
    DS3231_SimInitIfNeeded();
    DS3231_SimAdvanceTime();
    raw[0] = s_ds3231_sim_regs[DS3231_REG_SECONDS];
    raw[1] = s_ds3231_sim_regs[DS3231_REG_MINUTES];
    raw[2] = s_ds3231_sim_regs[DS3231_REG_HOURS];
#else
    status_t status = DS3231_I2C_Read(DS3231_REG_SECONDS, raw, 3U);
    if (status != STATUS_SUCCESS) {
        *hours = 0U;
        *minutes = 0U;
        *seconds = 0U;
        return (uint8_t)DS3231_STATUS_ERROR;
    }
#endif

    *seconds = ds3231_bcd_to_dec(raw[0] & 0x7FU);
    *minutes = ds3231_bcd_to_dec(raw[1] & 0x7FU);
    *hours   = ds3231_reg_to_hours(raw[2]);

    return (uint8_t)DS3231_STATUS_OK;
}

uint8_t DS3231_SetTimeScalars(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    if ((hours > 23U) || (minutes > 59U) || (seconds > 59U)) {
        return (uint8_t)DS3231_STATUS_PARAM_ERROR;
    }

    uint8_t raw[3];
    raw[0] = ds3231_dec_to_bcd(seconds);
    raw[1] = ds3231_dec_to_bcd(minutes);
    raw[2] = ds3231_hours_to_reg(hours);

#if DS3231_IS_SIMULATION
    DS3231_SimInitIfNeeded();
    s_ds3231_sim_regs[DS3231_REG_SECONDS] = raw[0];
    s_ds3231_sim_regs[DS3231_REG_MINUTES] = raw[1];
    s_ds3231_sim_regs[DS3231_REG_HOURS]   = raw[2];
    return (uint8_t)DS3231_STATUS_OK;
#else
    status_t status = DS3231_I2C_Write(DS3231_REG_SECONDS, raw, 3U);
    return (status == STATUS_SUCCESS) ? (uint8_t)DS3231_STATUS_OK : (uint8_t)DS3231_STATUS_ERROR;
#endif
}

uint8_t DS3231_GetDateScalars(uint8_t *year, uint8_t *month, uint8_t *date, uint8_t *day_of_week)
{
    if ((year == NULL) || (month == NULL) || (date == NULL) || (day_of_week == NULL)) {
        return (uint8_t)DS3231_STATUS_PARAM_ERROR;
    }

    uint8_t raw[4];

#if DS3231_IS_SIMULATION
    DS3231_SimInitIfNeeded();
    raw[0] = s_ds3231_sim_regs[DS3231_REG_DAY];
    raw[1] = s_ds3231_sim_regs[DS3231_REG_DATE];
    raw[2] = s_ds3231_sim_regs[DS3231_REG_MONTH];
    raw[3] = s_ds3231_sim_regs[DS3231_REG_YEAR];
#else
    status_t status = DS3231_I2C_Read(DS3231_REG_DAY, raw, 4U);
    if (status != STATUS_SUCCESS) {
        *year = 0U;
        *month = 0U;
        *date = 0U;
        *day_of_week = 0U;
        return (uint8_t)DS3231_STATUS_ERROR;
    }
#endif

    *day_of_week = raw[0] & 0x07U;
    *date        = ds3231_bcd_to_dec(raw[1] & 0x3FU);
    *month       = ds3231_bcd_to_dec(raw[2] & 0x1FU);
    *year        = ds3231_bcd_to_dec(raw[3]);

    return (uint8_t)DS3231_STATUS_OK;
}

uint8_t DS3231_SetDateScalars(uint8_t year, uint8_t month, uint8_t date, uint8_t day_of_week)
{
    if ((year > 99U) || (month < 1U) || (month > 12U) ||
        (date < 1U) || (date > 31U) || (day_of_week < 1U) || (day_of_week > 7U)) {
        return (uint8_t)DS3231_STATUS_PARAM_ERROR;
    }

    uint8_t raw[4];
    raw[0] = day_of_week & 0x07U;
    raw[1] = ds3231_dec_to_bcd(date);
    raw[2] = ds3231_dec_to_bcd(month);
    raw[3] = ds3231_dec_to_bcd(year);

#if DS3231_IS_SIMULATION
    DS3231_SimInitIfNeeded();
    s_ds3231_sim_regs[DS3231_REG_DAY]   = raw[0];
    s_ds3231_sim_regs[DS3231_REG_DATE]  = raw[1];
    s_ds3231_sim_regs[DS3231_REG_MONTH] = raw[2];
    s_ds3231_sim_regs[DS3231_REG_YEAR]  = raw[3];
    return (uint8_t)DS3231_STATUS_OK;
#else
    status_t status = DS3231_I2C_Write(DS3231_REG_DAY, raw, 4U);
    return (status == STATUS_SUCCESS) ? (uint8_t)DS3231_STATUS_OK : (uint8_t)DS3231_STATUS_ERROR;
#endif
}

