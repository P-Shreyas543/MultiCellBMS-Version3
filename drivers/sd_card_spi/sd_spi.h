/**
 * @file sd_spi.h
 * @brief Automotive/Industrial SPI Driver for SD / SDHC / MMC Flash Memory Cards.
 * 
 * Target: NXP S32K1xx (Cortex-M4) / Simulink Model-Based Design (MBDT)
 * Standard: Aligned with AUTOSAR conventions, MISRA-C:2012, and SD Physical Layer Spec.
 * 
 * Application: BMS Blackbox Trip Logging, Crash Data Recorder (EDR), Firmware Storage.
 * 
 * Hardware Characteristics:
 * - Protocol: SPI Mode 0 (CPOL=0, CPHA=0)
 * - Clock Speed: 250-400 kHz during initialization; up to 20-25 MHz for data transfer.
 * - Standard Sector Size: Fixed 512 Bytes per physical block.
 * - Supported Cards: SDSC (Standard Capacity <= 2GB), SDHC (4GB-32GB), SDXC (up to 2TB).
 * - Configurable LPSPI instance (default LPSPI0, selectable for LPSPI1 / LPSPI2).
 */

#ifndef SD_SPI_H
#define SD_SPI_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/* Hardware & Buffer Configuration                                            */
/* ========================================================================== */
#define SD_BLOCK_SIZE               512U     /*!< Standard SD card sector size in bytes */
#define SD_INIT_TIMEOUT_MS          1500U    /*!< Maximum card initialization timeout */
#define SD_READ_TIMEOUT_MS          100U     /*!< Sector read timeout in ms */
#define SD_WRITE_TIMEOUT_MS         250U     /*!< Sector write busy timeout in ms */
#define SD_SIM_NUM_SECTORS          8U       /*!< Number of virtual sectors in PC simulation (4 KB RAM) */

/* Default LPSPI hardware instance (can be overridden at compile-time or runtime) */
#ifndef SD_DEFAULT_SPI_INSTANCE
#define SD_DEFAULT_SPI_INSTANCE     0U
#endif

/* ========================================================================== */
/* Card Type Flags                                                            */
/* ========================================================================== */
#define SD_CARD_TYPE_UNKNOWN        0x00U    /*!< Card not identified or not present */
#define SD_CARD_TYPE_MMC            0x01U    /*!< MultiMediaCard (MMC) */
#define SD_CARD_TYPE_SD1            0x02U    /*!< Standard Capacity SD Card v1 (byte addressing) */
#define SD_CARD_TYPE_SD2_SC         0x04U    /*!< Standard Capacity SD Card v2 (byte addressing) */
#define SD_CARD_TYPE_SD2_HC         0x08U    /*!< High / Extended Capacity SDHC/SDXC (sector addressing) */

/* ========================================================================== */
/* Industry-Standard Return Codes (AUTOSAR aligned)                           */
/* ========================================================================== */
typedef enum {
    SD_STATUS_OK             = 0x00U,  /*!< Operation completed successfully */
    SD_STATUS_ERROR          = 0x01U,  /*!< SPI bus or hardware communication error */
    SD_STATUS_NOT_READY      = 0x02U,  /*!< Card not initialized or busy */
    SD_STATUS_NO_CARD        = 0x03U,  /*!< No SD card detected in slot */
    SD_STATUS_WRITE_PROTECT  = 0x04U,  /*!< Card write-protect switch active */
    SD_STATUS_PARAM_ERROR    = 0x05U,  /*!< Invalid parameter (NULL pointer or sector out of range) */
    SD_STATUS_TIMEOUT        = 0x06U,  /*!< Card response or busy timeout */
    SD_STATUS_CRC_ERROR      = 0x07U   /*!< Data integrity / checksum failure */
} sd_status_t;

/* ========================================================================== */
/* Public API Functions                                                       */
/* ========================================================================== */

/**
 * @brief Initialize the SD card over SPI.
 * 
 * Performs 80 dummy clock cycles with CS deasserted, executes CMD0 (Software Reset),
 * CMD8 (Voltage check), and ACMD41 loop until card exits idle state.
 * 
 * @return uint8_t Status code (SD_STATUS_OK on success).
 */
uint8_t SD_Init(void);

/**
 * @brief Dynamically set the LPSPI hardware instance (0 for LPSPI0, 1 for LPSPI1, 2 for LPSPI2).
 * @param instance Hardware LPSPI instance index.
 */
void SD_SetSPIInstance(uint32_t instance);

/**
 * @brief Get the currently configured LPSPI hardware instance.
 * @return uint32_t Hardware LPSPI instance index.
 */
uint32_t SD_GetSPIInstance(void);

/**
 * @brief Read a single 512-byte sector from the SD card.
 * 
 * Issues CMD17 (READ_SINGLE_BLOCK), waits for Data Token 0xFE, and receives 512 bytes + CRC.
 * 
 * @param sector_num Physical sector index (0 to max card capacity).
 * @param sector_buf Pointer to uint8 array of at least 512 bytes.
 * @return uint8_t Status code (SD_STATUS_OK on success).
 */
uint8_t SD_ReadSector(uint32_t sector_num, uint8_t *sector_buf);

