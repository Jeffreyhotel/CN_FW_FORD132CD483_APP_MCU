/* -- Avoid Re-Includes -- */
#ifndef M_TEMPERATUREDERATING_H
#define M_TEMPERATUREDERATING_H

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

/* -- Includes -- */
#include "../../Mymain.h"
#include "M_Derating.h"

/* -- Marco Define -- */
#define TEMP_DERATINGAPP_TEST false /* a compile option for simulative testing. */

#define TEMP_DERATING_METHOD_PERIOD_TIME 10U /* a minimal period time to check backlight derating state functions. */
#define TEMP_DERATING_METHOD_CHECK_TIME 20U /* a minimal period time to call backlight derating state functions. */

#if(DERATINGAPP_FLOAT_OPERATION)
#define TEMP_DERATING_DEFAULT_OUTDATA 100.0f /* The default value of backlight PWM duty. */
#define TEMP_DERATING_FULL_OUTDATA 100.0f /* The full value of backlight PWM duty. */
#define TEMP_DERATING_DEFAULT_DERA_TEMP 70.0f /* The default value of derating threshold */
#define TEMP_DERATING_DEFAULT_REDUCEBL_TEMP 87.0f /* The default value of redsuce threshold */
#define TEMP_DERATING_DEFAULT_SHUTDOWN_TEMP 96.0f /* The default value of shut down threshold */
#define TEMP_DERATING_DEFAULT_SHUTDOWN_REL_TEMP 94.0f /* The default value of shut down release threshold */
#else
#define TEMP_DERATING_TEMP_RESOLUTION 1 /* The minimal unit of derating threshold */
#define TEMP_DERATING_DEFAULT_OUTDATA 100  /* The default value of backlight PWM duty. */
#define TEMP_DERATING_FULL_OUTDATA 100  /* The full value of backlight PWM duty. */
#if(BX726_TDDI_NT51926)
#define TEMP_DERATING_DEFAULT_DERA_TEMP (84 * TEMP_DERATING_TEMP_RESOLUTION) /* The default value of derating threshold */
#define TEMP_DERATING_DEFAULT_REDUCEBL_TEMP (85 * TEMP_DERATING_TEMP_RESOLUTION) /* The default value of reduce threshold */
#define TEMP_DERATING_DEFAULT_SHUTDOWN_TEMP (92 * TEMP_DERATING_TEMP_RESOLUTION) /* The default value of shut down threshold */
#define TEMP_DERATING_DEFAULT_SHUTDOWN_REL_TEMP (90 * TEMP_DERATING_TEMP_RESOLUTION) /* The default value of shut down release threshold */
#else
#define TEMP_DERATING_DEFAULT_DERA_TEMP (88 * TEMP_DERATING_TEMP_RESOLUTION) /* The default value of derating threshold */
#define TEMP_DERATING_DEFAULT_REDUCEBL_TEMP (89 * TEMP_DERATING_TEMP_RESOLUTION) /* The default value of reduce threshold */
#define TEMP_DERATING_DEFAULT_SHUTDOWN_TEMP (96 * TEMP_DERATING_TEMP_RESOLUTION) /* The default value of shut down threshold */
#define TEMP_DERATING_DEFAULT_SHUTDOWN_REL_TEMP (94 * TEMP_DERATING_TEMP_RESOLUTION) /* The default value of shut down release threshold */
#endif
#endif

#define TEMP_DERATING_TABLE_SIZE 31 /* The data size of gDeratingTable. */
#define TEMP_DERATING_LIMITED_TEMPERATURE_LENGTH 6

#define TEMP_DERATING_INIT_WAIT_TIME_OUT 10000U /* The threshold is maximal acceptable time. */

#define FLASH_DERATING_CALIBRATION_LIMITED_TEMP_RESOLUTION 10.0 /* To transform calibration data when accessing flash or receiving I2C command */
#define FLASH_DERATING_CALIBRATION_LIMITED_TEMPOFFSET -40.0 /* To transform calibration data when accessing flash or receiving I2C command */

typedef enum
{
    LIMITED_DERATING_TEMP = 0, /**< 0:Index of derating temperature. */
    LIMITED_SHUTDOWN_TEMP, /**< 1:Index of shut down temperature. */
    LIMITED_SHUTDOWN_REL_TEMP, /**< 2:Index of shut down release temperature. */
}TEMP_DERATING_LIMITED_SELECTION;

typedef enum
{
    TEMP_DERATING_TERR_CLEAR = 0, /**< 0:To alert that the module is not at derating. */
    TEMP_DERATING_TERR_SET, /**< 1:To alert that the module is at derating. */
}TEMP_DERATING_CONTROL_TERR_E;

