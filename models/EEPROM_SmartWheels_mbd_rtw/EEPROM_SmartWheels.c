/*
 * File: EEPROM_SmartWheels.c
 *
 * Code generated for Simulink model 'EEPROM_SmartWheels'.
 *
 * Model version                   : 1.93
 * Simulink Coder version          : 24.2 (R2024b) 21-Jun-2024
 * MBDT for S32K1xx Series Version : 4.3.0 (R2016a-R2022a) 13-Sep-2022
 * C/C++ source code generated on  : Sat Sep 26 17:03:29 2026
 *
 * Target selection: mbd_s32k.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "EEPROM_SmartWheels.h"
#include "rtwtypes.h"
#include "EEPROM_SmartWheels_private.h"
#include <math.h>
#include "rt_nonfinite.h"

lpi2c_master_state_t lpi2cMasterState0;
void lpi2c_MasterCallback(i2c_master_event_t masterEvent, void *userData)
  __attribute__((weak));

/* Block signals (default storage) */
B_EEPROM_SmartWheels_T EEPROM_SmartWheels_B;

/* Real-time model */
static RT_MODEL_EEPROM_SmartWheels_T EEPROM_SmartWheels_M_;
RT_MODEL_EEPROM_SmartWheels_T *const EEPROM_SmartWheels_M =
  &EEPROM_SmartWheels_M_;

/* Forward declaration for local functions */
static real_T EEPROM_SmartWheels_mod(real_T x);
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

    /* CFunction: '<S1>/RTC_SetTime' */
    /* Write Time to DS3231 */
    EEPROM_SmartWheels_B.RTC_SetTime = DS3231_SetTimeScalars
      (EEPROM_SmartWheels_B.LPUART_Receive[0],
       EEPROM_SmartWheels_B.LPUART_Receive[1],
       EEPROM_SmartWheels_B.LPUART_Receive[2]);
  }

  EEPROM_SmartWheels_B.LPUART_RxTx_ISR_o2 = ((LPUART1)->STAT);
}

/* Function for MATLAB Function: '<S2>/MATLAB Function' */
static real_T EEPROM_SmartWheels_mod(real_T x)
{
  real_T r;
  if (rtIsNaN(x)) {
    r = (rtNaN);
  } else if (rtIsInf(x)) {
    r = (rtNaN);
  } else if (x == 0.0) {
    r = 0.0;
  } else {
    r = fmod(x, 10.0);
    if (r == 0.0) {
      r = 0.0;
    } else if (r < 0.0) {
      r += 10.0;
    }
  }

  return r;
}

real_T rt_roundd_snf(real_T u)
{
  real_T y;
  if (fabs(u) < 4.503599627370496E+15) {
    if (u >= 0.5) {
      y = floor(u + 0.5);
    } else if (u > -0.5) {
      y = u * 0.0;
    } else {
      y = ceil(u - 0.5);
    }
  } else {
    y = u;
  }

  return y;
}

/*
 * Output and update for atomic system:
 *    '<S2>/MATLAB Function'
 *    '<S2>/MATLAB Function1'
 *    '<S2>/MATLAB Function2'
 */
void EEPROM_SmartWhee_MATLABFunction(uint8_T rtu_input, uint8_T rty_ascii[3])
{
  real_T tmp;
  rty_ascii[0] = 0U;
  rty_ascii[1] = 0U;
  rty_ascii[2] = 0U;
  if (rtu_input >= 100) {
    rty_ascii[0] = (uint8_T)((int32_T)floor((real_T)rtu_input / 100.0) + 48);
    tmp = rt_roundd_snf(EEPROM_SmartWheels_mod(floor((real_T)rtu_input / 10.0))
                        + 48.0);
    if (tmp < 256.0) {
      if (tmp >= 0.0) {
        rty_ascii[1] = (uint8_T)tmp;
      } else {
        rty_ascii[1] = 0U;
      }
    } else {
      rty_ascii[1] = MAX_uint8_T;
    }

    tmp = rt_roundd_snf(EEPROM_SmartWheels_mod((real_T)rtu_input) + 48.0);
    if (tmp < 256.0) {
      if (tmp >= 0.0) {
        rty_ascii[2] = (uint8_T)tmp;
      } else {
        rty_ascii[2] = 0U;
      }
    } else {
      rty_ascii[2] = MAX_uint8_T;
    }
  } else if (rtu_input >= 10) {
    rty_ascii[0] = (uint8_T)((int32_T)floor((real_T)rtu_input / 10.0) + 48);
    tmp = rt_roundd_snf(EEPROM_SmartWheels_mod((real_T)rtu_input) + 48.0);
    if (tmp < 256.0) {
      if (tmp >= 0.0) {
        rty_ascii[1] = (uint8_T)tmp;
      } else {
        rty_ascii[1] = 0U;
      }
    } else {
      rty_ascii[1] = MAX_uint8_T;
    }
  } else {
    rty_ascii[0] = (uint8_T)(rtu_input + 48);
  }
}

