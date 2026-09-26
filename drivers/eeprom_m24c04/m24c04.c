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

/* Configurable LPI2C instance index (0 = LPI2C0, 1 = LPI2C1) */
static uint32_t s_m24c04_i2c_instance = M24C04_DEFAULT_I2C_INSTANCE;

void M24C04_SetI2CInstance(uint32_t instance)
{
    s_m24c04_i2c_instance = instance;
}

uint32_t M24C04_GetI2CInstance(void)
{
    return s_m24c04_i2c_instance;
}

void M24C04_Init(void)
{
#if M24C04_IS_SIMULATION
    M24C04_SimInitIfNeeded();
#else
    /* Target LPI2C peripheral is initialized via MBDT LPI2C_Config block */
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

        /* Set target slave address on LPI2C instance */
        LPI2C_DRV_MasterSetSlaveAddr(s_m24c04_i2c_instance, (uint16_t)slave_addr, false);

        /* Transmit chunk with STOP condition to trigger internal EEPROM write cycle */
        status_t status = LPI2C_DRV_MasterSendDataBlocking(
            s_m24c04_i2c_instance, tx_buf, (uint32_t)(chunk_size + 1U), true, M24C04_WRITE_TIMEOUT_MS);
        
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

    /* Set target slave address on LPI2C instance */
    LPI2C_DRV_MasterSetSlaveAddr(s_m24c04_i2c_instance, (uint16_t)slave_addr, false);

    /* 1. Transmit word address without STOP (Repeated START) */
    status_t status = LPI2C_DRV_MasterSendDataBlocking(
        s_m24c04_i2c_instance, &word_addr, 1U, false, M24C04_WRITE_TIMEOUT_MS);
    
    if (status != STATUS_SUCCESS) {
        return (uint8_t)M24C04_STATUS_ERROR;
    }

    /* 2. Read sequential data from EEPROM with STOP */
    status = LPI2C_DRV_MasterReceiveDataBlocking(
        s_m24c04_i2c_instance, data, (uint32_t)length, true, 50U);
    
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

/* ========================================================================== */
/* Extended Automotive & EEPROM Feature Implementations                       */
/* ========================================================================== */

uint8_t M24C04_WriteByte(uint16_t mem_addr, uint8_t byte_val)
{
    uint8_t buf = byte_val;
    return M24C04_Write(mem_addr, &buf, 1U);
}

uint8_t M24C04_ReadByte(uint16_t mem_addr, uint8_t *byte_val)
{
    if (byte_val == NULL) {
        return (uint8_t)M24C04_STATUS_PARAM_ERROR;
    }
    uint8_t buf[M24C04_MAX_BUFFER_SIZE];
    uint8_t status = M24C04_Read(mem_addr, buf, 1U);
    if (status == (uint8_t)M24C04_STATUS_OK) {
        *byte_val = buf[0];
    }
    return status;
}

uint8_t M24C04_WriteMulti(uint16_t mem_addr, const uint8_t *data, uint16_t length)
{
    if ((data == NULL) || (length == 0U) || ((mem_addr + length) > M24C04_TOTAL_SIZE)) {
        return (uint8_t)M24C04_STATUS_PARAM_ERROR;
    }

    uint16_t bytes_written = 0U;
    while (bytes_written < length) {
        uint16_t current_addr = mem_addr + bytes_written;
        /* Remaining bytes in current 16-byte physical page */
        uint16_t page_offset = current_addr % M24C04_PAGE_SIZE;
        uint16_t chunk = M24C04_PAGE_SIZE - page_offset;
        if (chunk > (length - bytes_written)) {
            chunk = length - bytes_written;
        }

#if M24C04_IS_SIMULATION
        M24C04_SimInitIfNeeded();
        memcpy(&s_eeprom_sim_mem[current_addr], &data[bytes_written], chunk);
#else
        uint8_t dev_addr = (uint8_t)(M24C04_BASE_ADDR | ((current_addr >> 8U) & 0x01U));
        uint8_t word_addr = (uint8_t)(current_addr & 0xFFU);
        uint8_t tx_buf[M24C04_PAGE_SIZE + 1U];
        tx_buf[0] = word_addr;
        memcpy(&tx_buf[1], &data[bytes_written], chunk);

        status_t status = LPI2C_DRV_MasterSendDataBlocking(
            s_m24c04_i2c_instance, tx_buf, (uint32_t)(chunk + 1U), true, M24C04_WRITE_TIMEOUT_MS);
        if (status != STATUS_SUCCESS) {
            return (uint8_t)M24C04_STATUS_ERROR;
        }
        OSIF_TimeDelay(M24C04_WRITE_CYCLE_DELAY_MS);
#endif
        bytes_written += chunk;
    }

    return (uint8_t)M24C04_STATUS_OK;
}

uint8_t M24C04_ReadMulti(uint16_t mem_addr, uint8_t *data, uint16_t length)
{
    if ((data == NULL) || (length == 0U) || ((mem_addr + length) > M24C04_TOTAL_SIZE)) {
        return (uint8_t)M24C04_STATUS_PARAM_ERROR;
    }

#if M24C04_IS_SIMULATION
    M24C04_SimInitIfNeeded();
    memcpy(data, &s_eeprom_sim_mem[mem_addr], length);
    return (uint8_t)M24C04_STATUS_OK;
#else
    uint16_t bytes_read = 0U;
    while (bytes_read < length) {
        uint16_t current_addr = mem_addr + bytes_read;
        /* Stay within 256-byte block boundary for sequential read */
        uint16_t block_remaining = 256U - (current_addr & 0xFFU);
        uint16_t chunk = (length - bytes_read < block_remaining) ? (length - bytes_read) : block_remaining;

        uint8_t dev_addr = (uint8_t)(M24C04_BASE_ADDR | ((current_addr >> 8U) & 0x01U));
        uint8_t word_addr = (uint8_t)(current_addr & 0xFFU);

        status_t status = LPI2C_DRV_MasterSendDataBlocking(
            s_m24c04_i2c_instance, &word_addr, 1U, false, M24C04_WRITE_TIMEOUT_MS);
        if (status != STATUS_SUCCESS) {
            return (uint8_t)M24C04_STATUS_ERROR;
        }

        status = LPI2C_DRV_MasterReceiveDataBlocking(
            s_m24c04_i2c_instance, &data[bytes_read], (uint32_t)chunk, true, M24C04_WRITE_TIMEOUT_MS);
        if (status != STATUS_SUCCESS) {
            return (uint8_t)M24C04_STATUS_ERROR;
        }
        bytes_read += chunk;
    }
    return (uint8_t)M24C04_STATUS_OK;
#endif
}

uint8_t M24C04_EraseRange(uint16_t start_addr, uint16_t length, uint8_t fill_byte)
{
    if ((start_addr + length) > M24C04_TOTAL_SIZE) {
        return (uint8_t)M24C04_STATUS_PARAM_ERROR;
    }

    uint8_t pattern[M24C04_PAGE_SIZE];
    memset(pattern, fill_byte, sizeof(pattern));

    uint16_t erased = 0U;
    while (erased < length) {
        uint16_t chunk = length - erased;
        if (chunk > M24C04_PAGE_SIZE) {
            chunk = M24C04_PAGE_SIZE;
        }
        uint8_t status = M24C04_WriteMulti(start_addr + erased, pattern, chunk);
        if (status != (uint8_t)M24C04_STATUS_OK) {
            return status;
        }
        erased += chunk;
    }

    return (uint8_t)M24C04_STATUS_OK;
}

uint8_t M24C04_EraseAll(uint8_t fill_byte)
{
    return M24C04_EraseRange(0U, M24C04_TOTAL_SIZE, fill_byte);
}

uint8_t M24C04_WriteWithCRC(uint16_t mem_addr, const uint8_t *data, uint16_t length)
{
    if ((data == NULL) || (length == 0U) || ((mem_addr + length + 1U) > M24C04_TOTAL_SIZE)) {
        return (uint8_t)M24C04_STATUS_PARAM_ERROR;
    }

    /* 1. Write the payload */
    uint8_t status = M24C04_WriteMulti(mem_addr, data, length);
    if (status != (uint8_t)M24C04_STATUS_OK) {
        return status;
    }

    /* 2. Compute and write the CRC-8 as the trailing byte */
    uint8_t crc = M24C04_ComputeCRC8(data, length);
    return M24C04_WriteByte(mem_addr + length, crc);
}

uint8_t M24C04_ReadWithCRC(uint16_t mem_addr, uint8_t *data, uint16_t length)
{
    if ((data == NULL) || (length == 0U) || ((mem_addr + length + 1U) > M24C04_TOTAL_SIZE)) {
        return (uint8_t)M24C04_STATUS_PARAM_ERROR;
    }

    /* 1. Read payload */
    uint8_t status = M24C04_ReadMulti(mem_addr, data, length);
    if (status != (uint8_t)M24C04_STATUS_OK) {
        return status;
    }

    /* 2. Read stored CRC */
    uint8_t stored_crc = 0x00U;
    status = M24C04_ReadByte(mem_addr + length, &stored_crc);
    if (status != (uint8_t)M24C04_STATUS_OK) {
        return status;
    }

    /* 3. Validate computed CRC against stored CRC */
    uint8_t computed_crc = M24C04_ComputeCRC8(data, length);
    if (computed_crc != stored_crc) {
        return (uint8_t)M24C04_STATUS_CRC_ERROR;
    }

    return (uint8_t)M24C04_STATUS_OK;
}

bool M24C04_IsDeviceReady(void)
{
#if M24C04_IS_SIMULATION
    return true;
#else
    uint8_t dummy = 0x00U;
    status_t status = LPI2C_DRV_MasterSendDataBlocking(
        s_m24c04_i2c_instance, &dummy, 1U, true, 5U);
    return (status == STATUS_SUCCESS);
#endif
}

