/**
 * @file ds3231.h
 * @brief Automotive/Industrial Driver for Maxim DS3231 Extremely Accurate I2C RTC/TCXO.
 * 
 * Target: NXP S32K1xx (Cortex-M4) / Simulink Model-Based Design (MBDT)
 * Standard: Aligned with AUTOSAR conventions and MISRA-C:2012.
 * 
 * Hardware Characteristics:
 * - 7-bit Slave Address: 0x68 (Binary 1101000)
 * - Clock / Calendar: Seconds, Minutes, Hours (24h/12h), Day, Date, Month, Year
 * - Internal TCXO accuracy: +-2ppm (0 to +40 degC), +-3.5ppm (-40 to +85 degC)
 * - Integrated 10-bit Temperature Sensor (0.25 degC resolution)
 * - Battery-backup input for continuous timekeeping
 * - Configurable I2C instance (default LPI2C0, selectable for LPI2C1/etc.)
 */

#ifndef DS3231_H
#define DS3231_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/* Hardware & Register Configuration                                          */
/* ========================================================================== */
#define DS3231_I2C_ADDR             0x68U    /*!< 7-bit standard I2C slave address */
#define DS3231_REG_SECONDS          0x00U    /*!< Seconds register address */
#define DS3231_REG_MINUTES          0x01U    /*!< Minutes register address */
#define DS3231_REG_HOURS            0x02U    /*!< Hours register address */
#define DS3231_REG_DAY              0x03U    /*!< Day of week register (1-7) */
#define DS3231_REG_DATE             0x04U    /*!< Date of month register (1-31) */
#define DS3231_REG_MONTH            0x05U    /*!< Month (1-12) & Century bit */
#define DS3231_REG_YEAR             0x06U    /*!< Year (0-99) */
#define DS3231_REG_ALARM1_SEC       0x07U    /*!< Alarm 1 seconds */
#define DS3231_REG_ALARM2_MIN       0x0BU    /*!< Alarm 2 minutes */
#define DS3231_REG_CONTROL          0x0EU    /*!< Control register */
#define DS3231_REG_STATUS           0x0FU    /*!< Control / Status register */
#define DS3231_REG_AGING            0x10U    /*!< Aging offset register */
#define DS3231_REG_TEMP_MSB         0x11U    /*!< Temperature MSB (integer part) */
#define DS3231_REG_TEMP_LSB         0x12U    /*!< Temperature LSB (fractional part) */
#define DS3231_REG_MAP_SIZE         0x13U    /*!< Total register map size (19 bytes) */

/* Default LPI2C instance (can be overridden at compile-time or runtime) */
#ifndef DS3231_DEFAULT_I2C_INSTANCE
#define DS3231_DEFAULT_I2C_INSTANCE 0U
#endif

#define DS3231_I2C_TIMEOUT_MS       25U      /*!< Bounded blocking timeout in ms */
#define DS3231_TIME_ARRAY_SIZE      7U       /*!< Standardized 7-byte time array */

/* ========================================================================== */
/* Status Return Codes (AUTOSAR aligned)                                      */
/* ========================================================================== */
typedef enum {
    DS3231_STATUS_OK          = 0x00U,  /*!< Operation completed successfully */
    DS3231_STATUS_ERROR       = 0x01U,  /*!< Hardware bus failure or I2C NACK */
    DS3231_STATUS_BUSY        = 0x02U,  /*!< Peripheral or TCXO busy */
    DS3231_STATUS_PARAM_ERROR = 0x03U,  /*!< Parameter out of range */
    DS3231_STATUS_OSF_SET     = 0x04U   /*!< Oscillator Stop Flag set (power lost) */
} ds3231_status_t;

/* ========================================================================== */
/* Data Structures                                                            */
/* ========================================================================== */

/**
 * @brief Clean human-readable date & time structure.
 */
typedef struct {
    uint8_t seconds;     /*!< 0 to 59 */
    uint8_t minutes;     /*!< 0 to 59 */
    uint8_t hours;       /*!< 0 to 23 (24-hour mode) */
    uint8_t day_of_week; /*!< 1 (Sunday) to 7 (Saturday) */
    uint8_t date;        /*!< 1 to 31 */
    uint8_t month;       /*!< 1 to 12 */
    uint8_t year;        /*!< 0 to 99 (represents 2000 to 2099) */
} ds3231_time_t;

/* ========================================================================== */
/* Public API Functions                                                       */
/* ========================================================================== */

/**
 * @brief Initialize the DS3231 driver.
 */
void DS3231_Init(void);

