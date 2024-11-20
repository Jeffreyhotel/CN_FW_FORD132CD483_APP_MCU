/**
 * @file M_TemperatureDerating.c
 * 
 * @brief The program could execute a specific derating method according to backlight temperature.
 * 
 * @details 1.[0.1]To build a derating table which saves each backlight percentage according to backlight temperature.\n
 * 2.[0.1]To turn off a smooth dimming control at this module.\n
 * 3.[0.1]To build a state machine mechanism according to backlight temperature.\n
 * 4.[0.2]To add controling int_terr callback and saving derating calibration data callback funcs.\n
 * 5.[0.2]To add getting ambient temperature callback func.\n
 * 6.[0.2]To add accessing flash for calibration data.\n
 * 
 * 
 * @note After building a derating method and dimming control,
 * you must register these by DeratingApp_RegisterElement() and DeratingApp_RegisterDimmingControl(). 
 * 
 * @author Orlando Huang (Orlando.Huang@auo.com)
 * 
 * @version 0.2
 * 
 * @date 2022-08-12
 * 
 */

/* -- Includes -- */
#include "M_TemperatureDerating.h"
#include "Memory_Pool.h"

/* -- Type Define -- */
/**
 * @brief The data type is used for calculating a PWM duty between two elements in a derating table.
 * 
 */
typedef struct
{
#if(DERATINGAPP_FLOAT_OPERATION)
    float DeratingTemperature; /**< To save near temperature according to system's backlight temperature. */
    float DeratingPWM; /**< To save the PWM value according to DeratingTemperature value. */
#else
    int16_t DeratingTemperature; /**< To save near temperature according to system's backlight temperature. */
    int16_t DeratingPWM; /**< To save the PWM value according to DeratingTemperature value. */
#endif
}MTemperatureDeratingTempToPWM_Typedef;

/* -- Global Variables -- */
/**
 * @brief Callback for controlling INT_TERR.
 * 
 * @details 1.If function pointer is null, doesn't control int terr.\n
 * 2.If DeratingPWM is less than 100%, pull up int terr.\n
 * 
 */
CALLBACK_CTRL_TERR gCallbackCtrlTerrFunc = NULL;
/**
 * @brief Callback for fetching ambient temperature.
 * 
 * @details If function pointer is null, jump to normal state.
 * 
 */
CALLBACK_GET_TEMPERATURE gCallbackGetTemperature = NULL;
/**
 * @brief Callback for accsessing calibration data of flash.
 * 
 * @details If function pointer is null, calibration datas are configed default values.
 * 
 */
CALLBACK_ACCESS_FLASH_CALIBRATIONDATA gCallbackAccessFlashCalibrationData = NULL;
/**
 * @brief Variable for controlling enable or disable temperature derating.
 * 
 * @details If variable is DERATING_DISABLE, temperature derating is disable.
 * 
 */
TEMP_DERATING_ENABLE_DISABLE_STATUS gTempDeratingEnableDisableStatus = DERATING_ENABLE;
/**
 * @brief Saved calibration datas from flash.
 * 
 * @details When it is init, datas are loaded default values.
 * 
 */
MTemperatureDeratingAccessFlashCalibrationData AccessFlashCalibrationData;
/**
 * @brief Saved calibration data's address from flash.
 * 
 * @details When it is init, datas are loaded default values.
 * 
 */
MTemperatureDeratingAccessFlashCalibrationData *ptrAccessFlashCalibrationData = &AccessFlashCalibrationData;
#if(TEMP_DERATINGAPP_TEST)
/**
 * @brief A simulate backlight temperature is used for testing function.
 * 
 * @details The simulate backlight temperature could affect state machine mechanism's result.
 * 
 */
#if(DERATINGAPP_FLOAT_OPERATION)
/**
 * @brief To replace BL temperatrue for testing.
 * 
 * @details None.
 * 
 */
float gSimuBLTemperature = (TEMP_DERATING_DEFAULT_DERA_TEMP - 1.0);
#else
/**
 * @brief To replace BL temperatrue for testing.
 * 
 * @details None.
 * 
 */
int16_t gSimuBLTemperature = (TEMP_DERATING_DEFAULT_DERA_TEMP - (1*TEMP_DERATING_TEMP_RESOLUTION));
#endif
#endif
/**
 * @brief To Save previous state.
 * 
 * @details To reserve for extended function in the future.  
 * 
 */
DeratingApp_Status PreDeratingStatus = INIT_MODE;
#if(DERATINGAPP_FLOAT_OPERATION)
/**
 * @brief The state is from normal to derating if the backlight temperature is over it.
 * 
 * @details The default value is equal to 70 celisius.
 * 
 */
static float gBLDeratingLimitedTemp = TEMP_DERATING_DEFAULT_DERA_TEMP;
/**
 * @brief The backlight control module state is from derating to shut down if the backlight temperature is over it.
 * 
 * @details The default value is equal to 96 celisius.
 * 
 */
static float gBLShutDownLimitedTemp = TEMP_DERATING_DEFAULT_SHUTDOWN_TEMP;
/**
 * @brief The backlight control module state is from shut down to derating if the backlight temperature is over it.
 * 
 * @details The default value is equal to 96 celisius.
 * 
 */
