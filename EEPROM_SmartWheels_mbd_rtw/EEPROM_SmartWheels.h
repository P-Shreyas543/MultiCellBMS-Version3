/*
 * File: EEPROM_SmartWheels.h
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

#ifndef EEPROM_SmartWheels_h_
#define EEPROM_SmartWheels_h_
#ifndef EEPROM_SmartWheels_COMMON_INCLUDES_
#define EEPROM_SmartWheels_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "math.h"
#include "lpuart_driver.h"
#include "lin_lpuart_driver.h"
#include "pcc_hw_access.h"
#include "pins_port_hw_access.h"
#include "lpuart_hw_access.h"
#include "clock_manager.h"
#include "pins_driver.h"
#include "lpi2c_driver.h"
#include "lpi2c_irq.h"
#include "lpi2c_hw_access.h"
#include "m24c04.h"
#endif                                 /* EEPROM_SmartWheels_COMMON_INCLUDES_ */

#include "EEPROM_SmartWheels_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Block signals (default storage) */
typedef struct {
  uint32_T LPUART_RxTx_ISR_o2;         /* '<Root>/LPUART_RxTx_ISR' */
  uint16_T LPI2C_Config;               /* '<Root>/LPI2C_Config' */
  uint8_T EEPROM_Read_o1;              /* '<Root>/EEPROM_Read' */
  uint8_T EEPROM_Read_o2[2];           /* '<Root>/EEPROM_Read' */
  uint8_T LPUART_Receive[2];           /* '<Root>/LPUART_Receive' */
  uint8_T EEPROM_Write;                /* '<S1>/EEPROM_Write' */
} B_EEPROM_SmartWheels_T;

/* Invariant block signals (default storage) */
typedef struct {
  const uint32_T Width;                /* '<S2>/Width' */
  const uint16_T Width_j;              /* '<S1>/Width' */
} ConstB_EEPROM_SmartWheels_T;

/* Real-time Model Data Structure */
struct tag_RTM_EEPROM_SmartWheels_T {
  const char_T * volatile errorStatus;
};

/* Block signals (default storage) */
extern B_EEPROM_SmartWheels_T EEPROM_SmartWheels_B;
extern const ConstB_EEPROM_SmartWheels_T EEPROM_SmartWheels_ConstB;/* constant block i/o */

/* Model entry point functions */
extern void EEPROM_SmartWheels_initialize(void);
extern void EEPROM_SmartWheels_step(void);
extern void EEPROM_SmartWheels_terminate(void);

/* Real-time Model object */
extern RT_MODEL_EEPROM_SmartWheels_T *const EEPROM_SmartWheels_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'EEPROM_SmartWheels'
 * '<S1>'   : 'EEPROM_SmartWheels/If Action Subsystem'
 * '<S2>'   : 'EEPROM_SmartWheels/If Action Subsystem1'
 */
#endif                                 /* EEPROM_SmartWheels_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