/**
 * @brief Dynamically set the LPI2C hardware instance (0 for LPI2C0, 1 for LPI2C1, etc.).
 * @param instance Hardware instance index.
 */
void DS3231_SetI2CInstance(uint32_t instance);

/**
 * @brief Get the currently configured LPI2C hardware instance.
 * @return uint32_t Hardware instance index.
 */
uint32_t DS3231_GetI2CInstance(void);

/**
 * @brief Set the RTC Date & Time using a 7-element vector.
 * 
 * Vector layout:
 * time_vec[0] = Year (0 to 99)
 * time_vec[1] = Month (1 to 12)
 * time_vec[2] = Date (1 to 31)
 * time_vec[3] = Day of Week (1 to 7)
 * time_vec[4] = Hours (0 to 23)
 * time_vec[5] = Minutes (0 to 59)
 * time_vec[6] = Seconds (0 to 59)
 * 
 * @param time_vec Pointer to uint8 array of 7 elements.
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_SetTimeArray(const uint8_t *time_vec);

/**
 * @brief Read the current Date & Time into a 7-element vector.
 * 
 * Vector layout:
 * time_vec[0] = Year (0 to 99)
 * time_vec[1] = Month (1 to 12)
 * time_vec[2] = Date (1 to 31)
 * time_vec[3] = Day of Week (1 to 7)
 * time_vec[4] = Hours (0 to 23)
 * time_vec[5] = Minutes (0 to 59)
 * time_vec[6] = Seconds (0 to 59)
 * 
 * @param time_vec Pointer to uint8 output array of 7 elements.
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_GetTimeArray(uint8_t *time_vec);

/**
 * @brief Set Date & Time using the structured type.
 * @param time Pointer to ds3231_time_t structure.
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_SetTime(const ds3231_time_t *time);

/**
 * @brief Read Date & Time into the structured type.
 * @param time Pointer to ds3231_time_t output structure.
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_GetTime(ds3231_time_t *time);

/**
 * @brief Read the internal TCXO die temperature in degrees Celsius.
 * 
 * Resolution: 0.25 degC.
 * 
 * @param temp_c Pointer to single-precision float output variable.
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_GetTemperature(float *temp_c);

/**
 * @brief Read the internal TCXO die temperature as an integer in units of 0.25 degC.
 * Useful for MISRA-compliant safety code without floating point math.
 * (e.g. 100 = 25.00 degC, -4 = -1.00 degC).
 * 
 * @param temp_quarter_deg Pointer to int16_t output variable.
 * @return uint8_t Status code.
 */
uint8_t DS3231_GetTemperatureFixed(int16_t *temp_quarter_deg);

/**
 * @brief Check and optionally clear the Oscillator Stop Flag (OSF).
 * When OSF is set, the RTC oscillator has stopped or power was disconnected,
 * indicating that the current time is untrusted.
 * 
 * @param osf_flag Pointer to boolean output (true if stopped / time invalid).
 * @param clear_if_set If true, writes 0 to clear the OSF flag in the status register.
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_CheckOscillatorStopFlag(bool *osf_flag, bool clear_if_set);

/**
 * @brief Read Time only (Hours, Minutes, Seconds) as separate scalar values.
 * Directly outputs hours (0-23), minutes (0-59), seconds (0-59).
 * Only accesses registers 0x00..0x02 on I2C bus (3 bytes).
 * 
 * @param hours   Pointer to uint8 output variable (0 to 23).
 * @param minutes Pointer to uint8 output variable (0 to 59).
 * @param seconds Pointer to uint8 output variable (0 to 59).
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_GetTimeScalars(uint8_t *hours, uint8_t *minutes, uint8_t *seconds);

/**
 * @brief Set Time only (Hours, Minutes, Seconds).
 * Writes registers 0x00..0x02 on I2C bus (3 bytes).
 * 
 * @param hours   Hours (0 to 23).
 * @param minutes Minutes (0 to 59).
 * @param seconds Seconds (0 to 59).
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_SetTimeScalars(uint8_t hours, uint8_t minutes, uint8_t seconds);

/**
 * @brief Read Date only (Year, Month, Date, DayOfWeek) as separate scalar values.
 * Reads registers 0x03..0x06 on I2C bus (4 bytes).
 * 
 * @param year        Pointer to uint8 output variable (0 to 99, represents 2000-2099).
 * @param month       Pointer to uint8 output variable (1 to 12).
 * @param date        Pointer to uint8 output variable (1 to 31).
 * @param day_of_week Pointer to uint8 output variable (1 to 7).
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_GetDateScalars(uint8_t *year, uint8_t *month, uint8_t *date, uint8_t *day_of_week);

/**
 * @brief Set Date only (Year, Month, Date, DayOfWeek).
 * Writes registers 0x03..0x06 on I2C bus (4 bytes).
 * 
 * @param year        Year (0 to 99).
 * @param month       Month (1 to 12).
 * @param date        Date (1 to 31).
 * @param day_of_week Day of week (1 to 7).
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_SetDateScalars(uint8_t year, uint8_t month, uint8_t date, uint8_t day_of_week);

/* ========================================================================== */
/* Register Bitfield Masks                                                    */
/* ========================================================================== */