static float gBLShutDownRelLimitedTemp = TEMP_DERATING_DEFAULT_SHUTDOWN_REL_TEMP;
#else
/**
 * @brief The state is from normal to derating if the backlight temperature is over it.
 * 
 * @details The value is equal to 70 celisius,it could start getting the PWM by looking for derating table.
 * 
 */
static int16_t gBLDeratingLimitedTemp = TEMP_DERATING_DEFAULT_DERA_TEMP;
/**
 * @brief The backlight control module state is from derating to shut down if the backlight temperature is over it.
 * 
 * @details The default value is equal to 96 celisius.
 * 
 */
static int16_t gBLShutDownLimitedTemp = TEMP_DERATING_DEFAULT_SHUTDOWN_TEMP;
/**
 * @brief The backlight control module state is from shut down to derating if the backlight temperature is over it.
 * 
 * @details The default value is equal to 96 celisius.
 * 
 */
static int16_t gBLShutDownRelLimitedTemp = TEMP_DERATING_DEFAULT_SHUTDOWN_REL_TEMP;
#endif

#if(DERATINGAPP_FLOAT_OPERATION)
/**
 * @brief A look up default table for catching the PWM duty according to backlight temperature. 
 * 
 * @details When table is not loaded from flash, the PWM duty is recoverd by the look up table.
 * 
 */
static const float gDeratingDefaultTable[TEMP_DERATING_TABLE_SIZE] = 
{
    100.0f, /**< 70 Degree. */
    100.0f, /**< 71 Degree. */
    100.0f, /**< 72 Degree. */
    100.0f, /**< 73 Degree. */
    100.0f, /**< 74 Degree. */
    100.0f, /**< 75 Degree. */
    100.0f, /**< 76 Degree. */
    100.0f, /**< 77 Degree. */
    100.0f, /**< 78 Degree. */
    100.0f, /**< 79 Degree. */
    100.0f, /**< 80 Degree. */
    100.0f, /**< 81 Degree. */
    100.0f, /**< 82 Degree. */
    100.0f, /**< 83 Degree. */
    100.0f, /**< 84 Degree. */
    100.0f, /**< 85 Degree. */
    100.0f, /**< 86 Degree. */
    100.0f, /**< 87 Degree. */
    88.0f, /**< 88 Degree. */
    75.0f, /**< 89 Degree. */
    63.0f, /**< 90 Degree. */
    50.0f, /**< 91 Degree. */
    38.0f, /**< 92 Degree. */
    25.0f, /**< 93 Degree. */
    13.0f, /**< 94 Degree. */
    10.0f, /**< 95 Degree. */
    0.0f, /**< 96 Degree. */
    0.0f, /**< 97 Degree. */
    0.0f, /**< 98 Degree. */
    0.0f, /**< 99 Degree. */
    0.0f, /**< 100 Degree. */
};
/**
 * @brief A look up table for catching the PWM duty according to backlight temperature. 
 * 
 * @details When the backlight temperature is over gBLDeratingLimitedTemp,the PWM duty is changed by the look up table.
 * 
 */
static float gDeratingTable[TEMP_DERATING_TABLE_SIZE];
#else
/**
 * @brief A look up default table for catching the PWM duty according to backlight temperature. 
 * 
 * @details When table is not loaded from flash, the PWM duty is recoverd by the look up table.
 * 
 */
