/**
 * @file sd_spi.c
 * @brief Automotive/Industrial Driver implementation for SD / SDHC Cards over SPI.
 * 
 * Target: NXP S32K1xx (Cortex-M4) / Simulink Model-Based Design (MBDT)
 * Supports dual-mode: Simulink Desktop Simulation and S32K hardware execution.
 */

#include "sd_spi.h"
#include <string.h>

#if defined(MATLAB_MEX_FILE) || defined(SIMULINK_SIM)
#define SD_IS_SIMULATION 1
#else
#define SD_IS_SIMULATION 0
#include "lpspi_driver.h"
#include "pins_driver.h"
#include "osif.h"
#endif

/* Configurable LPSPI instance (0 = LPSPI0, 1 = LPSPI1, 2 = LPSPI2) */
static uint32_t s_sd_spi_instance = SD_DEFAULT_SPI_INSTANCE;
static uint8_t  s_sd_card_type    = SD_CARD_TYPE_UNKNOWN;
static bool     s_sd_is_ready     = false;

/* ========================================================================== */
/* Simulation Mode: Virtual SD Card in RAM (8 sectors x 512 bytes = 4 KB)     */
/* ========================================================================== */
#if SD_IS_SIMULATION
static uint8_t s_sd_sim_sectors[SD_SIM_NUM_SECTORS][SD_BLOCK_SIZE];
static bool s_sd_sim_initialized = false;

static void SD_SimInitIfNeeded(void)
{
    if (!s_sd_sim_initialized) {
        memset(s_sd_sim_sectors, 0x00, sizeof(s_sd_sim_sectors));
        
        /* Initialize Sector 0 with a realistic BMS trip logging test header */
        const char *header = "MULTICELL_BMS_V3:TRIP_RECORDER_BLOCK_0";
        size_t hlen = strlen(header);
        memcpy(&s_sd_sim_sectors[0][0], header, hlen);
        
        /* Fill remainder of Sector 0 with a test gradient pattern */
        for (uint16_t i = (uint16_t)hlen; i < SD_BLOCK_SIZE; i++) {
            s_sd_sim_sectors[0][i] = (uint8_t)(i & 0xFFU);
        }
        
        s_sd_card_type = SD_CARD_TYPE_SD2_HC; /* Emulate SDHC card */
        s_sd_is_ready = true;
        s_sd_sim_initialized = true;
    }
}
#endif

/* ========================================================================== */
/* Hardware Low-Level SPI Helper Routines                                     */
/* ========================================================================== */
#if !SD_IS_SIMULATION

/* Standard SD SPI Command Codes */
#define CMD0_GO_IDLE_STATE          0U
#define CMD8_SEND_IF_COND           8U
#define CMD16_SET_BLOCKLEN          16U
#define CMD17_READ_SINGLE_BLOCK     17U
#define CMD24_WRITE_SINGLE_BLOCK    24U
#define CMD55_APP_CMD               55U
#define CMD58_READ_OCR              58U
#define ACMD41_SD_SEND_OP_COND      41U

#define SD_TOKEN_DATA_START         0xFEU
#define SD_TOKEN_DATA_ACCEPTED      0x05U

static uint8_t SD_SPI_TransferByte(uint8_t tx_byte)
{
    uint8_t rx_byte = 0xFFU;
    (void)LPSPI_DRV_MasterTransferBlocking(s_sd_spi_instance, &tx_byte, &rx_byte, 1U, 10U);
    return rx_byte;
}

static void SD_SPI_SendDummyClocks(uint16_t count)
{
    for (uint16_t i = 0U; i < count; i++) {
        (void)SD_SPI_TransferByte(0xFFU);
    }
}

