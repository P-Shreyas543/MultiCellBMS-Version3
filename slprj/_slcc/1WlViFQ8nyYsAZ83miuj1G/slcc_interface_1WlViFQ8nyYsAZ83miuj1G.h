#include "customcode_1WlViFQ8nyYsAZ83miuj1G.h"
#ifdef __cplusplus
extern "C" {
#endif


/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */
DLL_EXPORT_CC extern const char_T *get_dll_checksum_1WlViFQ8nyYsAZ83miuj1G(void);
DLL_EXPORT_CC extern void M24C04_Init_1WlViFQ8nyYsAZ83miuj1G(void);
DLL_EXPORT_CC extern void M24C04_SetI2CInstance_1WlViFQ8nyYsAZ83miuj1G(uint32_T instance);
DLL_EXPORT_CC extern uint32_T M24C04_GetI2CInstance_1WlViFQ8nyYsAZ83miuj1G(void);
DLL_EXPORT_CC extern uint8_T M24C04_Write_1WlViFQ8nyYsAZ83miuj1G(uint16_T mem_addr, const uint8_T *data, uint16_T length);
DLL_EXPORT_CC extern uint8_T M24C04_Read_1WlViFQ8nyYsAZ83miuj1G(uint16_T mem_addr, uint8_T *data, uint16_T length);
DLL_EXPORT_CC extern uint8_T M24C04_ComputeCRC8_1WlViFQ8nyYsAZ83miuj1G(const uint8_T *data, uint16_T length);

/* Function Definitions */
DLL_EXPORT_CC const uint8_T *get_checksum_source_info(int32_T *size);
#ifdef __cplusplus
}
#endif