#if(CX430_TDDI_NT51926 || U717_TDDI_NT51926)
static const int16_t gDeratingDefaultTable[TEMP_DERATING_TABLE_SIZE] =
{
    // 100, /**< 70 Degree. */
    // 100, /**< 71 Degree. */
    // 100, /**< 72 Degree. */
    // 100, /**< 73 Degree. */
    // 100, /**< 74 Degree. */
    // 100, /**< 75 Degree. */
    // 100, /**< 76 Degree. */
    // 100, /**< 77 Degree. */
    // 100, /**< 78 Degree. */
    // 100, /**< 79 Degree. */
    // 100, /**< 80 Degree. */
    // 100, /**< 81 Degree. */
    // 100, /**< 82 Degree. */
    // 100, /**< 83 Degree. */
    // 100, /**< 84 Degree. */
    // 100, /**< 85 Degree. */
    // 100, /**< 86 Degree. */
    // 100, /**< 87 Degree. */
    // 88, /**< 88 Degree. */
    // 75, /**< 89 Degree. */
    // 63, /**< 90 Degree. */
    // 50, /**< 91 Degree. */
    // 38, /**< 92 Degree. */
    // 25, /**< 93 Degree. */
    // 13, /**< 94 Degree. */
    // 10, /**< 95 Degree. */
    //  0, /**< 96 Degree. */
    //  0, /**< 97 Degree. */
    //  0, /**< 98 Degree. */
    //  0, /**< 99 Degree. */
    //  0, /**< 100 Degree. */

    100, /**< 88 Degree. */
    81, /**< 89 Degree. */
    64, /**< 90 Degree. */
    49, /**< 91 Degree. */
    36, /**< 92 Degree. */
    26, /**< 93 Degree. */
    17, /**< 94 Degree. */
    10, /**< 95 Degree. */
     0, /**< 96 Degree. */
     0, /**< 97 Degree. */
     0, /**< 98 Degree. */
     0, /**< 99 Degree. */
     0, /**< 100 Degree. */
};
#elif(BX726_TDDI_NT51926)
/* Jacky@2022.09.30 modify for BX726 PCBA follow "20220804_CX430 and U625_V1.1.xlsx" */
static int16_t gDeratingDefaultTable[TEMP_DERATING_TABLE_SIZE] =
{
	// 100, /**< 70 Degree. */
	// 100, /**< 71 Degree. */
	// 100, /**< 72 Degree. */
	// 100, /**< 73 Degree. */
	// 100, /**< 74 Degree. */
	// 100, /**< 75 Degree. */
	// 100, /**< 76 Degree. */
	// 100, /**< 77 Degree. */
	// 100, /**< 78 Degree. */
	// 100, /**< 79 Degree. */
	// 100, /**< 80 Degree. */
	// 100, /**< 81 Degree. */
	// 100, /**< 82 Degree. */
	// 100, /**< 83 Degree. */
	// 100, /**< 84 Degree. */
	// 91, /**< 85 Degree. */
	// 82, /**< 86 Degree. */
	// 73, /**< 87 Degree. */
	// 64, /**< 88 Degree. */
	// 55, /**< 89 Degree. */
	// 46, /**< 90 Degree. */
	// 37, /**< 91 Degree. */
	// 28, /**< 92 Degree. */
	// 19, /**< 93 Degree. */
	// 10, /**< 94 Degree. */
	// 0, /**< 95 Degree. */
	// 0, /**< 96 Degree. */
	// 0, /**< 97 Degree. */
	// 0, /**< 98 Degree. */
	// 0, /**< 99 Degree. */
	// 0, /**< 100 Degree. */

    100, /**< 84 Degree. */
	87, /**< 85 Degree. */
	74, /**< 86 Degree. */
	61, /**< 87 Degree. */
	49, /**< 88 Degree. */
	36, /**< 89 Degree. */
	23, /**< 90 Degree. */
	10, /**< 91 Degree. */
	0, /**< 92 Degree. */
    0, /**< 93 Degree. */
	0, /**< 94 Degree. */
	0, /**< 95 Degree. */
	0, /**< 96 Degree. */
	0, /**< 97 Degree. */
	0, /**< 98 Degree. */
	0, /**< 99 Degree. */
	0, /**< 100 Degree. */

};
#else
#endif
/**
 * @brief A look up table for catching the PWM duty according to backlight temperature. 
 * 
 * @details When the backlight temperature is over gBLDeratingLimitedTemp,the PWM duty is changed by the look up table.
 * 
 */
static int16_t gDeratingTable[TEMP_DERATING_TABLE_SIZE];
#endif
/**
 * @brief Records if the calibration datas of flash are updated.
 * 
 * @details When reveiving calibration data set I2C commnad, it is changed to true.
 * 
 */
static bool bDeratingCalibrationDataUpdatedFlash = false;

/* -- Local Functions -- */
/**
 * @brief When the system start,the function will be called.
 * 
 * @details If system does not get backlight temperature,the state machine is from init to normal avoid affecting backlingt control.
 * 
 * @note It is global function and be called by reference.
 * 
 * @return DeratingApp_ReturnMethod_Typedef To return the state function of derating method and the PWM dyuty.
 * 
 * @dot
 * digraph BLDerating_State_Machine_Mechanism {
 *  "Init" -> "Boot" [label = "BLTemperature >= 70 celies."];
 *  "Init" -> "Normal" [label = "BLTemperature is not vaild \n until gu16InitWaitTimer counts down to zero."];
 *  "Init" -> "Normal" [label = "BLTemperature is vaild \n and BLTemperature <70 celies."];
 * }
 * @enddot 
 * 
 */
static DeratingApp_ReturnMethod_Typedef TemperatureDerating_State_Init(void)
{
    DeratingApp_Status DeratingStatus = PreDeratingStatus;
    DeratingApp_ReturnMethod_Typedef ReturnMethod;

    DeratingStatus = NORMAL;

    gTempDeratingEnableDisableStatus = DERATING_ENABLE;

    PreDeratingStatus = DeratingStatus; 
    ReturnMethod.MethodStatus = DeratingStatus;
    ReturnMethod.DeratingOutData = TEMP_DERATING_DEFAULT_OUTDATA;

    return ReturnMethod;
}
/**
 * @brief Ready to execute derating function.
 * 
 * @details When system catchs backlight temperatrue which is over derating threshold.
 * 
 * @note It is global function and be called by reference.
 * 
 * @return DeratingApp_ReturnMethod_Typedef To return the state function of derating method and the PWM dyuty.
 * 
 * @dot
 * digraph BLDerating_State_Machine_Mechanism {
 *  "Boot" -> "Derating" [label = "No condition."];
 * }
 * @enddot 
 * 
 */