static uint8_t SD_SendCommand(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint8_t cmd_frame[6];
    cmd_frame[0] = (uint8_t)(0x40U | (cmd & 0x3FU));
    cmd_frame[1] = (uint8_t)((arg >> 24U) & 0xFFU);
    cmd_frame[2] = (uint8_t)((arg >> 16U) & 0xFFU);
    cmd_frame[3] = (uint8_t)((arg >> 8U) & 0xFFU);
    cmd_frame[4] = (uint8_t)(arg & 0xFFU);
    cmd_frame[5] = crc;

    /* Flush SPI pipeline */
    (void)SD_SPI_TransferByte(0xFFU);

    for (uint8_t i = 0U; i < 6U; i++) {
        (void)SD_SPI_TransferByte(cmd_frame[i]);
    }

    /* Wait for R1 response (MSB must be 0) */
    uint8_t r1 = 0xFFU;
    for (uint8_t i = 0U; i < 10U; i++) {
        r1 = SD_SPI_TransferByte(0xFFU);
        if ((r1 & 0x80U) == 0U) {
            break;
        }
    }

    return r1;
}

static uint8_t SD_SendAppCommand(uint8_t acmd, uint32_t arg)
{
    (void)SD_SendCommand(CMD55_APP_CMD, 0U, 0x65U);
    return SD_SendCommand(acmd, arg, 0x77U);
}

static bool SD_WaitCardReady(uint32_t timeout_ms)
{
    uint32_t start_ms = OSIF_GetMilliseconds();
    while (SD_SPI_TransferByte(0xFFU) != 0xFFU) {
        if ((OSIF_GetMilliseconds() - start_ms) > timeout_ms) {
            return false;
        }
    }
    return true;
}

#endif /* !SD_IS_SIMULATION */

/* ========================================================================== */
/* Public API Implementation                                                  */
/* ========================================================================== */

void SD_SetSPIInstance(uint32_t instance)
{
    s_sd_spi_instance = instance;
}

uint32_t SD_GetSPIInstance(void)
{
    return s_sd_spi_instance;
}

uint8_t SD_GetCardType(void)
{
    return s_sd_card_type;
}

bool SD_IsReady(void)
{
    return s_sd_is_ready;
}

uint8_t SD_Init(void)
{
#if SD_IS_SIMULATION
    SD_SimInitIfNeeded();
    return (uint8_t)SD_STATUS_OK;
#else
    s_sd_is_ready = false;
    s_sd_card_type = SD_CARD_TYPE_UNKNOWN;

    /* 1. Send > 74 dummy clock cycles with CS high to enter SPI mode */
    SD_SPI_SendDummyClocks(10U);

    /* 2. Issue CMD0 (GO_IDLE_STATE) with CRC 0x95 */
    uint8_t r1 = SD_SendCommand(CMD0_GO_IDLE_STATE, 0U, 0x95U);
    if (r1 != 0x01U) {
        return (uint8_t)SD_STATUS_NOT_READY;
    }

    /* 3. Issue CMD8 (SEND_IF_COND) with 3.3V pattern (0x1AA) and CRC 0x87 */
    r1 = SD_SendCommand(CMD8_SEND_IF_COND, 0x000001AAU, 0x87U);
    if (r1 == 0x01U) {
        /* Read 4 response bytes (R7) */
        uint8_t r7[4];
        for (uint8_t i = 0U; i < 4U; i++) {
            r7[i] = SD_SPI_TransferByte(0xFFU);
        }

        if (r7[3] != 0xAAU) {
            return (uint8_t)SD_STATUS_ERROR;
        }

        /* Card is SDv2. Issue ACMD41 with HCS bit (0x40000000) until card is ready */
        uint32_t start_ms = OSIF_GetMilliseconds();
        while (SD_SendAppCommand(ACMD41_SD_SEND_OP_COND, 0x40000000U) != 0x00U) {
            if ((OSIF_GetMilliseconds() - start_ms) > SD_INIT_TIMEOUT_MS) {
                return (uint8_t)SD_STATUS_TIMEOUT;
            }
            OSIF_TimeDelay(10U);
        }

        /* Check OCR register via CMD58 for SDHC/SDXC capacity bit */
        r1 = SD_SendCommand(CMD58_READ_OCR, 0U, 0xFDU);
        if (r1 == 0x00U) {
            uint8_t ocr[4];
            for (uint8_t i = 0U; i < 4U; i++) {
                ocr[i] = SD_SPI_TransferByte(0xFFU);
            }
            /* Bit 30 of OCR indicates block addressing (SDHC / SDXC) */
            if ((ocr[0] & 0x40U) != 0U) {
                s_sd_card_type = SD_CARD_TYPE_SD2_HC;
            } else {
                s_sd_card_type = SD_CARD_TYPE_SD2_SC;
            }
        }
    } else {
        /* Card is SDv1 or MMC */
        s_sd_card_type = SD_CARD_TYPE_SD1;
        uint32_t start_ms = OSIF_GetMilliseconds();
        while (SD_SendAppCommand(ACMD41_SD_SEND_OP_COND, 0U) != 0x00U) {
            if ((OSIF_GetMilliseconds() - start_ms) > SD_INIT_TIMEOUT_MS) {
                return (uint8_t)SD_STATUS_TIMEOUT;
            }
            OSIF_TimeDelay(10U);
        }
    }

    /* Force block length to 512 bytes for Standard Capacity cards */
    if (s_sd_card_type != SD_CARD_TYPE_SD2_HC) {
        (void)SD_SendCommand(CMD16_SET_BLOCKLEN, SD_BLOCK_SIZE, 0xFFU);
    }

    s_sd_is_ready = true;
    return (uint8_t)SD_STATUS_OK;
#endif
}