/* Model step function */
void EEPROM_SmartWheels_step(void)
{
  uint8_T rtb_ascii[3];
  uint8_T rtb_ascii_b[3];
  uint8_T rtb_ascii_l[3];

  /* S-Function (lpuart_s32k_receive): '<Root>/LPUART_Receive' incorporates:
   *  Constant: '<Root>/Constant2'
   */
  {
    LPUART_DRV_ReceiveData(1, &EEPROM_SmartWheels_B.LPUART_Receive[0], 3U);
  }

  /* End of Outputs for S-Function (lpuart_s32k_rxtx_isr): '<Root>/LPUART_RxTx_ISR' */

  /* CFunction: '<Root>/RTC_GetTime' */
  /* Read Time: Hours (0-23), Minutes (0-59), Seconds (0-59) */
  EEPROM_SmartWheels_B.RTC_GetTime_o1 = DS3231_GetTimeScalars
    (&EEPROM_SmartWheels_B.RTC_GetTime_o2, &EEPROM_SmartWheels_B.RTC_GetTime_o3,
     &EEPROM_SmartWheels_B.RTC_GetTime_o4);

  /* If: '<Root>/If' */
  if (EEPROM_SmartWheels_B.RTC_GetTime_o1 == 0) {
    /* Outputs for IfAction SubSystem: '<Root>/If Action Subsystem1' incorporates:
     *  ActionPort: '<S2>/Action Port'
     */
    /* MATLAB Function: '<S2>/MATLAB Function' */
    EEPROM_SmartWhee_MATLABFunction(EEPROM_SmartWheels_B.RTC_GetTime_o2,
      rtb_ascii_l);

    /* MATLAB Function: '<S2>/MATLAB Function1' */
    EEPROM_SmartWhee_MATLABFunction(EEPROM_SmartWheels_B.RTC_GetTime_o3,
      rtb_ascii_b);

    /* MATLAB Function: '<S2>/MATLAB Function2' */
    EEPROM_SmartWhee_MATLABFunction(EEPROM_SmartWheels_B.RTC_GetTime_o4,
      rtb_ascii);

    /* SignalConversion generated from: '<S2>/LPUART_Transmit' incorporates:
     *  Constant: '<S2>/Constant'
     *  Constant: '<S2>/Constant1'
     *  Constant: '<S2>/Constant2'
     *  Constant: '<S2>/Constant3'
     */
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[0] = 72U;
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[1] = 72U;
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[2] = 58U;
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[3] = 32U;
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[4] = rtb_ascii_l[0];
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[5] = rtb_ascii_l[1];
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[6] = rtb_ascii_l[2];
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[7] = 77U;
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[8] = 77U;
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[9] = 58U;
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[10] = 32U;
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[11] = rtb_ascii_b[0];
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[12] = rtb_ascii_b[1];
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[13] = rtb_ascii_b[2];
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[14] = 83U;
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[15] = 83U;
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[16] = 58U;
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[17] = 32U;
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[18] = rtb_ascii[0];
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[19] = rtb_ascii[1];
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[20] = rtb_ascii[2];
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[21] = 13U;
    EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra[22] = 10U;

    /* S-Function (lpuart_s32k_transmit): '<S2>/LPUART_Transmit' */
    {
      LPUART_DRV_SendData(1,
                          &EEPROM_SmartWheels_B.TmpSignalConversionAtLPUART_Tra
                          [0], EEPROM_SmartWheels_ConstB.Width);
    }

    /* End of Outputs for SubSystem: '<Root>/If Action Subsystem1' */
  }

  /* End of If: '<Root>/If' */
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