static DeratingApp_ReturnMethod_Typedef TemperatureDerating_State_Boot(void)
{
    DeratingApp_ReturnMethod_Typedef ReturnMethod;
    DeratingApp_Status DeratingStatus = DERATING;

    PreDeratingStatus = DeratingStatus;
    ReturnMethod.MethodStatus = DeratingStatus;
    ReturnMethod.DeratingOutData = TEMP_DERATING_DEFAULT_OUTDATA;

    return ReturnMethod;
}
/**
 * @brief To execute derating function and decide whether the next state is called.
 * 
 * @details 1.When backlight temperatrue is less derating threshold,the next state is normal.\n
 * 2.Because the state is derating,the PWM duty should be calculated according to gDeratingTable.
 * 
 * @note It is global function and be called by reference.
 * 
 * @return DeratingApp_ReturnMethod_Typedef To return the state function of derating method and the PWM dyuty.
 * 
 * @dot
 * digraph BLDerating_State_Machine_Mechanism {
 *  "Derating" -> "Derating" [label = "BLTemperature >= 70 celies."];
 *  "Derating" -> "Normal" [label = "BLTemperature < 70 celies."];
 *  "Derating" -> "Normal" [label = "Disable temperature derating."];
 * }
 * @enddot 
 * 
 */
static DeratingApp_ReturnMethod_Typedef TemperatureDerating_State_Derating(void)
{
    DeratingApp_ReturnMethod_Typedef ReturnMethod;
    DeratingApp_Status DeratingStatus = PreDeratingStatus;
    MTemperatureDeratingTempToPWM_Typedef DeratingTempToPWMUp;
    MTemperatureDeratingTempToPWM_Typedef DeratingTempToPWMDown;
    int16_t i16DeratingTableIndex = 0U;

#if(DERATINGAPP_FLOAT_OPERATION)
    float BLTemperature = (float)(gCallbackGetTemperature());
#else
    int16_t BLTemperature = gCallbackGetTemperature()*TEMP_DERATING_TEMP_RESOLUTION;
#endif

#if(TEMP_DERATINGAPP_TEST)
    BLTemperature = gSimuBLTemperature;
#endif

    if((gCallbackGetTemperature == NULL)\
        || (DERATING_DISABLE == gTempDeratingEnableDisableStatus))
    {
        DeratingStatus = NORMAL;
        gCallbackCtrlTerrFunc(TEMP_DERATING_TERR_CLEAR);
        ReturnMethod.DeratingOutData = TEMP_DERATING_DEFAULT_OUTDATA;
    }
    else
    {
        if(BLTemperature >= gBLDeratingLimitedTemp)
        {
            DeratingStatus = DERATING;
        }
        else
        {
            DeratingStatus = NORMAL;
        }
        
        /* PWM is calculate from derating table.The relationship of two items is linear. */
#if(DERATINGAPP_FLOAT_OPERATION)
        i16DeratingTableIndex = (BLTemperature - gBLDeratingLimitedTemp);
#else
        i16DeratingTableIndex = (BLTemperature - gBLDeratingLimitedTemp) / TEMP_DERATING_TEMP_RESOLUTION;
#endif
        /* Avoid to go out of array range. */
        if(i16DeratingTableIndex >= (TEMP_DERATING_TABLE_SIZE - 1))
        {
        	i16DeratingTableIndex = (TEMP_DERATING_TABLE_SIZE - 1);
        }
		else if(i16DeratingTableIndex <= 0)
		{
			i16DeratingTableIndex = 0;
		}
		else
		{/* Nothing */ }

        DeratingTempToPWMDown.DeratingPWM = gDeratingTable[i16DeratingTableIndex];
#if(DERATINGAPP_FLOAT_OPERATION)
        DeratingTempToPWMDown.DeratingTemperature = gBLDeratingLimitedTemp + (float)(i16DeratingTableIndex);
#else
        DeratingTempToPWMDown.DeratingTemperature = gBLDeratingLimitedTemp + (i16DeratingTableIndex * TEMP_DERATING_TEMP_RESOLUTION);
#endif
        DeratingTempToPWMUp.DeratingPWM = gDeratingTable[i16DeratingTableIndex + 1U];
#if(DERATINGAPP_FLOAT_OPERATION)
        DeratingTempToPWMUp.DeratingTemperature = gBLDeratingLimitedTemp + (float)(i16DeratingTableIndex + 1U);
#else
        DeratingTempToPWMUp.DeratingTemperature = gBLDeratingLimitedTemp + ((i16DeratingTableIndex + 1) * TEMP_DERATING_TEMP_RESOLUTION);
#endif
        ReturnMethod.DeratingOutData = DeratingTempToPWMDown.DeratingPWM + \
            (((BLTemperature - DeratingTempToPWMDown.DeratingTemperature)\
            *(DeratingTempToPWMUp.DeratingPWM - DeratingTempToPWMDown.DeratingPWM))\
            /(DeratingTempToPWMUp.DeratingTemperature - DeratingTempToPWMDown.DeratingTemperature));

        /* Control Derating Alert Flag. */
        if(ReturnMethod.DeratingOutData < TEMP_DERATING_DEFAULT_OUTDATA)
        {
            if(gCallbackCtrlTerrFunc == NULL)
            {
                /* Do nothing */
            }
            else
            {
                gCallbackCtrlTerrFunc(TEMP_DERATING_TERR_SET);
            }    
        }
        else
        {
            if(gCallbackCtrlTerrFunc == NULL)
            {
                /* Do nothing */
            }
            else
            {
                gCallbackCtrlTerrFunc(TEMP_DERATING_TERR_CLEAR);
            }
        }
    } 

    PreDeratingStatus = DeratingStatus;
    ReturnMethod.MethodStatus = DeratingStatus;
    return ReturnMethod;
}
/**
 * @brief To execute normal function and decide whether the next state is called.
 * 
 * @details 1.When backlight temperatrue is over derating threshold,the next state is derating.\n
 * 2.Because the state is normal,the PWM duty should be controled according to host's command.
 * 
 * @note It is global function and be called by reference.
 * 
 * @return DeratingApp_ReturnMethod_Typedef To return the state function of derating method and the PWM dyuty.
 * 
 * @dot
 * digraph BLDerating_State_Machine_Mechanism {
 *  "Normal" -> "Derating" [label = "BLTemperature >= 70 celies."];
 *  "Normal" -> "Normal" [label = "BLTemperature < 70 celies."];
 *  "Normal" -> "Normal" [label = "Disable temperature derating."];
 * }
 * @enddot 
 * 
 */