uint8_t SD_ReadSector(uint32_t sector_num, uint8_t *sector_buf)
{
    if (sector_buf == NULL) {
        return (uint8_t)SD_STATUS_PARAM_ERROR;
    }

#if SD_IS_SIMULATION
    SD_SimInitIfNeeded();
    uint32_t sim_idx = sector_num % SD_SIM_NUM_SECTORS;
    memcpy(sector_buf, s_sd_sim_sectors[sim_idx], SD_BLOCK_SIZE);
    return (uint8_t)SD_STATUS_OK;
#else
    if (!s_sd_is_ready) {
        uint8_t init_stat = SD_Init();
        if (init_stat != (uint8_t)SD_STATUS_OK) {
            return init_stat;
        }
    }

    /* Convert sector index to byte address for SDSC cards */
    uint32_t addr = (s_sd_card_type == SD_CARD_TYPE_SD2_HC) ? sector_num : (sector_num * SD_BLOCK_SIZE);

    /* Send CMD17 (READ_SINGLE_BLOCK) */
    uint8_t r1 = SD_SendCommand(CMD17_READ_SINGLE_BLOCK, addr, 0xFFU);
    if (r1 != 0x00U) {
        return (uint8_t)SD_STATUS_ERROR;
    }

    /* Wait for Data Token (0xFE) */
    uint32_t start_ms = OSIF_GetMilliseconds();
    uint8_t token = 0xFFU;
    while (token != SD_TOKEN_DATA_START) {
        token = SD_SPI_TransferByte(0xFFU);
        if ((OSIF_GetMilliseconds() - start_ms) > SD_READ_TIMEOUT_MS) {
            return (uint8_t)SD_STATUS_TIMEOUT;
        }
    }

    /* Read 512 data bytes */
    for (uint16_t i = 0U; i < SD_BLOCK_SIZE; i++) {
        sector_buf[i] = SD_SPI_TransferByte(0xFFU);
    }

    /* Discard 2-byte CRC */
    (void)SD_SPI_TransferByte(0xFFU);
    (void)SD_SPI_TransferByte(0xFFU);

    return (uint8_t)SD_STATUS_OK;
#endif
}

