/*
 * File: EEPROM_SmartWheels.c
 *
 * Code generated for Simulink model 'EEPROM_SmartWheels'.
 *
 * Model version                   : 1.83
 * Simulink Coder version          : 24.2 (R2024b) 21-Jun-2024
 * MBDT for S32K1xx Series Version : 4.3.0 (R2016a-R2022a) 13-Sep-2022
 * C/C++ source code generated on  : Sat Sep 26 12:37:07 2026
 *
 * Target selection: mbd_s32k.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "EEPROM_SmartWheels.h"
#include "EEPROM_SmartWheels_private.h"
#include "rtwtypes.h"

lpi2c_master_state_t lpi2cMasterState0;
void lpi2c_MasterCallback(i2c_master_event_t masterEvent, void *userData)
  __attribute__((weak));

/* Block signals (default storage) */
B_EEPROM_SmartWheels_T EEPROM_SmartWheels_B;

/* Real-time model */
static RT_MODEL_EEPROM_SmartWheels_T EEPROM_SmartWheels_M_;
RT_MODEL_EEPROM_SmartWheels_T *const EEPROM_SmartWheels_M =
  &EEPROM_SmartWheels_M_;
void LPI2C_DRV_SetSlaveAddr(uint16_t addr, bool is10BitAddr)
{
  LPI2C_Type *baseAddr = LPI2C0;
  LPI2C_Set_SlaveAddr0(baseAddr, addr);
  if (is10BitAddr) {
    LPI2C_Set_SlaveAddrConfig(baseAddr, LPI2C_SLAVE_ADDR_MATCH_0_10BIT);
  } else {
    LPI2C_Set_SlaveAddrConfig(baseAddr, LPI2C_SLAVE_ADDR_MATCH_0_7BIT);
  }
}

void LPUART1_RxTx_callback(void *driverState, uart_event_t event, void *userData)
{
  if (event == UART_EVENT_RX_FULL) {
    /* Output and update for function-call system: '<Root>/If Action Subsystem' */

    /* CFunction: '<S1>/EEPROM_Write' incorporates:
     *  Constant: '<S1>/Constant'
     */
    /* ST M24C04 EEPROM Write Operation */
    EEPROM_SmartWheels_B.EEPROM_Write = M24C04_Write((1),
      EEPROM_SmartWheels_B.LPUART_Receive, EEPROM_SmartWheels_ConstB.Width_j);
  }

  EEPROM_SmartWheels_B.LPUART_RxTx_ISR_o2 = ((LPUART1)->STAT);
}

/* Model step function */
void EEPROM_SmartWheels_step(void)
{
  /* CFunction: '<Root>/EEPROM_Read' incorporates:
   *  Constant: '<Root>/Constant'
   *  Constant: '<Root>/Constant1'
   */
  /* ST M24C04 EEPROM Read Operation */
  EEPROM_SmartWheels_B.EEPROM_Read_o1 = M24C04_Read((1),
    EEPROM_SmartWheels_B.EEPROM_Read_o2, (2));

  /* If: '<Root>/If' */
  if (EEPROM_SmartWheels_B.EEPROM_Read_o1 == 0) {
    /* Outputs for IfAction SubSystem: '<Root>/If Action Subsystem1' incorporates:
     *  ActionPort: '<S2>/Action Port'
     */
    /* S-Function (lpuart_s32k_transmit): '<S2>/LPUART_Transmit' */
    {
      LPUART_DRV_SendData(1, &EEPROM_SmartWheels_B.EEPROM_Read_o2[0],
                          EEPROM_SmartWheels_ConstB.Width);
    }

    /* End of Outputs for SubSystem: '<Root>/If Action Subsystem1' */
  }

  /* End of If: '<Root>/If' */

  /* S-Function (lpuart_s32k_receive): '<Root>/LPUART_Receive' incorporates:
   *  Constant: '<Root>/Constant2'
   */
  {
    LPUART_DRV_ReceiveData(1, &EEPROM_SmartWheels_B.LPUART_Receive[0], 2U);
  }

  /* End of Outputs for S-Function (lpuart_s32k_rxtx_isr): '<Root>/LPUART_RxTx_ISR' */
}

