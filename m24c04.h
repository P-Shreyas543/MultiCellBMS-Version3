/**
 * @file m24c04.h
 * @brief Automotive/Industrial Driver for ST M24C04-WMN6TP 4-Kbit I2C EEPROM.
 * 
 * Target: NXP S32K1xx (Cortex-M4) / Simulink Model-Based Design (MBDT)
 * Standard: Aligned with AUTOSAR NvM / MemIf conventions and MISRA-C:2012.
 * 
 * Hardware Characteristics:
 * - Total Memory: 4 Kbit = 512 Bytes (0x0000 - 0x01FF)
 * - Organization: 2 blocks x 256 bytes
 * - Page Write Buffer: 16 Bytes (writes must not cross 16-byte boundaries)
 * - Maximum Payload Buffer: 32 Bytes per Simulink block transaction
 * - I2C Device Addresses (7-bit):
 *     - Memory Range 0x000 - 0x0FF (A8 = 0): 0x50
 *     - Memory Range 0x100 - 0x1FF (A8 = 1): 0x51
 * - Write Cycle Time (tW): max 5 ms
 */

#ifndef M24C04_H
#define M24C04_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/* Hardware & Buffer Configuration                                            */
/* ========================================================================== */
#define M24C04_TOTAL_SIZE           512U     /* Total memory capacity in bytes */
#define M24C04_PAGE_SIZE            16U      /* Physical page write buffer size */
#define M24C04_BASE_ADDR            0x50U    /* Base 7-bit I2C slave address (A8 = 0) */
#define M24C04_MAX_BUFFER_SIZE      32U      /* Standardized array payload width (bytes) */
#define M24C04_WRITE_TIMEOUT_MS     25U      /* I2C transmission timeout in ms */
#define M24C04_WRITE_CYCLE_DELAY_MS 6U       /* Delay for internal write cycle (tW max = 5 ms) */

/* ========================================================================== */
/* Industry-Standard Return Codes (AUTOSAR NvM aligned)                       */
/* ========================================================================== */
typedef enum {
    M24C04_STATUS_OK          = 0x00U,  /*!< Job processed successfully (E_OK) */
    M24C04_STATUS_ERROR       = 0x01U,  /*!< Hardware I2C bus failure / NACK (E_NOT_OK) */
    M24C04_STATUS_BUSY        = 0x02U,  /*!< EEPROM write cycle in progress */
    M24C04_STATUS_PARAM_ERROR = 0x03U,  /*!< Invalid parameter (address out of range or len > 32) */
    M24C04_STATUS_CRC_ERROR   = 0x04U   /*!< Data integrity / CRC mismatch */
} m24c04_status_t;

/* ========================================================================== */
/* Public API Functions                                                       */
/* ========================================================================== */

/**
 * @brief Initialize the M24C04 EEPROM driver.
 */
void M24C04_Init(void);

/**
 * @brief Write up to 32 bytes into the M24C04 EEPROM.
 * 
 * Automatically segments writes across 16-byte physical page boundaries
 * and sets the proper A8 memory block address bit (0x50 / 0x51).
 * 
 * @param mem_addr Start memory address (0 to 511).
 * @param data     Pointer to 32-byte uint8 array to write.
 * @param length   Number of active bytes to write (1 to 32).
 * @return uint8_t Status code (M24C04_STATUS_OK on success).
 */
uint8_t M24C04_Write(uint16_t mem_addr, const uint8_t *data, uint16_t length);

/**
 * @brief Read up to 32 bytes from the M24C04 EEPROM.
 * 
 * Performs random-address sequential read. Zero-pads unused buffer positions
 * when length < 32 for deterministic downstream data integrity.
 * 
 * @param mem_addr Start memory address (0 to 511).
 * @param data     Pointer to 32-byte uint8 output buffer.
 * @param length   Number of active bytes to read (1 to 32).
 * @return uint8_t Status code (M24C04_STATUS_OK on success).
 */
uint8_t M24C04_Read(uint16_t mem_addr, uint8_t *data, uint16_t length);

/**
 * @brief Compute standard SAE J1850 CRC-8 for automotive data integrity.
 * 
 * Polynomial: x^8 + x^4 + x^3 + x^2 + 1 (0x1D), Initial: 0xFF.
 * 
 * @param data   Pointer to byte array.
 * @param length Number of bytes to compute CRC over.
 * @return uint8_t Calculated CRC-8 checksum.
 */
uint8_t M24C04_ComputeCRC8(const uint8_t *data, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* M24C04_H */