uint8_t SD_WriteSector(uint32_t sector_num, const uint8_t *sector_buf)
{
    if (sector_buf == NULL) {
        return (uint8_t)SD_STATUS_PARAM_ERROR;
    }

#if SD_IS_SIMULATION
    SD_SimInitIfNeeded();
    uint32_t sim_idx = sector_num % SD_SIM_NUM_SECTORS;
    memcpy(s_sd_sim_sectors[sim_idx], sector_buf, SD_BLOCK_SIZE);
    return (uint8_t)SD_STATUS_OK;
#else
    if (!s_sd_is_ready) {
        uint8_t init_stat = SD_Init();
        if (init_stat != (uint8_t)SD_STATUS_OK) {
            return init_stat;
        }
    }

    /* Convert sector index to byte address for SDSC cards */
    uint32_t addr = (s_sd_card_type == SD_CARD_TYPE_SD2_HC) ? sector_num : (sector_num * SD_BLOCK_SIZE);

    /* Send CMD24 (WRITE_SINGLE_BLOCK) */
    uint8_t r1 = SD_SendCommand(CMD24_WRITE_SINGLE_BLOCK, addr, 0xFFU);
    if (r1 != 0x00U) {
        return (uint8_t)SD_STATUS_ERROR;
    }

    /* Send Data Start Token */
    (void)SD_SPI_TransferByte(SD_TOKEN_DATA_START);

    /* Transmit 512 bytes */
    for (uint16_t i = 0U; i < SD_BLOCK_SIZE; i++) {
        (void)SD_SPI_TransferByte(sector_buf[i]);
    }

    /* Send dummy 16-bit CRC */
    (void)SD_SPI_TransferByte(0xFFU);
    (void)SD_SPI_TransferByte(0xFFU);

    /* Read Data Response Token */
    uint8_t resp = SD_SPI_TransferByte(0xFFU);
    if ((resp & 0x1FU) != SD_TOKEN_DATA_ACCEPTED) {
        return (uint8_t)SD_STATUS_ERROR;
    }

    /* Wait for card programming busy release */
    if (!SD_WaitCardReady(SD_WRITE_TIMEOUT_MS)) {
        return (uint8_t)SD_STATUS_TIMEOUT;
    }

    return (uint8_t)SD_STATUS_OK;
#endif
}

uint8_t SD_ReadPayload(uint32_t sector_num, uint16_t offset, uint8_t *data, uint16_t length)
{
    if ((data == NULL) || (length == 0U) || ((offset + length) > SD_BLOCK_SIZE)) {
        return (uint8_t)SD_STATUS_PARAM_ERROR;
    }

    uint8_t temp_sector[SD_BLOCK_SIZE];
    uint8_t status = SD_ReadSector(sector_num, temp_sector);
    if (status != (uint8_t)SD_STATUS_OK) {
        return status;
    }

    memcpy(data, &temp_sector[offset], length);
    return (uint8_t)SD_STATUS_OK;
}

uint8_t SD_WritePayload(uint32_t sector_num, uint16_t offset, const uint8_t *data, uint16_t length)
{
    if ((data == NULL) || (length == 0U) || ((offset + length) > SD_BLOCK_SIZE)) {
        return (uint8_t)SD_STATUS_PARAM_ERROR;
    }

    /* Read-Modify-Write */
    uint8_t temp_sector[SD_BLOCK_SIZE];
    uint8_t status = SD_ReadSector(sector_num, temp_sector);
    if (status != (uint8_t)SD_STATUS_OK) {
        return status;
    }

    memcpy(&temp_sector[offset], data, length);
    return SD_WriteSector(sector_num, temp_sector);
}

/* ========================================================================== */
/* Extended SD Physical Layer Implementations                                 */
/* ========================================================================== */

uint8_t SD_ReadMultipleSectors(uint32_t start_sector, uint8_t *buffer, uint32_t sector_count)
{
    if ((buffer == NULL) || (sector_count == 0U)) {
        return (uint8_t)SD_STATUS_PARAM_ERROR;
    }

    for (uint32_t i = 0U; i < sector_count; i++) {
        uint8_t status = SD_ReadSector(start_sector + i, &buffer[i * SD_BLOCK_SIZE]);
        if (status != (uint8_t)SD_STATUS_OK) {
            return status;
        }
    }
    return (uint8_t)SD_STATUS_OK;
}

uint8_t SD_WriteMultipleSectors(uint32_t start_sector, const uint8_t *buffer, uint32_t sector_count)
{
    if ((buffer == NULL) || (sector_count == 0U)) {
        return (uint8_t)SD_STATUS_PARAM_ERROR;
    }

    for (uint32_t i = 0U; i < sector_count; i++) {
        uint8_t status = SD_WriteSector(start_sector + i, &buffer[i * SD_BLOCK_SIZE]);
        if (status != (uint8_t)SD_STATUS_OK) {
            return status;
        }
    }
    return (uint8_t)SD_STATUS_OK;
}

