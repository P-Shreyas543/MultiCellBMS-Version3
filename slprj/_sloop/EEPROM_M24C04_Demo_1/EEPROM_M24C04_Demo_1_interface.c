/* Interface file for out-of-process execution of library:
 * EEPROM_M24C04_Demo_1
 */

#include "xil_interface.h"
#include "xil_data_stream.h"

#include "EEPROM_M24C04_Demo_1_interface.h"

#include <stdlib.h>

/* Function Init/Term */
void customcode_EEPROM_M24C04_Demo_1_initializer(void)
{
}

void customcode_EEPROM_M24C04_Demo_1_terminator(void)
{
}

/* Function isDebug */
boolean_T customcode_EEPROM_M24C04_Demo_1_isdebug(void)
{
   return false;
}


 
void __mw_get_oop_cov_hits_vector()
{
}


 
void __mw_set_oop_cov_enable()
{
}


 
void __mw_set_oop_cov_abs_tol()
{
}


 
void __mw_set_oop_cov_rel_tol()
{
}


/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */
void M24C04_Init_EEPROM_M24C04_Demo_1(void)
{
    M24C04_Init();
}

void M24C04_SetI2CInstance_EEPROM_M24C04_Demo_1(uint32_T instance)
{
    M24C04_SetI2CInstance(instance);
}

uint32_T M24C04_GetI2CInstance_EEPROM_M24C04_Demo_1(void)
{
    return M24C04_GetI2CInstance();
}

uint8_T M24C04_Write_EEPROM_M24C04_Demo_1(uint16_T mem_addr, const uint8_T *data, uint16_T length)
{
    return M24C04_Write(mem_addr, data, length);
}

uint8_T M24C04_Read_EEPROM_M24C04_Demo_1(uint16_T mem_addr, uint8_T *data, uint16_T length)
{
    return M24C04_Read(mem_addr, data, length);
}

uint8_T M24C04_ComputeCRC8_EEPROM_M24C04_Demo_1(const uint8_T *data, uint16_T length)
{
    return M24C04_ComputeCRC8(data, length);
}



XIL_INTERFACE_ERROR_CODE xilInitTargetData(void)
{
    return XIL_INTERFACE_SUCCESS;
}



XIL_INTERFACE_ERROR_CODE xilGetHostToTargetData(uint32_T xilFcnId, XIL_COMMAND_TYPE_ENUM xilCommandType, uint32_T xilCommandIdx, XILIOData **xilIOData)
{
    UNUSED_PARAMETER(xilFcnId);
    UNUSED_PARAMETER(xilCommandType);
    UNUSED_PARAMETER(xilCommandIdx);
    UNUSED_PARAMETER(xilIOData);

    return XIL_INTERFACE_UNKNOWN_TID;
}