/**
 * @brief Write a single 512-byte sector to the SD card.
 * 
 * Issues CMD24 (WRITE_SINGLE_BLOCK), transmits Data Token 0xFE + 512 bytes + dummy CRC,
 * and waits until card finishes internal programming (busy release).
 * 
 * @param sector_num Physical sector index (0 to max card capacity).
 * @param sector_buf Pointer to uint8 array of 512 bytes to write.
 * @return uint8_t Status code (SD_STATUS_OK on success).
 */
uint8_t SD_WriteSector(uint32_t sector_num, const uint8_t *sector_buf);

/**
 * @brief Read a partial payload from within a sector (useful for compact BMS logging).
 * Reads the 512-byte sector into a temporary buffer and extracts 'length' bytes starting at 'offset'.
 * 
 * @param sector_num Physical sector index.
 * @param offset     Byte offset within sector (0 to 511 - length).
 * @param data       Pointer to destination buffer.
 * @param length     Number of bytes to copy (1 to 512).
 * @return uint8_t Status code.
 */
uint8_t SD_ReadPayload(uint32_t sector_num, uint16_t offset, uint8_t *data, uint16_t length);

/**
 * @brief Write a partial payload into a sector (Read-Modify-Write).
 * 
 * @param sector_num Physical sector index.
 * @param offset     Byte offset within sector (0 to 511 - length).
 * @param data       Pointer to source buffer.
 * @param length     Number of bytes to write (1 to 512).
 * @return uint8_t Status code.
 */
uint8_t SD_WritePayload(uint32_t sector_num, uint16_t offset, const uint8_t *data, uint16_t length);

/**
 * @brief Get the detected SD card type.
 * @return uint8_t Card type bitmask (SD_CARD_TYPE_SD2_HC, SD_CARD_TYPE_SD2_SC, etc.).
 */
uint8_t SD_GetCardType(void);

/**
 * @brief Check if the SD card is currently initialized and ready for read/write.
 * @return true if card is ready, false otherwise.
 */
bool SD_IsReady(void);

/* ========================================================================== */
/* Extended SD Physical Layer Features                                        */
/* ========================================================================== */

/**
 * @brief Structure holding Card Identification (CID) register fields (CMD10).
 */
typedef struct {
    uint8_t  manufacturer_id;    /*!< Manufacturer ID (MID) */
    char     oem_id[3];          /*!< OEM / Application ID (OID, 2 chars + null) */
    char     product_name[6];    /*!< Product Name (PNM, 5 chars + null) */
    uint8_t  product_rev;        /*!< Product Revision (PRV: major.minor) */
    uint32_t serial_number;      /*!< Product Serial Number (PSN) */
    uint16_t mfg_year;           /*!< Manufacturing Year (e.g. 2024) */
    uint8_t  mfg_month;          /*!< Manufacturing Month (1 to 12) */
} sd_cid_t;

/**
 * @brief Read multiple consecutive 512-byte sectors (CMD18).
 * 
 * @param start_sector Physical sector to begin reading from.
 * @param buffer       Pointer to destination memory (must hold sector_count * 512 bytes).
 * @param sector_count Number of consecutive sectors to read.
 * @return uint8_t Status code (SD_STATUS_OK on success).
 */
uint8_t SD_ReadMultipleSectors(uint32_t start_sector, uint8_t *buffer, uint32_t sector_count);

/**
 * @brief Write multiple consecutive 512-byte sectors (CMD25).
 * 
 * @param start_sector Physical sector to begin writing to.
 * @param buffer       Pointer to source memory (must contain sector_count * 512 bytes).
 * @param sector_count Number of consecutive sectors to write.
 * @return uint8_t Status code (SD_STATUS_OK on success).
 */
uint8_t SD_WriteMultipleSectors(uint32_t start_sector, const uint8_t *buffer, uint32_t sector_count);

/**
 * @brief Query total card capacity in sectors by reading the CSD register (CMD9).
 * 
 * @param sector_count Pointer to uint32 output variable.
 * @return uint8_t Status code (SD_STATUS_OK on success).
 */
uint8_t SD_GetSectorCount(uint32_t *sector_count);

/**
 * @brief Read and parse Card Identification (CID) register (CMD10).
 * 
 * @param cid Pointer to destination sd_cid_t structure.
 * @return uint8_t Status code (SD_STATUS_OK on success).
 */
uint8_t SD_GetCardCID(sd_cid_t *cid);

/**
 * @brief Erase an addressable range of physical sectors (CMD32, CMD33, CMD38).
 * 
 * @param start_sector First sector of erase range.
 * @param end_sector   Last sector of erase range.
 * @return uint8_t Status code (SD_STATUS_OK on success).
 */
uint8_t SD_EraseSectors(uint32_t start_sector, uint32_t end_sector);

/**
 * @brief Read Card Status Register (CMD13).
 * 
 * @param status_word Pointer to uint16 output variable.
 * @return uint8_t Status code (SD_STATUS_OK on success).
 */
uint8_t SD_GetCardStatus(uint16_t *status_word);

#ifdef __cplusplus
}
#endif

#endif /* SD_SPI_H */

