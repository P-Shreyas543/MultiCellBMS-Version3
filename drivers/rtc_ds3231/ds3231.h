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

#ifdef __cplusplus
}
#endif

#endif /* DS3231_H */