uint8_t SD_GetSectorCount(uint32_t *sector_count)
{
    if (sector_count == NULL) {
        return (uint8_t)SD_STATUS_PARAM_ERROR;
    }

#if SD_IS_SIMULATION
    SD_SimInitIfNeeded();
    /* In simulation, report 31,250,000 sectors (~16 GB SDHC card) */
    *sector_count = 31250000U;
    return (uint8_t)SD_STATUS_OK;
#else
    if (!s_sd_is_ready) {
        return (uint8_t)SD_STATUS_NOT_READY;
    }

    /* Send CMD9 (SEND_CSD) */
    uint8_t csd[16];
    uint8_t r1 = SD_SendCommand(9U, 0U, 0xFFU);
    if (r1 != 0x00U) {
        return (uint8_t)SD_STATUS_ERROR;
    }

    /* Wait for start token */
    uint16_t timeout = 1000U;
    while ((SD_SPI_TransferByte(0xFFU) != 0xFEU) && (--timeout > 0U)) {}
    if (timeout == 0U) {
        return (uint8_t)SD_STATUS_TIMEOUT;
    }

    for (uint8_t i = 0U; i < 16U; i++) {
        csd[i] = SD_SPI_TransferByte(0xFFU);
    }
    /* Discard 2 CRC bytes */
    (void)SD_SPI_TransferByte(0xFFU);
    (void)SD_SPI_TransferByte(0xFFU);

    /* Check CSD structure version: bits 127..126 of CSD (byte 0, bits 7..6) */
    if ((csd[0] & 0xC0U) == 0x40U) {
        /* CSD Version 2.0 (SDHC / SDXC) */
        uint32_t c_size = ((uint32_t)(csd[7] & 0x3FU) << 16U) |
                          ((uint32_t)csd[8] << 8U) |
                          ((uint32_t)csd[9]);
        *sector_count = (c_size + 1U) * 1024U;
    } else {
        /* CSD Version 1.0 (Standard SDSC) */
        uint32_t c_size = ((uint32_t)(csd[6] & 0x03U) << 10U) |
                          ((uint32_t)csd[7] << 2U) |
                          ((uint32_t)(csd[8] & 0xC0U) >> 6U);
        uint8_t c_size_mult = (uint8_t)(((csd[9] & 0x03U) << 1U) | ((csd[10] & 0x80U) >> 7U));
        uint8_t read_bl_len = csd[5] & 0x0FU;
        uint32_t block_nr = (c_size + 1U) * (1UL << (c_size_mult + 2U));
        uint32_t block_len = 1UL << read_bl_len;
        *sector_count = (block_nr * block_len) / SD_BLOCK_SIZE;
    }

    return (uint8_t)SD_STATUS_OK;
#endif
}

uint8_t SD_GetCardCID(sd_cid_t *cid)
{
    if (cid == NULL) {
        return (uint8_t)SD_STATUS_PARAM_ERROR;
    }

#if SD_IS_SIMULATION
    SD_SimInitIfNeeded();
    cid->manufacturer_id = 0x03U;       /* SanDisk / Standard Manufacturer */
    strncpy(cid->oem_id, "SD", 2);
    cid->oem_id[2] = '\0';
    strncpy(cid->product_name, "SL16G", 5);
    cid->product_name[5] = '\0';
    cid->product_rev = 0x10U;           /* Rev 1.0 */
    cid->serial_number = 0x12345678U;
    cid->mfg_year = 2024U;
    cid->mfg_month = 6U;
    return (uint8_t)SD_STATUS_OK;
#else
    if (!s_sd_is_ready) {
        return (uint8_t)SD_STATUS_NOT_READY;
    }

    /* Send CMD10 (SEND_CID) */
    uint8_t raw_cid[16];
    uint8_t r1 = SD_SendCommand(10U, 0U, 0xFFU);
    if (r1 != 0x00U) {
        return (uint8_t)SD_STATUS_ERROR;
    }

    uint16_t timeout = 1000U;
    while ((SD_SPI_TransferByte(0xFFU) != 0xFEU) && (--timeout > 0U)) {}
    if (timeout == 0U) {
        return (uint8_t)SD_STATUS_TIMEOUT;
    }

    for (uint8_t i = 0U; i < 16U; i++) {
        raw_cid[i] = SD_SPI_TransferByte(0xFFU);
    }
    (void)SD_SPI_TransferByte(0xFFU);
    (void)SD_SPI_TransferByte(0xFFU);

    cid->manufacturer_id = raw_cid[0];
    cid->oem_id[0] = (char)raw_cid[1];
    cid->oem_id[1] = (char)raw_cid[2];
    cid->oem_id[2] = '\0';
    memcpy(cid->product_name, &raw_cid[3], 5U);
    cid->product_name[5] = '\0';
    cid->product_rev = raw_cid[8];
    cid->serial_number = ((uint32_t)raw_cid[9] << 24U) |
                         ((uint32_t)raw_cid[10] << 16U) |
                         ((uint32_t)raw_cid[11] << 8U) |
                         (uint32_t)raw_cid[12];
    cid->mfg_year = 2000U + ((uint16_t)(raw_cid[13] & 0x0FU) << 4U) | ((raw_cid[14] & 0xF0U) >> 4U);
    cid->mfg_month = raw_cid[14] & 0x0FU;

    return (uint8_t)SD_STATUS_OK;
#endif
}