typedef enum
{
    DERATING_CALIBRATION_DATAINDEX_DERATING_TEMP = 1, /**< 1:Index of derating temperature when receiving I2C command. */
    DERATING_CALIBRATION_DATAINDEX_SHUTDOWN_TEMP, /**< 2:Index of shut down temperature when receiving I2C command. */
    DERATING_CALIBRATION_DATAINDEX_SHUTDOWN_REL_TEMP, /**< 3:Index of shut down rlelease temperature when receiving I2C command. */
    DERATING_CALIBRATION_DATAINDEX_TABLE_START,/**< 4:Index of starting derating deuty table when receiving I2C command. */
}TEMP_DERATING_CALIBRATION_DATAINDEX;

typedef enum
{
    DERATING_FLASH_OPERATE_WRITE = 0, /**< 0:To write calibraiotn data to flash. */
    DERATING_FLASH_OPERATE_READ, /**< 1:To read calibraiotn data from flash. */
}TEMP_DERATING_FLASH_OPERATE;

typedef enum
{
    DERATING_ENABLE = 0xAAU, /**< 0xAA:To enable temperatrue derating. */
    DERATING_DISABLE = 0xABU, /**< 0xAB:To disable temperatrue derating. */
}TEMP_DERATING_ENABLE_DISABLE_STATUS;

/* -- Data Type Define -- */
typedef struct 
{
    union
    {
        struct
        {
            uint8_t u8DeratingTemperature[2U]; /**< Save derating temperature using byte. */
            uint8_t u8ShutDownTemperature[2U]; /**< Save shut down temperature using byte. */
            uint8_t u8ShutDownRelTemperature[2U]; /**< Save shut down release temperature using byte. */
        }DeratingLimitedTemperatures;
        uint8_t u8DeratingLimitedTemperatureByte[6]; /**< Save limited temperatures using byte. */
    }uDeratingLimitedTemperatures;
    
    uint8_t u8DeratingTable[TEMP_DERATING_TABLE_SIZE]; /**< Save temperature detating table using byte. */
}MTemperatureDeratingAccessFlashCalibrationData;

typedef void (*CALLBACK_CTRL_TERR)(TEMP_DERATING_CONTROL_TERR_E eTempDeratingControlTERR); /**< Call back to control if derating is running. */

#if(DERATINGAPP_FLOAT_OPERATION)
typedef float (*CALLBACK_GET_TEMPERATURE)(void); /**< Call back to catch BL temperature. */
#else
typedef int16_t (*CALLBACK_GET_TEMPERATURE)(void); /**< Call back to catch BL temperature. */
#endif

typedef bool (*CALLBACK_ACCESS_FLASH_CALIBRATIONDATA)(MTemperatureDeratingAccessFlashCalibrationData* ptrFlashCalibrationData,\
TEMP_DERATING_FLASH_OPERATE eDeratingFlashOperate); /**< Call back to access flash for calibration datas. */

/* -- Extern Global Variables -- */
#if(TEMP_DERATINGAPP_TEST)
#if(DERATINGAPP_FLOAT_OPERATION)
extern float gSimuBLTemperature; /* The simulative backlight temperature for testing. */
#else
extern int16_t gSimuBLTemperature; /* The simulative backlight temperature for testing. */
#endif
#endif

/* -- Extern Functions -- */
extern void TemperatureDerating_Init(CALLBACK_CTRL_TERR CallbackCtrlTerr,\
CALLBACK_GET_TEMPERATURE CallbackGetTemperature,\
CALLBACK_ACCESS_FLASH_CALIBRATIONDATA CallbackAccessFlashCalibrationData);

#if(DERATINGAPP_FLOAT_OPERATION)
extern float TemperatureDerating_GetLimitedTemperature(TEMP_DERATING_LIMITED_SELECTION TempDeratingLimitedSelection);
#else
extern int16_t TemperatureDerating_GetLimitedTemperature(TEMP_DERATING_LIMITED_SELECTION TempDeratingLimitedSelection);
#endif

#if(BACKDOOR_WRITE_DERATINGDATA)
extern bool TemperatureDerating_LimitedTemperature_Get(volatile uint8_t* ptrCalibrationElement, uint8_t u8CalibrationElementLength);
extern bool TemperatureDerating_DeratingTable_Get(volatile uint8_t* ptrCalibrationElement, uint8_t u8CalibrationElementLength);
extern bool TemperatureDerating_DeratingCalibrationData_Set(volatile uint8_t* ptrCalibrationElement);
#endif

extern void TemperatureDerating_SaveCalibrationData(void);
extern void TemperatureDerating_TurnOnOff_Set(TEMP_DERATING_ENABLE_DISABLE_STATUS TempDeratingEnableDisableStatus);
extern TEMP_DERATING_ENABLE_DISABLE_STATUS TemperatureDerating_TurnOnOff_Get(void);
#endif
/* -- END -- */
