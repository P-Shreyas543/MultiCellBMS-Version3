/* Interface file for out-of-process execution of library:
 * RTC_DS3231_Demo_6
 */

#include "xil_interface.h"
#include "xil_data_stream.h"

#include "RTC_DS3231_Demo_6_interface.h"

#include <stdlib.h>

#include <string.h>

/* Function Init/Term */
void customcode_RTC_DS3231_Demo_6_initializer(void)
{
}

void customcode_RTC_DS3231_Demo_6_terminator(void)
{
}

/* Function isDebug */
boolean_T customcode_RTC_DS3231_Demo_6_isdebug(void)
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
void DS3231_Init_RTC_DS3231_Demo_6(void)
{
    DS3231_Init();
}

void DS3231_SetI2CInstance_RTC_DS3231_Demo_6(uint32_T instance)
{
    DS3231_SetI2CInstance(instance);
}

uint32_T DS3231_GetI2CInstance_RTC_DS3231_Demo_6(void)
{
    return DS3231_GetI2CInstance();
}

uint8_T DS3231_SetTimeArray_RTC_DS3231_Demo_6(const uint8_T *time_vec)
{
    return DS3231_SetTimeArray(time_vec);
}

uint8_T DS3231_GetTimeArray_RTC_DS3231_Demo_6(uint8_T *time_vec)
{
    return DS3231_GetTimeArray(time_vec);
}

uint8_T DS3231_SetTime_RTC_DS3231_Demo_6(const ds3231_time_t *time)
{
    return DS3231_SetTime(time);
}

uint8_T DS3231_GetTime_RTC_DS3231_Demo_6(ds3231_time_t *time)
{
    return DS3231_GetTime(time);
}

uint8_T DS3231_GetTemperature_RTC_DS3231_Demo_6(real32_T *temp_c)
{
    return DS3231_GetTemperature(temp_c);
}

uint8_T DS3231_GetTemperatureFixed_RTC_DS3231_Demo_6(int16_T *temp_quarter_deg)
{
    return DS3231_GetTemperatureFixed(temp_quarter_deg);
}

uint8_T DS3231_CheckOscillatorStopFlag_RTC_DS3231_Demo_6(boolean_T *osf_flag, boolean_T clear_if_set)
{
    return DS3231_CheckOscillatorStopFlag(osf_flag, clear_if_set);
}

uint8_T DS3231_GetTimeScalars_RTC_DS3231_Demo_6(uint8_T *hours, uint8_T *minutes, uint8_T *seconds)
{
    return DS3231_GetTimeScalars(hours, minutes, seconds);
}

uint8_T DS3231_SetTimeScalars_RTC_DS3231_Demo_6(uint8_T hours, uint8_T minutes, uint8_T seconds)
{
    return DS3231_SetTimeScalars(hours, minutes, seconds);
}

uint8_T DS3231_GetDateScalars_RTC_DS3231_Demo_6(uint8_T *year, uint8_T *month, uint8_T *date, uint8_T *day_of_week)
{
    return DS3231_GetDateScalars(year, month, date, day_of_week);
}