uint8_t SD_EraseSectors(uint32_t start_sector, uint32_t end_sector)
{
    if (start_sector > end_sector) {
        return (uint8_t)SD_STATUS_PARAM_ERROR;
    }

#if SD_IS_SIMULATION
    SD_SimInitIfNeeded();
    uint32_t s_max = (end_sector < SD_SIM_NUM_SECTORS) ? end_sector : (SD_SIM_NUM_SECTORS - 1U);
    for (uint32_t s = start_sector; s <= s_max; s++) {
        memset(s_sd_sim_sectors[s], 0x00, SD_BLOCK_SIZE);
    }
    return (uint8_t)SD_STATUS_OK;
#else
    if (!s_sd_is_ready) {
        return (uint8_t)SD_STATUS_NOT_READY;
    }

    uint32_t addr_start = (s_sd_card_type & SD_CARD_TYPE_SD2_HC) ? start_sector : (start_sector * SD_BLOCK_SIZE);
    uint32_t addr_end   = (s_sd_card_type & SD_CARD_TYPE_SD2_HC) ? end_sector   : (end_sector * SD_BLOCK_SIZE);

    /* CMD32: Set erase start sector */
    if (SD_SendCommand(32U, addr_start, 0xFFU) != 0x00U) {
        return (uint8_t)SD_STATUS_ERROR;
    }

    /* CMD33: Set erase end sector */
    if (SD_SendCommand(33U, addr_end, 0xFFU) != 0x00U) {
        return (uint8_t)SD_STATUS_ERROR;
    }

    /* CMD38: Execute flash erase */
    if (SD_SendCommand(38U, 0U, 0xFFU) != 0x00U) {
        return (uint8_t)SD_STATUS_ERROR;
    }

    /* Wait for erase busy release */
    if (!SD_WaitCardReady(2000U)) {
        return (uint8_t)SD_STATUS_TIMEOUT;
    }

    return (uint8_t)SD_STATUS_OK;
#endif
}

uint8_t SD_GetCardStatus(uint16_t *status_word)
{
    if (status_word == NULL) {
        return (uint8_t)SD_STATUS_PARAM_ERROR;
    }

#if SD_IS_SIMULATION
    SD_SimInitIfNeeded();
    *status_word = 0x0000U;
    return (uint8_t)SD_STATUS_OK;
#else
    if (!s_sd_is_ready) {
        return (uint8_t)SD_STATUS_NOT_READY;
    }

    /* Send CMD13 (SEND_STATUS) -> returns 2-byte R2 response */
    uint8_t r1 = SD_SendCommand(13U, 0U, 0xFFU);
    uint8_t r2 = SD_SPI_TransferByte(0xFFU);
    *status_word = ((uint16_t)r1 << 8U) | (uint16_t)r2;

    return (uint8_t)SD_STATUS_OK;
#endif
}