/* Model initialize function */
void EEPROM_SmartWheels_initialize(void)
{
  /* Start for S-Function (lpuart_s32k_config): '<Root>/LPUART_Config' */
  {
    static lpuart_state_t lpuartState;

    /* Enable clock for PORTC */
    PCC_SetClockMode(PCC, PCC_PORTC_CLOCK, true);

    /* Configure pin for RX function */
    PINS_SetMuxModeSel(PORTC, 6, PORT_MUX_ALT2);

    /* Enable clock for PORTC */
    PCC_SetClockMode(PCC, PCC_PORTC_CLOCK, true);

    /* Configure pin for TX function */
    PINS_SetMuxModeSel(PORTC, 7, PORT_MUX_ALT2);

    /* Set LPUART clock source */
    PCC_SetPeripheralClockControl(PCC, PCC_LPUART1_CLOCK, true,
      CLK_SRC_FIRC_DIV2, 0, 0);

    /* Enable LPUART clock */
    PCC_SetClockMode(PCC, PCC_LPUART1_CLOCK, true);
    const lpuart_user_config_t lpuart1_config = {
      .transferType = LPUART_USING_INTERRUPTS,
      .baudRate = 115200U,
      .parityMode = LPUART_PARITY_DISABLED,
      .stopBitCount = LPUART_ONE_STOP_BIT,
      .bitCountPerChar = LPUART_8_BITS_PER_CHAR,
      .rxDMAChannel = 0U,
      .txDMAChannel = 0U,
    };

    /* Initializes a LPUART instance for operation */
    LPUART_DRV_Init(1, &lpuartState, &lpuart1_config);
  }

  /* Start for S-Function (lpi2c_s32k_config): '<Root>/LPI2C_Config' */
  {
    /* Enable clock for LPI2C and GPIO (SDA, SCL pins) */
    PCC_SetPeripheralClockControl(PCC, LPI2C0_CLK, true, CLK_SRC_SPLL,
      DIVIDE_BY_ONE, MULTIPLY_BY_ONE);
    PCC_SetClockMode(PCC, PORTA_CLK, true);
    PCC_SetClockMode(PCC, PORTA_CLK, true);

    /* Setup i2c instance0 pins */
    pin_settings_config_t i2c0_pins[2]= {
      {
        .base = PORTA,
        .pinPortIdx = 2,
        .pullConfig = PORT_INTERNAL_PULL_UP_ENABLED,
        .passiveFilter = false,
        .driveSelect = PORT_LOW_DRIVE_STRENGTH,
        .mux = PORT_MUX_ALT3,
        .pinLock = false,
        .intConfig = PORT_DMA_INT_DISABLED,
        .clearIntFlag = false,
      },

      {
        .base = PORTA,
        .pinPortIdx = 3,
        .pullConfig = PORT_INTERNAL_PULL_UP_ENABLED,
        .passiveFilter = false,
        .driveSelect = PORT_LOW_DRIVE_STRENGTH,
        .mux = PORT_MUX_ALT3,
        .pinLock = false,
        .intConfig = PORT_DMA_INT_DISABLED,
        .clearIntFlag = false,
      } };

    PINS_DRV_Init(2, i2c0_pins);
    static lpi2c_master_user_config_t lpi2c_MasterConfig0 = {
      .slaveAddress = 1U,     /* default value, will be changed upon transfer */
      .is10bitAddr = false,
      .operatingMode = LPI2C_STANDARD_MODE,
                    /* on S32K144 only Standard mode supported (up to 100kbps)*/
      .baudRate = 100000U,
      .transferType = LPI2C_USING_INTERRUPTS,
      .masterCallback = (i2c_master_callback_t)lpi2c_MasterCallback,
      .callbackParam = NULL
    };

    /* Initialize LPI2C instance with the config above */
    status_t status;
    status = LPI2C_DRV_MasterInit(0, &lpi2c_MasterConfig0, &lpi2cMasterState0);
    EEPROM_SmartWheels_B.LPI2C_Config = status;
  }

  /* SystemInitialize for S-Function (lpuart_s32k_rxtx_isr): '<Root>/LPUART_RxTx_ISR' */
  {
    INT_SYS_SetPriority(LPUART1_RxTx_IRQn, 12);
    LPUART_DRV_InstallRxCallback(1, LPUART1_RxTx_callback, NULL);
  }

  /* End of SystemInitialize for S-Function (lpuart_s32k_rxtx_isr): '<Root>/LPUART_RxTx_ISR' */
}

/* Model terminate function */
void EEPROM_SmartWheels_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