uint8_T DS3231_SetDateScalars_RTC_DS3231_Demo_6(uint8_T year, uint8_T month, uint8_T date, uint8_T day_of_week)
{
    return DS3231_SetDateScalars(year, month, date, day_of_week);
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


        customcode_RTC_DS3231_Demo_6_initializer();



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }


        break;
    }

    case 1:
    {


        customcode_RTC_DS3231_Demo_6_terminator();



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



        isAllowToDebug = customcode_RTC_DS3231_Demo_6_isdebug();



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

        uint32_T dataWidth_time = 0;
        xilReadData((MemUnit_T *) &dataWidth_time, sizeData);
        uint8_T scopeID_time = 0;
        xilReadData((MemUnit_T *) &scopeID_time, sizeScopeID);
        ds3231_time_t *time = (ds3231_time_t *) calloc((size_t) dataWidth_time, sizeof(ds3231_time_t));
        xilReadData((MemUnit_T *) time, dataWidth_time * ((uint32_T) sizeof(ds3231_time_t)));



        out = DS3231_SetTime_RTC_DS3231_Demo_6(time);



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (xilWriteData((MemUnit_T *) &out, (uint32_T) sizeof(uint8_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        free(time);

        break;
    }

    case 8:
    {
        uint32_T dataWidth_out = 0;
        xilReadData((MemUnit_T *) &dataWidth_out, sizeData);
        uint8_T scopeID_out = 0;
        xilReadData((MemUnit_T *) &scopeID_out, sizeScopeID);
        uint8_T out = 0;

        uint32_T dataWidth_hours = 0;
        xilReadData((MemUnit_T *) &dataWidth_hours, sizeData);
        uint8_T scopeID_hours = 0;
        xilReadData((MemUnit_T *) &scopeID_hours, sizeScopeID);
        uint8_T hours = 0;
        xilReadData((MemUnit_T *) &hours, (uint32_T) sizeof(uint8_T));

        uint32_T dataWidth_minutes = 0;
        xilReadData((MemUnit_T *) &dataWidth_minutes, sizeData);
        uint8_T scopeID_minutes = 0;
        xilReadData((MemUnit_T *) &scopeID_minutes, sizeScopeID);
        uint8_T minutes = 0;
        xilReadData((MemUnit_T *) &minutes, (uint32_T) sizeof(uint8_T));

        uint32_T dataWidth_seconds = 0;
        xilReadData((MemUnit_T *) &dataWidth_seconds, sizeData);
        uint8_T scopeID_seconds = 0;
        xilReadData((MemUnit_T *) &scopeID_seconds, sizeScopeID);
        uint8_T seconds = 0;
        xilReadData((MemUnit_T *) &seconds, (uint32_T) sizeof(uint8_T));



        out = DS3231_SetTimeScalars_RTC_DS3231_Demo_6(hours, minutes, seconds);



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (xilWriteData((MemUnit_T *) &out, (uint32_T) sizeof(uint8_T)) != XIL_DATA_STREAM_SUCCESS) {
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

        uint32_T dataWidth_time_vec = 0;
        xilReadData((MemUnit_T *) &dataWidth_time_vec, sizeData);
        uint8_T scopeID_time_vec = 0;
        xilReadData((MemUnit_T *) &scopeID_time_vec, sizeScopeID);
        uint8_T *time_vec = (uint8_T *) calloc((size_t) dataWidth_time_vec, sizeof(uint8_T));
        xilReadData((MemUnit_T *) time_vec, dataWidth_time_vec * ((uint32_T) sizeof(uint8_T)));



        out = DS3231_SetTimeArray_RTC_DS3231_Demo_6(time_vec);



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (xilWriteData((MemUnit_T *) &out, (uint32_T) sizeof(uint8_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        free(time_vec);

        break;
    }

    case 10:
    {
        uint32_T dataWidth_instance = 0;
        xilReadData((MemUnit_T *) &dataWidth_instance, sizeData);
        uint8_T scopeID_instance = 0;
        xilReadData((MemUnit_T *) &scopeID_instance, sizeScopeID);
        uint32_T instance = 0;
        xilReadData((MemUnit_T *) &instance, (uint32_T) sizeof(uint32_T));



        DS3231_SetI2CInstance_RTC_DS3231_Demo_6(instance);



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
        uint8_T out = 0;

        uint32_T dataWidth_year = 0;
        xilReadData((MemUnit_T *) &dataWidth_year, sizeData);
        uint8_T scopeID_year = 0;
        xilReadData((MemUnit_T *) &scopeID_year, sizeScopeID);
        uint8_T year = 0;
        xilReadData((MemUnit_T *) &year, (uint32_T) sizeof(uint8_T));

        uint32_T dataWidth_month = 0;
        xilReadData((MemUnit_T *) &dataWidth_month, sizeData);
        uint8_T scopeID_month = 0;
        xilReadData((MemUnit_T *) &scopeID_month, sizeScopeID);
        uint8_T month = 0;
        xilReadData((MemUnit_T *) &month, (uint32_T) sizeof(uint8_T));

        uint32_T dataWidth_date = 0;
        xilReadData((MemUnit_T *) &dataWidth_date, sizeData);
        uint8_T scopeID_date = 0;
        xilReadData((MemUnit_T *) &scopeID_date, sizeScopeID);
        uint8_T date = 0;
        xilReadData((MemUnit_T *) &date, (uint32_T) sizeof(uint8_T));

        uint32_T dataWidth_day_of_week = 0;
        xilReadData((MemUnit_T *) &dataWidth_day_of_week, sizeData);
        uint8_T scopeID_day_of_week = 0;
        xilReadData((MemUnit_T *) &scopeID_day_of_week, sizeScopeID);
        uint8_T day_of_week = 0;
        xilReadData((MemUnit_T *) &day_of_week, (uint32_T) sizeof(uint8_T));



        out = DS3231_SetDateScalars_RTC_DS3231_Demo_6(year, month, date, day_of_week);



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (xilWriteData((MemUnit_T *) &out, (uint32_T) sizeof(uint8_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }


        break;
    }

    case 12:
    {


        DS3231_Init_RTC_DS3231_Demo_6();



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }


        break;
    }

    case 13:
    {
        uint32_T dataWidth_out = 0;
        xilReadData((MemUnit_T *) &dataWidth_out, sizeData);
        uint8_T scopeID_out = 0;
        xilReadData((MemUnit_T *) &scopeID_out, sizeScopeID);
        uint8_T out = 0;

        uint32_T dataWidth_time = 0;
        xilReadData((MemUnit_T *) &dataWidth_time, sizeData);
        uint8_T scopeID_time = 0;
        xilReadData((MemUnit_T *) &scopeID_time, sizeScopeID);
        ds3231_time_t *time = (ds3231_time_t *) calloc((size_t) dataWidth_time, sizeof(ds3231_time_t));
        if (scopeID_time < 2) {
            xilReadData((MemUnit_T *) time, dataWidth_time * ((uint32_T) sizeof(ds3231_time_t)));
        }



        out = DS3231_GetTime_RTC_DS3231_Demo_6(time);



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (xilWriteData((MemUnit_T *) &out, (uint32_T) sizeof(uint8_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (scopeID_time > 0) {
            if (xilWriteData((MemUnit_T *) time, dataWidth_time * ((uint32_T) sizeof(ds3231_time_t))) != XIL_DATA_STREAM_SUCCESS) {
                return XIL_INTERFACE_COMMS_FAILURE;
            }
        }

        free(time);

        break;
    }

    case 14:
    {
        uint32_T dataWidth_out = 0;
        xilReadData((MemUnit_T *) &dataWidth_out, sizeData);
        uint8_T scopeID_out = 0;
        xilReadData((MemUnit_T *) &scopeID_out, sizeScopeID);
        uint8_T out = 0;

        uint32_T dataWidth_hours = 0;
        xilReadData((MemUnit_T *) &dataWidth_hours, sizeData);
        uint8_T scopeID_hours = 0;
        xilReadData((MemUnit_T *) &scopeID_hours, sizeScopeID);
        uint8_T *hours = (uint8_T *) calloc((size_t) dataWidth_hours, sizeof(uint8_T));
        if (scopeID_hours < 2) {
            xilReadData((MemUnit_T *) hours, dataWidth_hours * ((uint32_T) sizeof(uint8_T)));
        }

        uint32_T dataWidth_minutes = 0;
        xilReadData((MemUnit_T *) &dataWidth_minutes, sizeData);
        uint8_T scopeID_minutes = 0;
        xilReadData((MemUnit_T *) &scopeID_minutes, sizeScopeID);
        uint8_T *minutes = (uint8_T *) calloc((size_t) dataWidth_minutes, sizeof(uint8_T));
        if (scopeID_minutes < 2) {
            xilReadData((MemUnit_T *) minutes, dataWidth_minutes * ((uint32_T) sizeof(uint8_T)));
        }

        uint32_T dataWidth_seconds = 0;
        xilReadData((MemUnit_T *) &dataWidth_seconds, sizeData);
        uint8_T scopeID_seconds = 0;
        xilReadData((MemUnit_T *) &scopeID_seconds, sizeScopeID);
        uint8_T *seconds = (uint8_T *) calloc((size_t) dataWidth_seconds, sizeof(uint8_T));
        if (scopeID_seconds < 2) {
            xilReadData((MemUnit_T *) seconds, dataWidth_seconds * ((uint32_T) sizeof(uint8_T)));
        }



        out = DS3231_GetTimeScalars_RTC_DS3231_Demo_6(hours, minutes, seconds);



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (xilWriteData((MemUnit_T *) &out, (uint32_T) sizeof(uint8_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (scopeID_hours > 0) {
            if (xilWriteData((MemUnit_T *) hours, dataWidth_hours * ((uint32_T) sizeof(uint8_T))) != XIL_DATA_STREAM_SUCCESS) {
                return XIL_INTERFACE_COMMS_FAILURE;
            }
        }

        if (scopeID_minutes > 0) {
            if (xilWriteData((MemUnit_T *) minutes, dataWidth_minutes * ((uint32_T) sizeof(uint8_T))) != XIL_DATA_STREAM_SUCCESS) {
                return XIL_INTERFACE_COMMS_FAILURE;
            }
        }

        if (scopeID_seconds > 0) {
            if (xilWriteData((MemUnit_T *) seconds, dataWidth_seconds * ((uint32_T) sizeof(uint8_T))) != XIL_DATA_STREAM_SUCCESS) {
                return XIL_INTERFACE_COMMS_FAILURE;
            }
        }

        free(hours);
        free(minutes);
        free(seconds);

        break;
    }

    case 15:
    {
        uint32_T dataWidth_out = 0;
        xilReadData((MemUnit_T *) &dataWidth_out, sizeData);
        uint8_T scopeID_out = 0;
        xilReadData((MemUnit_T *) &scopeID_out, sizeScopeID);
        uint8_T out = 0;

        uint32_T dataWidth_time_vec = 0;
        xilReadData((MemUnit_T *) &dataWidth_time_vec, sizeData);
        uint8_T scopeID_time_vec = 0;
        xilReadData((MemUnit_T *) &scopeID_time_vec, sizeScopeID);
        uint8_T *time_vec = (uint8_T *) calloc((size_t) dataWidth_time_vec, sizeof(uint8_T));
        if (scopeID_time_vec < 2) {
            xilReadData((MemUnit_T *) time_vec, dataWidth_time_vec * ((uint32_T) sizeof(uint8_T)));
        }



        out = DS3231_GetTimeArray_RTC_DS3231_Demo_6(time_vec);



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (xilWriteData((MemUnit_T *) &out, (uint32_T) sizeof(uint8_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (scopeID_time_vec > 0) {
            if (xilWriteData((MemUnit_T *) time_vec, dataWidth_time_vec * ((uint32_T) sizeof(uint8_T))) != XIL_DATA_STREAM_SUCCESS) {
                return XIL_INTERFACE_COMMS_FAILURE;
            }
        }

        free(time_vec);

        break;
    }

    case 16:
    {
        uint32_T dataWidth_out = 0;
        xilReadData((MemUnit_T *) &dataWidth_out, sizeData);
        uint8_T scopeID_out = 0;
        xilReadData((MemUnit_T *) &scopeID_out, sizeScopeID);
        uint8_T out = 0;

        uint32_T dataWidth_temp_c = 0;
        xilReadData((MemUnit_T *) &dataWidth_temp_c, sizeData);
        uint8_T scopeID_temp_c = 0;
        xilReadData((MemUnit_T *) &scopeID_temp_c, sizeScopeID);
        real32_T *temp_c = (real32_T *) calloc((size_t) dataWidth_temp_c, sizeof(real32_T));
        if (scopeID_temp_c < 2) {
            xilReadData((MemUnit_T *) temp_c, dataWidth_temp_c * ((uint32_T) sizeof(real32_T)));
        }



        out = DS3231_GetTemperature_RTC_DS3231_Demo_6(temp_c);



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (xilWriteData((MemUnit_T *) &out, (uint32_T) sizeof(uint8_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (scopeID_temp_c > 0) {
            if (xilWriteData((MemUnit_T *) temp_c, dataWidth_temp_c * ((uint32_T) sizeof(real32_T))) != XIL_DATA_STREAM_SUCCESS) {
                return XIL_INTERFACE_COMMS_FAILURE;
            }
        }

        free(temp_c);

        break;
    }

    case 17:
    {
        uint32_T dataWidth_out = 0;
        xilReadData((MemUnit_T *) &dataWidth_out, sizeData);
        uint8_T scopeID_out = 0;
        xilReadData((MemUnit_T *) &scopeID_out, sizeScopeID);
        uint8_T out = 0;

        uint32_T dataWidth_temp_quarter_deg = 0;
        xilReadData((MemUnit_T *) &dataWidth_temp_quarter_deg, sizeData);
        uint8_T scopeID_temp_quarter_deg = 0;
        xilReadData((MemUnit_T *) &scopeID_temp_quarter_deg, sizeScopeID);
        int16_T *temp_quarter_deg = (int16_T *) calloc((size_t) dataWidth_temp_quarter_deg, sizeof(int16_T));
        if (scopeID_temp_quarter_deg < 2) {
            xilReadData((MemUnit_T *) temp_quarter_deg, dataWidth_temp_quarter_deg * ((uint32_T) sizeof(int16_T)));
        }



        out = DS3231_GetTemperatureFixed_RTC_DS3231_Demo_6(temp_quarter_deg);



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (xilWriteData((MemUnit_T *) &out, (uint32_T) sizeof(uint8_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (scopeID_temp_quarter_deg > 0) {
            if (xilWriteData((MemUnit_T *) temp_quarter_deg, dataWidth_temp_quarter_deg * ((uint32_T) sizeof(int16_T))) != XIL_DATA_STREAM_SUCCESS) {
                return XIL_INTERFACE_COMMS_FAILURE;
            }
        }

        free(temp_quarter_deg);

        break;
    }

    case 18:
    {
        uint32_T dataWidth_out = 0;
        xilReadData((MemUnit_T *) &dataWidth_out, sizeData);
        uint8_T scopeID_out = 0;
        xilReadData((MemUnit_T *) &scopeID_out, sizeScopeID);
        uint32_T out = 0;



        out = DS3231_GetI2CInstance_RTC_DS3231_Demo_6();



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (xilWriteData((MemUnit_T *) &out, (uint32_T) sizeof(uint32_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }


        break;
    }

    case 19:
    {
        uint32_T dataWidth_out = 0;
        xilReadData((MemUnit_T *) &dataWidth_out, sizeData);
        uint8_T scopeID_out = 0;
        xilReadData((MemUnit_T *) &scopeID_out, sizeScopeID);
        uint8_T out = 0;

        uint32_T dataWidth_year = 0;
        xilReadData((MemUnit_T *) &dataWidth_year, sizeData);
        uint8_T scopeID_year = 0;
        xilReadData((MemUnit_T *) &scopeID_year, sizeScopeID);
        uint8_T *year = (uint8_T *) calloc((size_t) dataWidth_year, sizeof(uint8_T));
        if (scopeID_year < 2) {
            xilReadData((MemUnit_T *) year, dataWidth_year * ((uint32_T) sizeof(uint8_T)));
        }

        uint32_T dataWidth_month = 0;
        xilReadData((MemUnit_T *) &dataWidth_month, sizeData);
        uint8_T scopeID_month = 0;
        xilReadData((MemUnit_T *) &scopeID_month, sizeScopeID);
        uint8_T *month = (uint8_T *) calloc((size_t) dataWidth_month, sizeof(uint8_T));
        if (scopeID_month < 2) {
            xilReadData((MemUnit_T *) month, dataWidth_month * ((uint32_T) sizeof(uint8_T)));
        }

        uint32_T dataWidth_date = 0;
        xilReadData((MemUnit_T *) &dataWidth_date, sizeData);
        uint8_T scopeID_date = 0;
        xilReadData((MemUnit_T *) &scopeID_date, sizeScopeID);
        uint8_T *date = (uint8_T *) calloc((size_t) dataWidth_date, sizeof(uint8_T));
        if (scopeID_date < 2) {
            xilReadData((MemUnit_T *) date, dataWidth_date * ((uint32_T) sizeof(uint8_T)));
        }

        uint32_T dataWidth_day_of_week = 0;
        xilReadData((MemUnit_T *) &dataWidth_day_of_week, sizeData);
        uint8_T scopeID_day_of_week = 0;
        xilReadData((MemUnit_T *) &scopeID_day_of_week, sizeScopeID);
        uint8_T *day_of_week = (uint8_T *) calloc((size_t) dataWidth_day_of_week, sizeof(uint8_T));
        if (scopeID_day_of_week < 2) {
            xilReadData((MemUnit_T *) day_of_week, dataWidth_day_of_week * ((uint32_T) sizeof(uint8_T)));
        }



        out = DS3231_GetDateScalars_RTC_DS3231_Demo_6(year, month, date, day_of_week);



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (xilWriteData((MemUnit_T *) &out, (uint32_T) sizeof(uint8_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (scopeID_year > 0) {
            if (xilWriteData((MemUnit_T *) year, dataWidth_year * ((uint32_T) sizeof(uint8_T))) != XIL_DATA_STREAM_SUCCESS) {
                return XIL_INTERFACE_COMMS_FAILURE;
            }
        }

        if (scopeID_month > 0) {
            if (xilWriteData((MemUnit_T *) month, dataWidth_month * ((uint32_T) sizeof(uint8_T))) != XIL_DATA_STREAM_SUCCESS) {
                return XIL_INTERFACE_COMMS_FAILURE;
            }
        }

        if (scopeID_date > 0) {
            if (xilWriteData((MemUnit_T *) date, dataWidth_date * ((uint32_T) sizeof(uint8_T))) != XIL_DATA_STREAM_SUCCESS) {
                return XIL_INTERFACE_COMMS_FAILURE;
            }
        }

        if (scopeID_day_of_week > 0) {
            if (xilWriteData((MemUnit_T *) day_of_week, dataWidth_day_of_week * ((uint32_T) sizeof(uint8_T))) != XIL_DATA_STREAM_SUCCESS) {
                return XIL_INTERFACE_COMMS_FAILURE;
            }
        }

        free(year);
        free(month);
        free(date);
        free(day_of_week);

        break;
    }

    case 20:
    {
        uint32_T dataWidth_out = 0;
        xilReadData((MemUnit_T *) &dataWidth_out, sizeData);
        uint8_T scopeID_out = 0;
        xilReadData((MemUnit_T *) &scopeID_out, sizeScopeID);
        uint8_T out = 0;

        uint32_T dataWidth_osf_flag = 0;
        xilReadData((MemUnit_T *) &dataWidth_osf_flag, sizeData);
        uint8_T scopeID_osf_flag = 0;
        xilReadData((MemUnit_T *) &scopeID_osf_flag, sizeScopeID);
        boolean_T *osf_flag = (boolean_T *) calloc((size_t) dataWidth_osf_flag, sizeof(boolean_T));
        if (scopeID_osf_flag < 2) {
            xilReadData((MemUnit_T *) osf_flag, dataWidth_osf_flag * ((uint32_T) sizeof(boolean_T)));
        }

        uint32_T dataWidth_clear_if_set = 0;
        xilReadData((MemUnit_T *) &dataWidth_clear_if_set, sizeData);
        uint8_T scopeID_clear_if_set = 0;
        xilReadData((MemUnit_T *) &scopeID_clear_if_set, sizeScopeID);
        boolean_T clear_if_set = 0;
        xilReadData((MemUnit_T *) &clear_if_set, (uint32_T) sizeof(boolean_T));



        out = DS3231_CheckOscillatorStopFlag_RTC_DS3231_Demo_6(osf_flag, clear_if_set);



        MemUnit_T responseId = XIL_RESPONSE_OUTPUT_DATA;
        if (xilWriteData(&responseId, (uint32_T) sizeof(MemUnit_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (xilWriteData((MemUnit_T *) &out, (uint32_T) sizeof(uint8_T)) != XIL_DATA_STREAM_SUCCESS) {
            return XIL_INTERFACE_COMMS_FAILURE;
        }

        if (scopeID_osf_flag > 0) {
            if (xilWriteData((MemUnit_T *) osf_flag, dataWidth_osf_flag * ((uint32_T) sizeof(boolean_T))) != XIL_DATA_STREAM_SUCCESS) {
                return XIL_INTERFACE_COMMS_FAILURE;
            }
        }

        free(osf_flag);

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