static DeratingApp_ReturnMethod_Typedef TemperatureDerating_State_Normal(void)
{
    DeratingApp_ReturnMethod_Typedef ReturnMethod;
    DeratingApp_Status DeratingStatus = PreDeratingStatus;

#if(DERATINGAPP_FLOAT_OPERATION)
    float BLTemperature = (float)(gCallbackGetTemperature());
#else
    int16_t BLTemperature = gCallbackGetTemperature()*TEMP_DERATING_TEMP_RESOLUTION;
#endif

#if(TEMP_DERATINGAPP_TEST)
    BLTemperature = gSimuBLTemperature;
#endif

    if((gCallbackGetTemperature == NULL)\
        || (DERATING_DISABLE == gTempDeratingEnableDisableStatus))
    {
        DeratingStatus = NORMAL;
    }
    else
    {
        if(BLTemperature >= gBLDeratingLimitedTemp)
        {
            DeratingStatus = DERATING;
        }
        else
        {
            DeratingStatus = NORMAL;
        }
    } 
    
    PreDeratingStatus = DeratingStatus;
    ReturnMethod.MethodStatus = DeratingStatus;
    ReturnMethod.DeratingOutData = TEMP_DERATING_DEFAULT_OUTDATA;

    return ReturnMethod;
}
/**
 * @brief Calibration datas are default values.
 * 
 * @details When reading calibration datas is not successfully, calibration datas are default values.
 * 
 * @note It's a local function.
 * 
 */
static void TemperatureDerating_CalibrationDataInit(void)
{  
    gBLDeratingLimitedTemp = TEMP_DERATING_DEFAULT_DERA_TEMP;
    gBLShutDownLimitedTemp = TEMP_DERATING_DEFAULT_SHUTDOWN_TEMP;
    gBLShutDownRelLimitedTemp = TEMP_DERATING_DEFAULT_SHUTDOWN_REL_TEMP;
    memcpy(gDeratingTable, gDeratingDefaultTable, sizeof(gDeratingTable));
}
/**
 * @brief Calibration dats are reloading from flash.
 * 
 * @details When system is over boot successfully, calibration datas are reading flash to load values.
 * 
 * @note It's a local function.
 * 
 */
static void TemperatureDerating_CalibrationDataReloadFlash(void)
{
    float fConvertedTemp = 0.0f;
    uint8_t *ptrDataAddress = NULL;
    uint8_t u8DeratingTableIndex = 0U;
    uint16_t u16TransfromData = 0U;

    ptrDataAddress = AccessFlashCalibrationData.uDeratingLimitedTemperatures.DeratingLimitedTemperatures.u8DeratingTemperature; 

    u16TransfromData = ((uint16_t)(*ptrDataAddress) << 8) | ((uint16_t)*(ptrDataAddress + 1U));

    fConvertedTemp = (float)(u16TransfromData);
    fConvertedTemp /= FLASH_DERATING_CALIBRATION_LIMITED_TEMP_RESOLUTION;
    fConvertedTemp += FLASH_DERATING_CALIBRATION_LIMITED_TEMPOFFSET;

#if(DERATINGAPP_FLOAT_OPERATION)
    gBLDeratingLimitedTemp = fConvertedTemp;
#else
    gBLDeratingLimitedTemp = (int16_t)(fConvertedTemp);
#endif

    ptrDataAddress = AccessFlashCalibrationData.uDeratingLimitedTemperatures.DeratingLimitedTemperatures.u8ShutDownTemperature; 

    u16TransfromData = ((uint16_t)(*ptrDataAddress) << 8) | ((uint16_t)*(ptrDataAddress + 1U));

    fConvertedTemp = (float)(u16TransfromData);
    fConvertedTemp /= FLASH_DERATING_CALIBRATION_LIMITED_TEMP_RESOLUTION;
    fConvertedTemp += FLASH_DERATING_CALIBRATION_LIMITED_TEMPOFFSET;
#if(DERATINGAPP_FLOAT_OPERATION)
    gBLShutDownLimitedTemp = fConvertedTemp;
#else
    gBLShutDownLimitedTemp = (int16_t)(fConvertedTemp);
#endif

    ptrDataAddress = AccessFlashCalibrationData.uDeratingLimitedTemperatures.DeratingLimitedTemperatures.u8ShutDownRelTemperature; 

    u16TransfromData = ((uint16_t)(*ptrDataAddress) << 8) | ((uint16_t)*(ptrDataAddress + 1U));

    fConvertedTemp = (float)(u16TransfromData);
    fConvertedTemp /= FLASH_DERATING_CALIBRATION_LIMITED_TEMP_RESOLUTION;
    fConvertedTemp += FLASH_DERATING_CALIBRATION_LIMITED_TEMPOFFSET;
#if(DERATINGAPP_FLOAT_OPERATION)
    gBLShutDownRelLimitedTemp = fConvertedTemp;
#else
    gBLShutDownRelLimitedTemp = (int16_t)(fConvertedTemp);
#endif

    ptrDataAddress = AccessFlashCalibrationData.u8DeratingTable;
    for(u8DeratingTableIndex = 0U ; u8DeratingTableIndex < TEMP_DERATING_TABLE_SIZE ; u8DeratingTableIndex++)
    {
#if(DERATINGAPP_FLOAT_OPERATION)
        gDeratingTable[u8DeratingTableIndex] = (float)(*(ptrDataAddress + u8DeratingTableIndex));
#else
        gDeratingTable[u8DeratingTableIndex] = (int16_t)(*(ptrDataAddress + u8DeratingTableIndex));
#endif
    }
}