/* Control Register (0x0E) */
#define DS3231_CTRL_EOSC            0x80U    /*!< Enable Oscillator (0 = active, 1 = stop on battery) */
#define DS3231_CTRL_BBSQW           0x40U    /*!< Battery-Backed Square-Wave Enable */
#define DS3231_CTRL_CONV            0x20U    /*!< Convert Temperature */
#define DS3231_CTRL_RS2             0x10U    /*!< Rate Select bit 2 */
#define DS3231_CTRL_RS1             0x08U    /*!< Rate Select bit 1 */
#define DS3231_CTRL_INTCN           0x04U    /*!< Interrupt Control (1 = Alarm INT, 0 = SQW) */
#define DS3231_CTRL_A2IE            0x02U    /*!< Alarm 2 Interrupt Enable */
#define DS3231_CTRL_A1IE            0x01U    /*!< Alarm 1 Interrupt Enable */

/* Status Register (0x0F) */
#define DS3231_STAT_OSF             0x80U    /*!< Oscillator Stop Flag */
#define DS3231_STAT_EN32KHZ         0x08U    /*!< Enable 32.768 kHz Output Pin */
#define DS3231_STAT_BSY             0x04U    /*!< Busy Flag (TCXO converting temp) */
#define DS3231_STAT_A2F             0x02U    /*!< Alarm 2 Flag */
#define DS3231_STAT_A1F             0x01U    /*!< Alarm 1 Flag */

/* Square-Wave Output Frequencies */
typedef enum {
    DS3231_SQW_1HZ                  = 0x00U, /*!< 1 Hz */
    DS3231_SQW_1024HZ               = 0x08U, /*!< 1.024 kHz */
    DS3231_SQW_4096HZ               = 0x10U, /*!< 4.096 kHz */
    DS3231_SQW_8192HZ               = 0x18U  /*!< 8.192 kHz */
} ds3231_sqw_freq_t;

/* ========================================================================== */
/* Alarm Configuration & Control                                              */
/* ========================================================================== */

/**
 * @brief Alarm 1 trigger rate modes (mask bits A1M4..A1M1).
 */
typedef enum {
    DS3231_ALARM1_EVERY_SEC          = 0x0FU, /*!< 1111: Trigger once per second */
    DS3231_ALARM1_MATCH_SEC          = 0x0EU, /*!< 1110: Trigger when seconds match (once per minute) */
    DS3231_ALARM1_MATCH_MIN_SEC      = 0x0CU, /*!< 1100: Trigger when minutes & seconds match (once per hour) */
    DS3231_ALARM1_MATCH_HR_MIN_SEC   = 0x08U, /*!< 1000: Trigger when hours, minutes & seconds match (daily) */
    DS3231_ALARM1_MATCH_DATE         = 0x00U  /*!< 0000: Trigger when date, hours, minutes & seconds match */
} ds3231_alarm1_mode_t;

/**
 * @brief Alarm 2 trigger rate modes (mask bits A2M4..A2M2).
 */
typedef enum {
    DS3231_ALARM2_EVERY_MIN          = 0x07U, /*!< 111: Trigger once per minute (at 00 seconds) */
    DS3231_ALARM2_MATCH_MIN          = 0x06U, /*!< 110: Trigger when minutes match (once per hour) */
    DS3231_ALARM2_MATCH_HR_MIN       = 0x04U, /*!< 100: Trigger when hours & minutes match (daily) */
    DS3231_ALARM2_MATCH_DATE         = 0x00U  /*!< 000: Trigger when date, hours & minutes match */
} ds3231_alarm2_mode_t;

