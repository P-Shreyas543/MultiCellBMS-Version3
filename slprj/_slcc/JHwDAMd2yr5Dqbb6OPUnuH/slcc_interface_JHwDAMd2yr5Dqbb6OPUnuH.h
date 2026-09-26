#include "customcode_JHwDAMd2yr5Dqbb6OPUnuH.h"
#ifdef __cplusplus
extern "C" {
#endif


/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */
DLL_EXPORT_CC extern const char_T *get_dll_checksum_JHwDAMd2yr5Dqbb6OPUnuH(void);
DLL_EXPORT_CC extern void DS3231_Init_JHwDAMd2yr5Dqbb6OPUnuH(void);
DLL_EXPORT_CC extern void DS3231_SetI2CInstance_JHwDAMd2yr5Dqbb6OPUnuH(uint32_T instance);
DLL_EXPORT_CC extern uint32_T DS3231_GetI2CInstance_JHwDAMd2yr5Dqbb6OPUnuH(void);
DLL_EXPORT_CC extern uint8_T DS3231_SetTimeArray_JHwDAMd2yr5Dqbb6OPUnuH(const uint8_T *time_vec);
DLL_EXPORT_CC extern uint8_T DS3231_GetTimeArray_JHwDAMd2yr5Dqbb6OPUnuH(uint8_T *time_vec);
DLL_EXPORT_CC extern uint8_T DS3231_SetTime_JHwDAMd2yr5Dqbb6OPUnuH(const ds3231_time_t *time);
DLL_EXPORT_CC extern uint8_T DS3231_GetTime_JHwDAMd2yr5Dqbb6OPUnuH(ds3231_time_t *time);
DLL_EXPORT_CC extern uint8_T DS3231_GetTemperature_JHwDAMd2yr5Dqbb6OPUnuH(real32_T *temp_c);
DLL_EXPORT_CC extern uint8_T DS3231_GetTemperatureFixed_JHwDAMd2yr5Dqbb6OPUnuH(int16_T *temp_quarter_deg);
DLL_EXPORT_CC extern uint8_T DS3231_CheckOscillatorStopFlag_JHwDAMd2yr5Dqbb6OPUnuH(boolean_T *osf_flag, boolean_T clear_if_set);
DLL_EXPORT_CC extern uint8_T DS3231_GetTimeScalars_JHwDAMd2yr5Dqbb6OPUnuH(uint8_T *hours, uint8_T *minutes, uint8_T *seconds);
DLL_EXPORT_CC extern uint8_T DS3231_SetTimeScalars_JHwDAMd2yr5Dqbb6OPUnuH(uint8_T hours, uint8_T minutes, uint8_T seconds);
DLL_EXPORT_CC extern uint8_T DS3231_GetDateScalars_JHwDAMd2yr5Dqbb6OPUnuH(uint8_T *year, uint8_T *month, uint8_T *date, uint8_T *day_of_week);
DLL_EXPORT_CC extern uint8_T DS3231_SetDateScalars_JHwDAMd2yr5Dqbb6OPUnuH(uint8_T year, uint8_T month, uint8_T date, uint8_T day_of_week);

/* Function Definitions */
DLL_EXPORT_CC const uint8_T *get_checksum_source_info(int32_T *size);
#ifdef __cplusplus
}
#endif