/* -- Global Functions -- */
/**
 * @brief DeratingApp.c apply to execute temperature derating.
 * 
 * @details If CallbackGetTemperature is NULL, truns off temperature derating.\n
 * 
 * @note It is a global function.
 * 
 */
void TemperatureDerating_Init(CALLBACK_CTRL_TERR CallbackCtrlTerr,\
CALLBACK_GET_TEMPERATURE CallbackGetTemperature,\
CALLBACK_ACCESS_FLASH_CALIBRATIONDATA CallbackAccessFlashCalibrationData)
{
    /* Register callback func to control Terr. */
    gCallbackCtrlTerrFunc = CallbackCtrlTerr;
    /* Register callback func to get Temp. */
    gCallbackGetTemperature = CallbackGetTemperature;
    /* Register callback func to access flash calibration data. */
    gCallbackAccessFlashCalibrationData = CallbackAccessFlashCalibrationData;
    /* Loads calibration data */
    if(gCallbackAccessFlashCalibrationData == NULL)
    {
        /* Calibration datas are default value. */
        TemperatureDerating_CalibrationDataInit();
    }
    else
    {
        /* Gets calibration data are from flash. */
        if(!gCallbackAccessFlashCalibrationData(ptrAccessFlashCalibrationData, DERATING_FLASH_OPERATE_READ))
        {
            /* Get faild to set calibration datas are default value. */
            TemperatureDerating_CalibrationDataInit(); 
        }
        else
        {
            /* Loads calibration data */
            TemperatureDerating_CalibrationDataReloadFlash();
        }
    }

    /* Register BL Derating Dimming */
    DeratingApp_RegisterElement(AMBIENT_TEMPERATURE,\
        TemperatureDerating_State_Init,\
        TemperatureDerating_State_Boot,\
        TemperatureDerating_State_Derating,\
        TemperatureDerating_State_Normal,\
        TEMP_DERATING_METHOD_PERIOD_TIME,\
        TEMP_DERATING_METHOD_CHECK_TIME
    );
}

#if(DERATINGAPP_FLOAT_OPERATION)
/**
 * @brief To return limited temperature threshold.
 * 
 * @details By TEMP_DERATING_LIMITED_SELECTION to select a specific limited temperature.
 * 
 * @note It is a global function.
 * 
 * @param TEMP_DERATING_LIMITED_SELECTION Controls to return a specific limited temperature.
 * 
 * @return float A specific limited temperature.
 * 
 */
float TemperatureDerating_GetLimitedTemperature(TEMP_DERATING_LIMITED_SELECTION TempDeratingLimitedSelection)
#else
/**
 * @brief To return limited temperature threshold.
 * 
 * @details By TEMP_DERATING_LIMITED_SELECTION to select a specific limited temperature.
 * 
 * @note It is a global function.
 * 
 * @param TEMP_DERATING_LIMITED_SELECTION Controls to return a specific limited temperature.
 * 
 * @return int16_t A specific limited temperature.
 * 
 */
int16_t TemperatureDerating_GetLimitedTemperature(TEMP_DERATING_LIMITED_SELECTION TempDeratingLimitedSelection)
#endif
{
    switch (TempDeratingLimitedSelection)
    {    
        default:
            return 0;
            break;

        case LIMITED_DERATING_TEMP:
            return gBLDeratingLimitedTemp;
            break;

        case LIMITED_SHUTDOWN_TEMP:
            return gBLShutDownLimitedTemp;
            break;

        case LIMITED_SHUTDOWN_REL_TEMP:
            return gBLShutDownRelLimitedTemp;
            break;
    }
}