/**
 * @brief Configure Alarm 1 and enable interrupt output on the INT/SQW pin.
 * 
 * Sets the match registers (0x07..0x0A), enables INTCN & A1IE in Control register (0x0E),
 * and clears the A1F flag in Status register (0x0F).
 * 
 * @param hours   Match hours (0-23)
 * @param minutes Match minutes (0-59)
 * @param seconds Match seconds (0-59)
 * @param mode    Trigger condition (ds3231_alarm1_mode_t)
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_SetAlarm1(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t mode);

/**
 * @brief Check if Alarm 1 has triggered.
 * 
 * Reads bit 0 (A1F) of Status register (0x0F). If clear_if_fired is true,
 * automatically writes 0 to clear A1F so subsequent alarms can trigger.
 * 
 * @param alarm_fired    Pointer to boolean output (true if alarm matched).
 * @param clear_if_fired If true, clears the A1F flag in hardware.
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_CheckAlarm1(bool *alarm_fired, bool clear_if_fired);

/**
 * @brief Explicitly clear the Alarm 1 flag (A1F) in the status register.
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_ClearAlarm1(void);

/**
 * @brief Configure Alarm 2 (1-minute resolution).
 * 
 * Sets the match registers (0x0B..0x0D), enables INTCN & A2IE in Control register (0x0E),
 * and clears the A2F flag in Status register (0x0F).
 * 
 * @param hours   Match hours (0-23)
 * @param minutes Match minutes (0-59)
 * @param mode    Trigger condition (ds3231_alarm2_mode_t)
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_SetAlarm2(uint8_t hours, uint8_t minutes, uint8_t mode);

/**
 * @brief Check if Alarm 2 has triggered.
 * 
 * @param alarm_fired    Pointer to boolean output (true if alarm matched).
 * @param clear_if_fired If true, clears the A2F flag in hardware.
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_CheckAlarm2(bool *alarm_fired, bool clear_if_fired);

/**
 * @brief Explicitly clear the Alarm 2 flag (A2F) in the status register.
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_ClearAlarm2(void);

/* ========================================================================== */
/* Square-Wave & 32kHz Clock Output Features                                  */
/* ========================================================================== */

/**
 * @brief Enable programmable square-wave output on the INT/SQW pin.
 * 
 * Clears INTCN (bit 2) to route square wave to pin, sets RS2/RS1 bits for frequency,
 * and optionally sets BBSQW to continue square-wave output while on backup battery.
 * 
 * @param freq            Desired frequency (1 Hz, 1.024 kHz, 4.096 kHz, or 8.192 kHz).
 * @param battery_backed  If true, outputs square wave even when main VCC is disconnected.
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_EnableSquareWave(ds3231_sqw_freq_t freq, bool battery_backed);

/**
 * @brief Disable square-wave output (restores INTCN = 1 for alarm interrupt mode).
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_DisableSquareWave(void);

/**
 * @brief Enable or disable the 32.768 kHz open-drain output pin (32K).
 * 
 * Controls bit 3 (EN32kHz) of Status Register (0x0F). When enabled, outputs 32.768 kHz
 * for clocking external microcontrollers or radio modules.
 * 
 * @param enable True to enable 32kHz output, false to set pin to high-impedance.
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_Enable32kHzOutput(bool enable);

/**
 * @brief Check if the 32kHz output pin is currently enabled.
 * @param enabled Pointer to boolean output variable.
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_Is32kHzOutputEnabled(bool *enabled);

/* ========================================================================== */
/* Advanced Thermal & Calibration Features                                    */
/* ========================================================================== */

/**
 * @brief Force an immediate temperature conversion (CONV bit in Reg 0x0E).
 * 
 * Normally temperature is sampled automatically every 64 seconds. This command
 * triggers an immediate reading to update TCXO crystal capacitor arrays.
 * 
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_TriggerTemperatureConversion(void);

/**
 * @brief Check if a temperature conversion is currently in progress (BSY bit in Reg 0x0F).
 * @param busy Pointer to boolean output (true if converting, false if idle).
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_IsTemperatureBusy(bool *busy);

/**
 * @brief Write the Aging Offset register (0x10) for crystal frequency trim.
 * 
 * The aging register adds/subtracts capacitance to the crystal oscillator.
 * Sensitivity is approximately 0.1 ppm per LSB.
 * Value range: -128 to +127 (signed two's complement).
 * 
 * @param offset Signed 8-bit aging trim value.
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_SetAgingOffset(int8_t offset);

/**
 * @brief Read the Aging Offset register (0x10).
 * @param offset Pointer to signed 8-bit output variable.
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_GetAgingOffset(int8_t *offset);

/**
 * @brief Configure oscillator behavior when running on battery backup (EOSC bit in Reg 0x0E).
 * 
 * @param stop_on_battery If true, oscillator is stopped when VCC is cut (saves backup battery shelf life).
 *                        If false (default), oscillator continues running on battery.
 * @return uint8_t Status code (DS3231_STATUS_OK on success).
 */
uint8_t DS3231_SetOscillatorStopOnBattery(bool stop_on_battery);

#ifdef __cplusplus
}
#endif

#endif /* DS3231_H */