XIL_INTERFACE_ERROR_CODE xilOutput(uint32_T xilFcnId, uint32_T xilTID)
{
    UNUSED_PARAMETER(xilTID);

    static uint32_T sizeData = (uint32_T) sizeof(uint32_T);
    static uint32_T sizeScopeID = (uint32_T) sizeof(uint8_T);

    switch (xilFcnId) {
    case 0:
    {


        customcode_EEPROM_M24C04_Demo_1_initializer();



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }


        break;
    }

    case 1:
    {


        customcode_EEPROM_M24C04_Demo_1_terminator();



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }


        break;
    }

    case 2:
    {
        uint32_T dataWidth_isAllowToDebug = 0;
        xilReadData((MemUnit_T *) &dataWidth_isAllowToDebug, sizeData);
        uint8_T scopeID_isAllowToDebug = 0;
        xilReadData((MemUnit_T *) &scopeID_isAllowToDebug, sizeScopeID);
        boolean_T isAllowToDebug = 0;



        isAllowToDebug = customcode_EEPROM_M24C04_Demo_1_isdebug();



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (xilWriteData((MemUnit_T *) &isAllowToDebug, (uint32_T) sizeof(boolean_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }


        break;
    }

    case 3:
    {


        __mw_get_oop_cov_hits_vector();



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }


        break;
    }

    case 4:
    {


        __mw_set_oop_cov_enable();



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }


        break;
    }

    case 5:
    {


        __mw_set_oop_cov_abs_tol();



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }


        break;
    }

    case 6:
    {


        __mw_set_oop_cov_rel_tol();



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }


        break;
    }

    case 7:
    {
        uint32_T dataWidth_out = 0;
        xilReadData((MemUnit_T *) &dataWidth_out, sizeData);
        uint8_T scopeID_out = 0;
        xilReadData((MemUnit_T *) &scopeID_out, sizeScopeID);
        uint8_T out = 0;

        uint32_T dataWidth_mem_addr = 0;
        xilReadData((MemUnit_T *) &dataWidth_mem_addr, sizeData);
        uint8_T scopeID_mem_addr = 0;
        xilReadData((MemUnit_T *) &scopeID_mem_addr, sizeScopeID);
        uint16_T mem_addr = 0;
        xilReadData((MemUnit_T *) &mem_addr, (uint32_T) sizeof(uint16_T));

        uint32_T dataWidth_data = 0;
        xilReadData((MemUnit_T *) &dataWidth_data, sizeData);
        uint8_T scopeID_data = 0;
        xilReadData((MemUnit_T *) &scopeID_data, sizeScopeID);
        uint8_T *data = (uint8_T *) calloc((size_t) dataWidth_data, sizeof(uint8_T));
        xilReadData((MemUnit_T *) data, dataWidth_data * ((uint32_T) sizeof(uint8_T)));

        uint32_T dataWidth_length = 0;
        xilReadData((MemUnit_T *) &dataWidth_length, sizeData);
        uint8_T scopeID_length = 0;
        xilReadData((MemUnit_T *) &scopeID_length, sizeScopeID);
        uint16_T length = 0;
        xilReadData((MemUnit_T *) &length, (uint32_T) sizeof(uint16_T));



        out = M24C04_Write_EEPROM_M24C04_Demo_1(mem_addr, data, length);



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (xilWriteData((MemUnit_T *) &out, (uint32_T) sizeof(uint8_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        free(data);

        break;
    }

    case 8:
    {
        uint32_T dataWidth_instance = 0;
        xilReadData((MemUnit_T *) &dataWidth_instance, sizeData);
        uint8_T scopeID_instance = 0;
        xilReadData((MemUnit_T *) &scopeID_instance, sizeScopeID);
        uint32_T instance = 0;
        xilReadData((MemUnit_T *) &instance, (uint32_T) sizeof(uint32_T));



        M24C04_SetI2CInstance_EEPROM_M24C04_Demo_1(instance);



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }


        break;
    }

    case 9:
    {
        uint32_T dataWidth_out = 0;
        xilReadData((MemUnit_T *) &dataWidth_out, sizeData);
        uint8_T scopeID_out = 0;
        xilReadData((MemUnit_T *) &scopeID_out, sizeScopeID);
        uint8_T out = 0;

        uint32_T dataWidth_mem_addr = 0;
        xilReadData((MemUnit_T *) &dataWidth_mem_addr, sizeData);
        uint8_T scopeID_mem_addr = 0;
        xilReadData((MemUnit_T *) &scopeID_mem_addr, sizeScopeID);
        uint16_T mem_addr = 0;
        xilReadData((MemUnit_T *) &mem_addr, (uint32_T) sizeof(uint16_T));

        uint32_T dataWidth_data = 0;
        xilReadData((MemUnit_T *) &dataWidth_data, sizeData);
        uint8_T scopeID_data = 0;
        xilReadData((MemUnit_T *) &scopeID_data, sizeScopeID);
        uint8_T *data = (uint8_T *) calloc((size_t) dataWidth_data, sizeof(uint8_T));
        if (scopeID_data < 2) {
            xilReadData((MemUnit_T *) data, dataWidth_data * ((uint32_T) sizeof(uint8_T)));
        }

        uint32_T dataWidth_length = 0;
        xilReadData((MemUnit_T *) &dataWidth_length, sizeData);
        uint8_T scopeID_length = 0;
        xilReadData((MemUnit_T *) &scopeID_length, sizeScopeID);
        uint16_T length = 0;
        xilReadData((MemUnit_T *) &length, (uint32_T) sizeof(uint16_T));



        out = M24C04_Read_EEPROM_M24C04_Demo_1(mem_addr, data, length);



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (xilWriteData((MemUnit_T *) &out, (uint32_T) sizeof(uint8_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (scopeID_data > 0) {
            if (xilWriteData((MemUnit_T *) data, dataWidth_data * ((uint32_T) sizeof(uint8_T))) != XIL_DATA_STREAM_SUCCESS) {
                return XIL_INTERFACE_COMMS_FAILURE;
            }
        }

        free(data);

        break;
    }

    case 10:
    {


        M24C04_Init_EEPROM_M24C04_Demo_1();



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }


        break;
    }

    case 11:
    {
        uint32_T dataWidth_out = 0;
        xilReadData((MemUnit_T *) &dataWidth_out, sizeData);
        uint8_T scopeID_out = 0;
        xilReadData((MemUnit_T *) &scopeID_out, sizeScopeID);
        uint32_T out = 0;



        out = M24C04_GetI2CInstance_EEPROM_M24C04_Demo_1();



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (xilWriteData((MemUnit_T *) &out, (uint32_T) sizeof(uint32_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }


        break;
    }

    case 12:
    {
        uint32_T dataWidth_out = 0;
        xilReadData((MemUnit_T *) &dataWidth_out, sizeData);
        uint8_T scopeID_out = 0;
        xilReadData((MemUnit_T *) &scopeID_out, sizeScopeID);
        uint8_T out = 0;

        uint32_T dataWidth_data = 0;
        xilReadData((MemUnit_T *) &dataWidth_data, sizeData);
        uint8_T scopeID_data = 0;
        xilReadData((MemUnit_T *) &scopeID_data, sizeScopeID);
        uint8_T *data = (uint8_T *) calloc((size_t) dataWidth_data, sizeof(uint8_T));
        xilReadData((MemUnit_T *) data, dataWidth_data * ((uint32_T) sizeof(uint8_T)));

        uint32_T dataWidth_length = 0;
        xilReadData((MemUnit_T *) &dataWidth_length, sizeData);
        uint8_T scopeID_length = 0;
        xilReadData((MemUnit_T *) &scopeID_length, sizeScopeID);
        uint16_T length = 0;
        xilReadData((MemUnit_T *) &length, (uint32_T) sizeof(uint16_T));



        out = M24C04_ComputeCRC8_EEPROM_M24C04_Demo_1(data, length);



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (xilWriteData((MemUnit_T *) &out, (uint32_T) sizeof(uint8_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        free(data);

        break;
    }

    default:
        return XIL_INTERFACE_UNKNOWN_FCNID;
    }

    return XIL_INTERFACE_SUCCESS;
}

XIL_INTERFACE_ERROR_CODE xilGetTargetToHostData(uint32_T xilFcnId, XIL_COMMAND_TYPE_ENUM xilCommandType, uint32_T xilCommandIdx, XILIOData **xilIOData, MemUnit_T responseId, uint32_T serverFcnId)
{
    UNUSED_PARAMETER(xilFcnId);
    UNUSED_PARAMETER(xilCommandType);
    UNUSED_PARAMETER(xilCommandIdx);
    UNUSED_PARAMETER(xilIOData);
    UNUSED_PARAMETER(responseId);
    UNUSED_PARAMETER(serverFcnId);

    return XIL_INTERFACE_UNKNOWN_TID;
}

XIL_INTERFACE_ERROR_CODE xilGetTargetToHostPreData(uint32_T xilFcnId, XIL_COMMAND_TYPE_ENUM xilCommandType, uint32_T xilCommandIdx, XILIOData **xilIOData, MemUnit_T responseId, uint32_T serverFcnId)
{
    UNUSED_PARAMETER(xilFcnId);
    UNUSED_PARAMETER(xilCommandType);
    UNUSED_PARAMETER(xilCommandIdx);
    UNUSED_PARAMETER(xilIOData);
    UNUSED_PARAMETER(responseId);
    UNUSED_PARAMETER(serverFcnId);

    return XIL_INTERFACE_UNKNOWN_TID;
}

XIL_INTERFACE_ERROR_CODE xilTeardownTargetData(void)
{
    return XIL_INTERFACE_SUCCESS;
}