/**
 * @brief Getting DeratingLimitedTemp,ShutDownLimitedTemp and ShutDownRelLimitedTemp.
 * 
 * @details If u8CalibrationElementLength is more than TEMP_DERATING_LIMITED_TEMPERATURE_LENGTH, getting data is not successfully.
 * 
 * @note It is a global function.
 * 
 * @param volatile uint8_t* Returned data's address.
 * 
 * @param Returned data's length.
 * 
 * @return true Getting data is successfully.
 *  
 * @return false Getting data is not successfully.
 *  
 */
bool TemperatureDerating_LimitedTemperature_Get(volatile uint8_t* ptrCalibrationElement, uint8_t u8CalibrationElementLength)
{
    float fConvertedTemp = 0.0f;
    uint16_t u16ReturnedTemp = 0U;

    if(u8CalibrationElementLength != TEMP_DERATING_LIMITED_TEMPERATURE_LENGTH)
    {
        return false;
    }
    else{/* NA */}

    /* Sets Derating Setting Temperature */
    fConvertedTemp = (((float)(gBLDeratingLimitedTemp) - FLASH_DERATING_CALIBRATION_LIMITED_TEMPOFFSET)) * (float)FLASH_DERATING_CALIBRATION_LIMITED_TEMP_RESOLUTION;
    u16ReturnedTemp = (uint16_t)(fConvertedTemp);
    /* Fills Data */
    *ptrCalibrationElement = (uint8_t)(u16ReturnedTemp >> 8);
    *(ptrCalibrationElement + 1U) = (uint8_t)(u16ReturnedTemp);

    /* Sets Shutdown Setting Temperature */
    fConvertedTemp = (((float)(gBLShutDownLimitedTemp) - FLASH_DERATING_CALIBRATION_LIMITED_TEMPOFFSET)) * (float)FLASH_DERATING_CALIBRATION_LIMITED_TEMP_RESOLUTION;
    u16ReturnedTemp = (uint16_t)(fConvertedTemp);
    /* Fills Data */
    *(ptrCalibrationElement + 2U) = (uint8_t)(u16ReturnedTemp >> 8);
    *(ptrCalibrationElement + 3U) = (uint8_t)(u16ReturnedTemp);

    /* Sets Shutdown Rel Setting Temperature */
    fConvertedTemp = (((float)(gBLShutDownRelLimitedTemp) - FLASH_DERATING_CALIBRATION_LIMITED_TEMPOFFSET)) * (float)FLASH_DERATING_CALIBRATION_LIMITED_TEMP_RESOLUTION;
    u16ReturnedTemp = (uint16_t)(fConvertedTemp);
    /* Fills Data */
    *(ptrCalibrationElement + 4U) = (uint8_t)(u16ReturnedTemp >> 8);
    *(ptrCalibrationElement + 5U) = (uint8_t)(u16ReturnedTemp);

    return true;
}
/**
 * @brief Getting contents of temperature derating duty table.
 * 
 * @details If u8CalibrationElementLength is more than TEMP_DERATING_TABLE_SIZE, getting data is not successfully.
 * 
 * @note It is a global function.
 * 
 * @param volatile uint8_t* Returned data's address.
 * 
 * @param Returned data's length.
 * 
 * @return true Getting data is successfully.
 *  
 * @return false Getting data is not successfully.
 * 
 */
bool TemperatureDerating_DeratingTable_Get(volatile uint8_t* ptrCalibrationElement, uint8_t u8CalibrationElementLength)
{
    uint8_t u8DeratingTableIndex = 0;

    if(u8CalibrationElementLength != TEMP_DERATING_TABLE_SIZE)
    {
        return false;
    }
    else{/* NA */}

    for(u8DeratingTableIndex = 0 ; u8DeratingTableIndex < u8CalibrationElementLength; u8DeratingTableIndex++)
    {
        *(ptrCalibrationElement + u8DeratingTableIndex) = (uint8_t)(*(gDeratingTable + u8DeratingTableIndex));
    }

    return true;
}
/**
 * @brief Setting temperature derating calibration datas.
 * 
 * @details *ptrCalibrationElement indicates which data is changed.
 * 
 * @note It is a global function.
 * 
 * @param volatile uint8_t* Setting address of command.
 * 
 * @return true Setting data is successfully.
 * 
 * @return false Setting data is not successfully.
 *  
 */
