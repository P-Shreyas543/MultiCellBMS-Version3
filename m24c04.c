/**
 * @file m24c04.c
 * @brief Automotive/Industrial Driver implementation for ST M24C04-WMN6TP EEPROM.
 * 
 * Target: NXP S32K144 / Simulink Model-Based Design (MBDT)
 * Supports dual-mode: Simulink Desktop Simulation and S32K hardware execution.
 */

#include "m24c04.h"
#include <string.h>

#if defined(MATLAB_MEX_FILE) || defined(SIMULINK_SIM)
/* Simulation mode: runs on PC inside Simulink */
#define M24C04_IS_SIMULATION 1
#else
/* Hardware mode: runs on NXP S32K144 target */
#define M24C04_IS_SIMULATION 0
#include "lpi2c_driver.h"
#include "osif.h"
#endif

/* Virtual EEPROM memory array for desktop simulation (512 bytes initialized to 0xFF) */
#if M24C04_IS_SIMULATION
static uint8_t s_eeprom_sim_mem[M24C04_TOTAL_SIZE];
static bool s_sim_initialized = false;

static void M24C04_SimInitIfNeeded(void)
{
    if (!s_sim_initialized) {
        memset(s_eeprom_sim_mem, 0xFF, sizeof(s_eeprom_sim_mem));
        s_sim_initialized = true;
    }
}
#endif

void M24C04_Init(void)
{
#if M24C04_IS_SIMULATION
    M24C04_SimInitIfNeeded();
#else
    /* Target LPI2C0 peripheral is initialized via MBDT LPI2C_Config block */
#endif
}

uint8_t M24C04_Write(uint16_t mem_addr, const uint8_t *data, uint16_t length)
{
    /* Strict parameter validation */
    if (data == NULL) {
        return (uint8_t)M24C04_STATUS_PARAM_ERROR;
    }
    if ((length == 0U) || (length > M24C04_MAX_BUFFER_SIZE)) {
        return (uint8_t)M24C04_STATUS_PARAM_ERROR;
    }
    if (((uint32_t)mem_addr + (uint32_t)length) > (uint32_t)M24C04_TOTAL_SIZE) {
        return (uint8_t)M24C04_STATUS_PARAM_ERROR;
    }

#if M24C04_IS_SIMULATION
    M24C04_SimInitIfNeeded();

    /* Write directly to simulated EEPROM memory */
    for (uint16_t i = 0U; i < length; i++) {
        s_eeprom_sim_mem[mem_addr + i] = data[i];
    }
    return (uint8_t)M24C04_STATUS_OK;

#else
    uint16_t bytes_written = 0U;

    while (bytes_written < length) {
        uint16_t current_addr = mem_addr + bytes_written;

        /* Calculate 7-bit slave address: 0x50 (A8=0) or 0x51 (A8=1) */
        uint8_t slave_addr = (uint8_t)(M24C04_BASE_ADDR | ((current_addr >> 8U) & 0x01U));
        uint8_t word_addr = (uint8_t)(current_addr & 0xFFU);

        /* Calculate bytes remaining in current 16-byte physical page */
        uint16_t page_offset = current_addr & (M24C04_PAGE_SIZE - 1U);
        uint16_t bytes_left_in_page = M24C04_PAGE_SIZE - page_offset;
        uint16_t chunk_size = length - bytes_written;
        if (chunk_size > bytes_left_in_page) {
            chunk_size = bytes_left_in_page;
        }

        /* Prepare transmit packet: [Word Address, Data0, Data1, ...] */
        uint8_t tx_buf[M24C04_PAGE_SIZE + 1U];
        tx_buf[0] = word_addr;
        for (uint16_t i = 0U; i < chunk_size; i++) {
            tx_buf[1U + i] = data[bytes_written + i];
        }

        /* Set target slave address on LPI2C instance 0 */
        LPI2C_DRV_MasterSetSlaveAddr(0U, (uint16_t)slave_addr, false);

        /* Transmit chunk with STOP condition to trigger internal EEPROM write cycle */
        status_t status = LPI2C_DRV_MasterSendDataBlocking(
            0U, tx_buf, (uint32_t)(chunk_size + 1U), true, M24C04_WRITE_TIMEOUT_MS);
        
        if (status != STATUS_SUCCESS) {
            return (uint8_t)M24C04_STATUS_ERROR;
        }

        bytes_written += chunk_size;

        /* Wait 6 ms for internal self-timed EEPROM write cycle (tW max = 5 ms) */
        OSIF_TimeDelay(M24C04_WRITE_CYCLE_DELAY_MS);
    }

    return (uint8_t)M24C04_STATUS_OK;
#endif
}

uint8_t M24C04_Read(uint16_t mem_addr, uint8_t *data, uint16_t length)
{
    /* Strict parameter validation */
    if (data == NULL) {
        return (uint8_t)M24C04_STATUS_PARAM_ERROR;
    }
    if ((length == 0U) || (length > M24C04_MAX_BUFFER_SIZE)) {
        return (uint8_t)M24C04_STATUS_PARAM_ERROR;
    }
    if (((uint32_t)mem_addr + (uint32_t)length) > (uint32_t)M24C04_TOTAL_SIZE) {
        return (uint8_t)M24C04_STATUS_PARAM_ERROR;
    }

    /* Initialize buffer up to requested length for deterministic behavior */
    memset(data, 0x00, length);

#if M24C04_IS_SIMULATION
    M24C04_SimInitIfNeeded();

    /* Read from simulated EEPROM array */
    for (uint16_t i = 0U; i < length; i++) {
        data[i] = s_eeprom_sim_mem[mem_addr + i];
    }
    return (uint8_t)M24C04_STATUS_OK;

#else
    /* Calculate 7-bit slave address: 0x50 (A8=0) or 0x51 (A8=1) */
    uint8_t slave_addr = (uint8_t)(M24C04_BASE_ADDR | ((mem_addr >> 8U) & 0x01U));
    uint8_t word_addr = (uint8_t)(mem_addr & 0xFFU);

    /* Set target slave address on LPI2C instance 0 */
    LPI2C_DRV_MasterSetSlaveAddr(0U, (uint16_t)slave_addr, false);

    /* 1. Transmit word address without STOP (Repeated START) */
    status_t status = LPI2C_DRV_MasterSendDataBlocking(
        0U, &word_addr, 1U, false, M24C04_WRITE_TIMEOUT_MS);
    
    if (status != STATUS_SUCCESS) {
        return (uint8_t)M24C04_STATUS_ERROR;
    }

    /* 2. Read sequential data from EEPROM with STOP */
    status = LPI2C_DRV_MasterReceiveDataBlocking(
        0U, data, (uint32_t)length, true, 50U);
    
    if (status != STATUS_SUCCESS) {
        return (uint8_t)M24C04_STATUS_ERROR;
    }

    return (uint8_t)M24C04_STATUS_OK;
#endif
}

uint8_t M24C04_ComputeCRC8(const uint8_t *data, uint16_t length)
{
    /* SAE J1850 CRC-8 calculation (Poly = 0x1D, Init = 0xFF, XorOut = 0xFF) */
    uint8_t crc = 0xFFU;
    
    if (data == NULL) {
        return 0x00U;
    }

    for (uint16_t i = 0U; i < length; i++) {
        crc ^= data[i];
        for (uint8_t bit = 0U; bit < 8U; bit++) {
            if ((crc & 0x80U) != 0U) {
                crc = (uint8_t)((crc << 1U) ^ 0x1DU);
            } else {
                crc = (uint8_t)(crc << 1U);
            }
        }
    }

    return (uint8_t)(crc ^ 0xFFU);
}