bool TemperatureDerating_DeratingCalibrationData_Set(volatile uint8_t* ptrCalibrationElement)
{
    float fConvertedTemp = 0.0f;
    bool bResult = false;
    uint16_t u16TransfromData = 0U;

    switch((TEMP_DERATING_CALIBRATION_DATAINDEX)(*ptrCalibrationElement))
    {
        case DERATING_CALIBRATION_DATAINDEX_DERATING_TEMP:
            /* Gets Derating Setting Temperature */
            u16TransfromData = ((uint16_t)*(ptrCalibrationElement + 1) << 8) | ((uint16_t)*(ptrCalibrationElement + 2));
            fConvertedTemp = (float)u16TransfromData;
            fConvertedTemp /= FLASH_DERATING_CALIBRATION_LIMITED_TEMP_RESOLUTION;
            fConvertedTemp += FLASH_DERATING_CALIBRATION_LIMITED_TEMPOFFSET;
#if(DERATINGAPP_FLOAT_OPERATION)
            gBLDeratingLimitedTemp = fConvertedTemp;
#else
            gBLDeratingLimitedTemp = (int16_t)(fConvertedTemp);
#endif
            bResult = true;
            break;
        case DERATING_CALIBRATION_DATAINDEX_SHUTDOWN_TEMP:
            /* Gets Shut Down Setting Temperature */
            u16TransfromData = ((uint16_t)*(ptrCalibrationElement + 1) << 8) | ((uint16_t)*(ptrCalibrationElement + 2));
            fConvertedTemp = (float)u16TransfromData;
            fConvertedTemp /= FLASH_DERATING_CALIBRATION_LIMITED_TEMP_RESOLUTION;
            fConvertedTemp += FLASH_DERATING_CALIBRATION_LIMITED_TEMPOFFSET;
#if(DERATINGAPP_FLOAT_OPERATION)
            gBLShutDownLimitedTemp = fConvertedTemp;
#else
            gBLShutDownLimitedTemp = (int16_t)(fConvertedTemp);
#endif
            bResult = true;
            break;
        case DERATING_CALIBRATION_DATAINDEX_SHUTDOWN_REL_TEMP:
            /* Gets Shut Down Rel Setting Temperature */
            u16TransfromData = ((uint16_t)*(ptrCalibrationElement + 1) << 8) | ((uint16_t)*(ptrCalibrationElement + 2));
            fConvertedTemp = (float)u16TransfromData;
            fConvertedTemp /= FLASH_DERATING_CALIBRATION_LIMITED_TEMP_RESOLUTION;
            fConvertedTemp += FLASH_DERATING_CALIBRATION_LIMITED_TEMPOFFSET;
#if(DERATINGAPP_FLOAT_OPERATION)
            gBLShutDownRelLimitedTemp = fConvertedTemp;
#else
            gBLShutDownRelLimitedTemp = (int16_t)(fConvertedTemp);
#endif
            bResult = true;
            break;

        default:
            if(*ptrCalibrationElement >= (DERATING_CALIBRATION_DATAINDEX_TABLE_START + TEMP_DERATING_TABLE_SIZE))
            {
                bResult = false;
            }
            else
            {
                u16TransfromData = ((uint16_t)*(ptrCalibrationElement + 1) << 8) | ((uint16_t)*(ptrCalibrationElement + 2));
                fConvertedTemp = (float)u16TransfromData;
#if(DERATINGAPP_FLOAT_OPERATION)
                gDeratingTable[(*ptrCalibrationElement - DERATING_CALIBRATION_DATAINDEX_TABLE_START)] = fConvertedTemp;
#else
                gDeratingTable[(*ptrCalibrationElement - DERATING_CALIBRATION_DATAINDEX_TABLE_START)] = (int16_t)(fConvertedTemp);
#endif
            }
            bResult = true;
            break;
    }

    if(!bResult)
    {
        /* NA */
    }
    else
    {
        bDeratingCalibrationDataUpdatedFlash = (!bDeratingCalibrationDataUpdatedFlash)?\
        true:\
        bDeratingCalibrationDataUpdatedFlash;
    }

    return bResult;
}
/**
 * @brief To save calibration datas to flash of MCU.
 * 
 * @details To save calibration datas to flash of MCU if access flash api has supplied.
 * 
 * @note It is a global function.
 * 
 */
void TemperatureDerating_SaveCalibrationData(void)
{
    if((!bDeratingCalibrationDataUpdatedFlash)\
        || (gCallbackAccessFlashCalibrationData == NULL))
    {
        /* Doesn't Save */
    }
    else
    {
        /* Save */
        (void)TemperatureDerating_LimitedTemperature_Get(AccessFlashCalibrationData.uDeratingLimitedTemperatures.u8DeratingLimitedTemperatureByte,\
        TEMP_DERATING_LIMITED_TEMPERATURE_LENGTH);
        
        (void)TemperatureDerating_DeratingTable_Get(AccessFlashCalibrationData.u8DeratingTable,\
        TEMP_DERATING_TABLE_SIZE);

        (void)gCallbackAccessFlashCalibrationData(ptrAccessFlashCalibrationData, DERATING_FLASH_OPERATE_WRITE);
        /* Clear Flag */
        bDeratingCalibrationDataUpdatedFlash = false;
    }
}
/**
 * @brief To supply turn on or off for external.
 * 
 * @details None.
 * 
 * @note None.
 * 
 * @param TempDeratingEnableDisableStatus Variable for controlling turn on or off.
 *  
 */
void TemperatureDerating_TurnOnOff_Set(TEMP_DERATING_ENABLE_DISABLE_STATUS TempDeratingEnableDisableStatus)
{
    gTempDeratingEnableDisableStatus = TempDeratingEnableDisableStatus;
}
/**
 * @brief To retun status of turn on or off.
 * 
 * @details None.
 * 
 * @note None.
 * 
 * @return TEMP_DERATING_ENABLE_DISABLE_STATUS Variable for controlling turn on or off.
 * 
 */
extern TEMP_DERATING_ENABLE_DISABLE_STATUS TemperatureDerating_TurnOnOff_Get(void)
{
    return gTempDeratingEnableDisableStatus;
}
/* -- END -- */
