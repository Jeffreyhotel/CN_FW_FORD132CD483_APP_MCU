/**
 * @file M_BacklightControl.c
 * @author Orlando.Huang (orlando.huang@auo.com)
 * @brief 
 * @version 0.1
 * @date 2022-05-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */
/* -- Includes -- */
#include "M_BacklightControl.h"
#include "M_PWMDimming.h"
#include "Memory_Pool.h"
#include "hal_pwm.h"
#include "M_Derating.h"
#include "M_FixedFlashAccess.h"

/* -- Marco Define -- */
#define LOWBYTE(x) (uint8_t)(x & 0x00FF) /* Gets low byte of uint16_t data. */
#define HIGHYTE(x) (uint8_t)((x & 0x0300) >> 0x08) /* Gets low byte of uint16_t data. */

#define EMPTYPWM 0U
#define FULLPWM 1023U

/**
 * @brief Defines dimming status.
 * 
 */
typedef enum
{
    E_MBL_DIMMING_DIS = 0, /**< Disables dimming = 0. */
    E_MBL_DIMMING_EN = 1, /**< Enables dimming = 1. */
}MBacklightControlDimmingControl_E;

/* -- Data Type Define -- */
typedef struct
{
    bool bReceivedAlert;
    CALLBACK_EX_ALERT_HANDLER_COMPLETE CallbackExAlertHandlerComplete;
}MBacklightControl_AlertEvent;
/**
 * @brief Backlight control state machine.
 * 
 */
typedef struct
{
    MBacklightControlStateMachine_E BacklightStateMachine; /**< For the state of backlight control state machine. */
    int16_t AmbientTemperature; /**< Saves temperature from momory pool. */
    uint16_t u16CurrentPwmValue; /**< Saves current pwm. */
    MBacklightControlExternalEnable_E eMBLCtrlExternalEn;
    MBacklightControlDimmingControl_E eMBLCtrlDimmingEn;
    MBacklightControl_AlertEvent CallbackExAlertEnComplete; /**< When backlight turn on due to external alert. */
    MBacklightControl_AlertEvent CallbackExAlertDisComplete; /**< When backlight turn off due to external alert. */
}MBacklightControlStateMachineControl_Typedef;

/* -- Global Variables -- */
#if (HIJACK_ADC)
/* Jacky@20220930, for hijack pcba adc input*/
static uint8_t gbHijackPCBA = 0U;
static int16_t gi16FakePCBA = 0U;
#endif

/**
 * @brief Implements MBacklightControlStateMachineControl_Typedef data.
 * 
 * @details Initializes MBacklightControlStateMachineControl_Typedef data.
 * 
 */
static MBacklightControlStateMachineControl_Typedef StateMachineControl = \
{
    E_MBL_STATEMACHINE_INIT,
    0,
    0U,
    E_MBL_EXTERNAL_DISABLE_DIMMING,
    E_MBL_DIMMING_EN,
    {.bReceivedAlert = false,.CallbackExAlertHandlerComplete = NULL},
    {.bReceivedAlert = false,.CallbackExAlertHandlerComplete = NULL},
};
/* -- Local Functions -- */
/**
 * @brief Erases or sets TERR in the display status.
 * 
 * @param eMBLUpdateTERR Operation of erase or set TERR.
 */
static void MBacklightControl_UpdateTERR(TEMP_DERATING_CONTROL_TERR_E eTempDeratingControlTERR)
{
    switch(eTempDeratingControlTERR)
    {
        default:
			/* Do nothing. */
            break;

        case TEMP_DERATING_TERR_CLEAR:
            Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() & (~BIT_TERR_POS));
            break;

        case TEMP_DERATING_TERR_SET:
            Memory_Pool_DisplayStatus_Set(Memory_Pool_DisplayStatus_Get() | (BIT_TERR_POS));
            Memory_Pool_ActualDisplayStatus_Set(Memory_Pool_ActualDisplayStatus_Get() | (BIT_TERR_POS));
            break;
    }
}
/**
 * @brief Supports it for dimming module to register callback pwm.
 * 
 * @details Calls HAL_PWM_Duty_Output_Adjust to output physics pwm.
 * 
 * @note Transfers 2 bytes date length pwm to 4 bytes date length pwm.
 * 
 * @param u16DimmingPWMSignal The pwm after dimming.
 * 
 */
static void MBacklightControl_SendPWMSignal(uint16_t u16DimmingPWMSignal)
{
    HAL_PWM_Duty_Output_Adjust(PWM_OUT_DIM_NUM,(uint32_t)(u16DimmingPWMSignal));
}

static uint8_t MBacklightControl_CalculateFlashCalibrationData(uint8_t *Data, uint8_t DataLen)
{
    uint8_t u8DataIndex = 0U;
    uint8_t u8Crc = E_MBL_FLASHDERATINGDATA_CHECKBYTE;

    for(u8DataIndex = 0U ; u8DataIndex < DataLen ; u8DataIndex++)
    {
        u8Crc += *(Data + u8DataIndex);
    }

    return u8Crc;
}
/**
 * @brief 
 * 
 */
static bool MBacklightControl_CallbackAccessFlashCalibrationData(MTemperatureDeratingAccessFlashCalibrationData* ptrFlashCalibrationData,\
TEMP_DERATING_FLASH_OPERATE eDeratingFlashOperate)
{
    bool bResult = false;
    uint8_t u8AccessFlashData[FIXED_FLASH_ACCESS_PAGE_BYTE_SIZE];
    const uint8_t u8DeratingLimitedTemperatureByteSize = sizeof(ptrFlashCalibrationData->uDeratingLimitedTemperatures.u8DeratingLimitedTemperatureByte);
    
    switch(eDeratingFlashOperate)
    {
        default:
            /* Do nothing. */
            bResult = false;
            break;

        case DERATING_FLASH_OPERATE_WRITE:
            memset(u8AccessFlashData, 0xFFU, sizeof(u8AccessFlashData));
            
            memcpy((u8AccessFlashData + E_MBL_FLASHDERATINGDATA_CHECKBYTE_LOCATION + E_MBL_FLASHDERATINGDATA_CHECKBYTE_SIZE),\
            ptrFlashCalibrationData->uDeratingLimitedTemperatures.u8DeratingLimitedTemperatureByte,\
            u8DeratingLimitedTemperatureByteSize);

            /* Calculate CRC */
            u8AccessFlashData[E_MBL_FLASHDERATINGDATA_CHECKBYTE_LOCATION] = \
            MBacklightControl_CalculateFlashCalibrationData((u8AccessFlashData + E_MBL_FLASHDERATINGDATA_CHECKBYTE_LOCATION + E_MBL_FLASHDERATINGDATA_CHECKBYTE_SIZE),\
            u8DeratingLimitedTemperatureByteSize);
            
            bResult = MFixedFlashAccess_WritePage(ADDR_DERATINGDATA_LIMITED_TEMP,\
            u8AccessFlashData,\
            (u8DeratingLimitedTemperatureByteSize + E_MBL_FLASHDERATINGDATA_CHECKBYTE_SIZE));

            if(!bResult)
            {
                break;
            }
            else
            {
                /* Write derating table to flash */
                memset(u8AccessFlashData, 0xFFU, sizeof(u8AccessFlashData));

                memcpy((u8AccessFlashData + E_MBL_FLASHDERATINGDATA_CHECKBYTE_LOCATION + E_MBL_FLASHDERATINGDATA_CHECKBYTE_SIZE),\
                ptrFlashCalibrationData->u8DeratingTable,\
                TEMP_DERATING_TABLE_SIZE);

                /* Calculate CRC */
                u8AccessFlashData[E_MBL_FLASHDERATINGDATA_CHECKBYTE_LOCATION] = \
                MBacklightControl_CalculateFlashCalibrationData((u8AccessFlashData + E_MBL_FLASHDERATINGDATA_CHECKBYTE_LOCATION + E_MBL_FLASHDERATINGDATA_CHECKBYTE_SIZE),\
                TEMP_DERATING_TABLE_SIZE);

                bResult = MFixedFlashAccess_WritePage(ADDR_DERATINGDATA_DUTY_TABLE,\
                u8AccessFlashData,\
                (TEMP_DERATING_TABLE_SIZE + E_MBL_FLASHDERATINGDATA_CHECKBYTE_SIZE));
        
                break;
            }
            

        case DERATING_FLASH_OPERATE_READ:
            /* Read derating limted Data */
            memset(u8AccessFlashData, 0xFFU, sizeof(u8AccessFlashData));
            bResult = MFixedFlashAccess_ReadPage(ADDR_DERATINGDATA_LIMITED_TEMP,\
            u8AccessFlashData,\
            (u8DeratingLimitedTemperatureByteSize + E_MBL_FLASHDERATINGDATA_CHECKBYTE_SIZE));
            
            if(!bResult)
            {
                break;
            }
            else if(u8AccessFlashData[E_MBL_FLASHDERATINGDATA_CHECKBYTE_LOCATION] !=\
            MBacklightControl_CalculateFlashCalibrationData((u8AccessFlashData + E_MBL_FLASHDERATINGDATA_CHECKBYTE_LOCATION + E_MBL_FLASHDERATINGDATA_CHECKBYTE_SIZE),\
            u8DeratingLimitedTemperatureByteSize))
            {
                bResult = false;
                
                break;
            }
            else
            {
                bResult = true;
                /* Updates limited temperature  data */
                memcpy(ptrFlashCalibrationData->uDeratingLimitedTemperatures.u8DeratingLimitedTemperatureByte,\
                (u8AccessFlashData + E_MBL_FLASHDERATINGDATA_CHECKBYTE_LOCATION + E_MBL_FLASHDERATINGDATA_CHECKBYTE_SIZE),\
                u8DeratingLimitedTemperatureByteSize);
            }
            
            /* Reads derating table Data */
            memset(u8AccessFlashData, 0xFFU, sizeof(u8AccessFlashData));
            bResult = MFixedFlashAccess_ReadPage(ADDR_DERATINGDATA_DUTY_TABLE,\
            u8AccessFlashData,\
            (TEMP_DERATING_TABLE_SIZE + E_MBL_FLASHDERATINGDATA_CHECKBYTE_SIZE));

            if(!bResult)
            {
                break;
            }
            else if(u8AccessFlashData[E_MBL_FLASHDERATINGDATA_CHECKBYTE_LOCATION] !=\
            MBacklightControl_CalculateFlashCalibrationData((u8AccessFlashData + E_MBL_FLASHDERATINGDATA_CHECKBYTE_LOCATION + E_MBL_FLASHDERATINGDATA_CHECKBYTE_SIZE),\
            TEMP_DERATING_TABLE_SIZE))
            {
                bResult = false;
                
                break;
            }
            else
            {
                bResult = true;
                /* Updates derating table Data */
                memcpy(ptrFlashCalibrationData->u8DeratingTable,\
                (u8AccessFlashData + E_MBL_FLASHDERATINGDATA_CHECKBYTE_LOCATION + E_MBL_FLASHDERATINGDATA_CHECKBYTE_SIZE),\
                TEMP_DERATING_TABLE_SIZE);
                
                break;
            }
    }

    return bResult;
}
/**
 * @brief Registers pwm callback func.
 * 
 * @details 1.Registers pwm callback func.\n
 * 2.Init temperature derating module.
 * 
 * @note It's state machine func. 
 * 
 * @return MBacklightControlStateMachine_E The next control state. 
 * 
 */
static MBacklightControlStateMachine_E MBacklightControl_StateInit(void)
{
    MBacklightControlStateMachine_E TempBacklightStateMachine;

    /* Registers callback function for sending PWM signal. */
    MPWMDimming_RegisterPWMDriverCtrl(MBacklightControl_SendPWMSignal);

#if(BACKDOOR_WRITE_DERATINGDATA)
    /* Init temperature derating module. */
    TemperatureDerating_Init(MBacklightControl_UpdateTERR,\
    Memory_Pool_PCBATemp_Get,\
    MBacklightControl_CallbackAccessFlashCalibrationData);
#else
    /* Init temperature derating module. */
    TemperatureDerating_Init(MBacklightControl_UpdateTERR,\
    Memory_Pool_PCBATemp_Get,\
    NULL);
#endif

    
    /* Clears TERR */
    MBacklightControl_UpdateTERR(TEMP_DERATING_TERR_CLEAR);

    TempBacklightStateMachine = E_MBL_STATEMACHINE_WAITENABLE;
    return TempBacklightStateMachine;
}
/**
 * @brief Waits backlight enable.
 * 
 * @details None.
 * 
 * @note It's state machine func. 
 * 
 * @return MBacklightControlStateMachine_E The next control state. 
 * 
 */
static MBacklightControlStateMachine_E MBacklightControl_StateWaitEnable(void)
{
    uint16_t u16TempHostPWM = 0;
    MBacklightControlStateMachine_E TempBacklightStateMachine;

    switch(StateMachineControl.eMBLCtrlExternalEn)
    {
        default:
            TempBacklightStateMachine = E_MBL_STATEMACHINE_WAITENABLE;
            break;

        case E_MBL_EXTERNAL_ENABLE_DIMMING:
        case E_MBL_EXTERNAL_ENABLE_NODIMMNG:
            u16TempHostPWM = Memory_Pool_BacklightDuty_Get();
            if(StateMachineControl.u16CurrentPwmValue != u16TempHostPWM)
            {
                MPWMDimming_BLBrightness_Ctrl(HIGHYTE(u16TempHostPWM),LOWBYTE(u16TempHostPWM),StateMachineControl.eMBLCtrlDimmingEn);
                StateMachineControl.u16CurrentPwmValue = u16TempHostPWM;
                /* Recovers dimming enable */
                StateMachineControl.eMBLCtrlDimmingEn = E_MBL_DIMMING_EN;
            }
            TempBacklightStateMachine = E_MBL_STATEMACHINE_NORMAL;
            break;

        case E_MBL_EXTERNAL_DISABLE_DIMMING:
        case E_MBL_EXTERNAL_DISABLE_NODIMMNG:
            TempBacklightStateMachine = E_MBL_STATEMACHINE_WAITENABLE;
            break;
    }

    return TempBacklightStateMachine;
}
/**
 * @brief Outputs dimming backlight of the host backlight request.
 * 
 * @details 1.Next state is derating if the temperature over 70 celsius.\n
 * 2.Catchs SDM ambient temperature.\n
 * 3.Decides next state.\n
 * 4.Catchs host pwm request.\n
 * 5.Sends host dimming pwm.
 * 
 * @note It's state machine func. 
 * 
 * @return MBacklightControlStateMachine_E The next control state. 
 * 
 */
static MBacklightControlStateMachine_E MBacklightControl_StateNormal(void)
{
    uint16_t u16TempHostPWM = Memory_Pool_BacklightDuty_Get();
    MBacklightControlStateMachine_E TempBacklightStateMachine;

#if(HIJACK_ADC)
    if( 0x01U == gbHijackPCBA )
    	StateMachineControl.AmbientTemperature = gi16FakePCBA*TEMP_DERATING_TEMP_RESOLUTION;
    else
    	StateMachineControl.AmbientTemperature = Memory_Pool_PCBATemp_Get()*TEMP_DERATING_TEMP_RESOLUTION;
#else
    StateMachineControl.AmbientTemperature = Memory_Pool_PCBATemp_Get()*TEMP_DERATING_TEMP_RESOLUTION;
#endif

    if((StateMachineControl.eMBLCtrlExternalEn == E_MBL_EXTERNAL_DISABLE_DIMMING)\
        ||(StateMachineControl.eMBLCtrlExternalEn == E_MBL_EXTERNAL_DISABLE_NODIMMNG))
    {
        TempBacklightStateMachine = E_MBL_STATEMACHINE_SHUTDOWM;
    }
	else if(DERATING_DISABLE == TemperatureDerating_TurnOnOff_Get())
	{
		TempBacklightStateMachine = E_MBL_STATEMACHINE_NORMAL;
	}
    else if(StateMachineControl.AmbientTemperature > TemperatureDerating_GetLimitedTemperature(LIMITED_DERATING_TEMP))
    {
        TempBacklightStateMachine = E_MBL_STATEMACHINE_DERATING;
    }
    else
    {
       TempBacklightStateMachine = E_MBL_STATEMACHINE_NORMAL;
    }

    if(StateMachineControl.u16CurrentPwmValue != u16TempHostPWM)
    {
        MPWMDimming_BLBrightness_Ctrl(HIGHYTE(u16TempHostPWM),LOWBYTE(u16TempHostPWM),StateMachineControl.eMBLCtrlDimmingEn);
        StateMachineControl.u16CurrentPwmValue = u16TempHostPWM;
        /* Recovers dimming enable */
        StateMachineControl.eMBLCtrlDimmingEn = E_MBL_DIMMING_EN;
    }

    /* Sends BL enable callback. */
    if((StateMachineControl.CallbackExAlertEnComplete.bReceivedAlert)\
        &&(MPWMDimming_ChangePWMStatus_Get() == MPWMDIMMING_CHANGEPWM_STOP)\
        &&(StateMachineControl.CallbackExAlertEnComplete.CallbackExAlertHandlerComplete != NULL))
    {
        StateMachineControl.CallbackExAlertEnComplete.bReceivedAlert = false;
        StateMachineControl.CallbackExAlertEnComplete.CallbackExAlertHandlerComplete();
    }
    else if((StateMachineControl.CallbackExAlertEnComplete.bReceivedAlert)\
            &&(StateMachineControl.CallbackExAlertEnComplete.CallbackExAlertHandlerComplete != NULL))
    {
         StateMachineControl.CallbackExAlertEnComplete.bReceivedAlert = false;
    }
    else{/* NA */}

    return TempBacklightStateMachine;
}
/**
 * @brief Catchs derating pwm
 * 
 * @details 1.Next state is normal or shutdown.\n
 * 2.Catchs SDM ambient temperature.\n
 * 3.Decides next state.\n
 * 4.Catchs derating pwm request.\n
 * 5.Sends derating dimming pwm.
 * 
 * @return MBacklightControlStateMachine_E 
 */
static MBacklightControlStateMachine_E MBacklightControl_StateDerating(void)
{
    uint16_t u16TempHostPWM = Memory_Pool_BacklightDuty_Get();
    uint16_t u16TempDeratingPWM = (uint16_t)(((uint32_t)DeratingApp_DeratingOutData_Get() * (uint32_t)FULLPWM) / (uint32_t)TEMP_DERATING_FULL_OUTDATA);
    MBacklightControlStateMachine_E TempBacklightStateMachine;

    StateMachineControl.AmbientTemperature = Memory_Pool_PCBATemp_Get()*TEMP_DERATING_TEMP_RESOLUTION;

    if((StateMachineControl.eMBLCtrlExternalEn == E_MBL_EXTERNAL_DISABLE_DIMMING)\
        ||(StateMachineControl.eMBLCtrlExternalEn == E_MBL_EXTERNAL_DISABLE_NODIMMNG))
    {
        TempBacklightStateMachine = E_MBL_STATEMACHINE_SHUTDOWM;
    }
	else if(DERATING_DISABLE == TemperatureDerating_TurnOnOff_Get())
	{
		TempBacklightStateMachine = E_MBL_STATEMACHINE_NORMAL;
	}
    else if(StateMachineControl.AmbientTemperature >= TemperatureDerating_GetLimitedTemperature(LIMITED_SHUTDOWN_TEMP))
    {
		Memory_Pool_GeneralDiagnosis_Set(Memory_Pool_GeneralDiagnosis_Get() | BIT_A3_BL_SHUTDOWN_ERROR_POS);
        TempBacklightStateMachine = E_MBL_STATEMACHINE_SHUTDOWM;
    }
    else if(StateMachineControl.AmbientTemperature <= TemperatureDerating_GetLimitedTemperature(LIMITED_DERATING_TEMP))
    {
        TempBacklightStateMachine = E_MBL_STATEMACHINE_NORMAL;
    }
    else
    {
        TempBacklightStateMachine = E_MBL_STATEMACHINE_DERATING;
    }

    /* Sets and compare BL pwm */
    u16TempHostPWM = (u16TempHostPWM >= u16TempDeratingPWM)?\
    u16TempDeratingPWM:\
    u16TempHostPWM;

    if(StateMachineControl.u16CurrentPwmValue != u16TempHostPWM)
    {
        MPWMDimming_BLBrightness_Ctrl(HIGHYTE(u16TempHostPWM),LOWBYTE(u16TempHostPWM),StateMachineControl.eMBLCtrlDimmingEn);
        StateMachineControl.u16CurrentPwmValue = u16TempHostPWM;
        /* Recovers dimming enable */
        StateMachineControl.eMBLCtrlDimmingEn = E_MBL_DIMMING_EN;
    }
    else{/* NA */}

    /* Sends BL enable callback. */
    if((StateMachineControl.CallbackExAlertEnComplete.bReceivedAlert)\
        &&(MPWMDimming_ChangePWMStatus_Get() == MPWMDIMMING_CHANGEPWM_STOP)\
        &&(StateMachineControl.CallbackExAlertEnComplete.CallbackExAlertHandlerComplete != NULL))
    {
        StateMachineControl.CallbackExAlertEnComplete.bReceivedAlert = false;
        StateMachineControl.CallbackExAlertEnComplete.CallbackExAlertHandlerComplete();
    }
    else if((StateMachineControl.CallbackExAlertEnComplete.bReceivedAlert)\
            &&(StateMachineControl.CallbackExAlertEnComplete.CallbackExAlertHandlerComplete != NULL))
    {
        StateMachineControl.CallbackExAlertEnComplete.bReceivedAlert = false;
    }
    else{/* NA */}

    return TempBacklightStateMachine;
}
/**
 * @brief Turns off pwm.
 * 
 * @details 1.Next state is derating.\n
 * 2.Decides next state.\n
 * 
 * @return MBacklightControlStateMachine_E The next control state.
 */
static MBacklightControlStateMachine_E MBacklightControl_StateShutdown(void)
{
    MBacklightControlStateMachine_E TempBacklightStateMachine;

    StateMachineControl.AmbientTemperature = Memory_Pool_PCBATemp_Get()*TEMP_DERATING_TEMP_RESOLUTION;

    if((StateMachineControl.eMBLCtrlExternalEn == E_MBL_EXTERNAL_DISABLE_DIMMING)\
        ||(StateMachineControl.eMBLCtrlExternalEn == E_MBL_EXTERNAL_DISABLE_NODIMMNG))
    {
        TempBacklightStateMachine = E_MBL_STATEMACHINE_SHUTDOWM;
    }
    else if(DERATING_DISABLE == TemperatureDerating_TurnOnOff_Get())
    {
        TempBacklightStateMachine = E_MBL_STATEMACHINE_NORMAL;
    }
    else if(StateMachineControl.AmbientTemperature > TemperatureDerating_GetLimitedTemperature(LIMITED_SHUTDOWN_REL_TEMP))
    {
		Memory_Pool_GeneralDiagnosis_Set(Memory_Pool_GeneralDiagnosis_Get() | BIT_A3_BL_SHUTDOWN_ERROR_POS);
        TempBacklightStateMachine = E_MBL_STATEMACHINE_SHUTDOWM;
    }
    else
    {
		Memory_Pool_GeneralDiagnosis_Set(Memory_Pool_GeneralDiagnosis_Get() & (~BIT_A3_BL_SHUTDOWN_ERROR_POS));
        TempBacklightStateMachine = E_MBL_STATEMACHINE_DERATING;
    }

    if(StateMachineControl.u16CurrentPwmValue != EMPTYPWM)
    {
        MPWMDimming_BLBrightness_Ctrl(HIGHYTE(EMPTYPWM),LOWBYTE(EMPTYPWM),StateMachineControl.eMBLCtrlDimmingEn);
        StateMachineControl.u16CurrentPwmValue = EMPTYPWM;
        /* Recovers dimming enable */
        StateMachineControl.eMBLCtrlDimmingEn = E_MBL_DIMMING_EN;
    }
    else{/* NA */}

    /* Sends BL disable callback. */
    if((StateMachineControl.CallbackExAlertDisComplete.bReceivedAlert)\
        &&(MPWMDimming_ChangePWMStatus_Get() == MPWMDIMMING_CHANGEPWM_STOP)\
        &&(StateMachineControl.CallbackExAlertDisComplete.CallbackExAlertHandlerComplete != NULL))
    {
        StateMachineControl.CallbackExAlertDisComplete.bReceivedAlert = false;
        StateMachineControl.CallbackExAlertDisComplete.CallbackExAlertHandlerComplete();
    }
    else if((StateMachineControl.CallbackExAlertDisComplete.bReceivedAlert)\
            &&(StateMachineControl.CallbackExAlertDisComplete.CallbackExAlertHandlerComplete != NULL))
    {
        StateMachineControl.CallbackExAlertDisComplete.bReceivedAlert = false;
    }
    else{/* NA */}

    return TempBacklightStateMachine;
}


/* -- Global Functions -- */
/**
 * @brief To set up enable or disable to temperature derating module.
 * 
 * @details None.
 * 
 * @note None.
 * 
 * @param eMBLCtrlDeratingEn To Enable or disable to temperature derating module.
 * 
 */
void MBacklightControl_TurnOnOffDerating(MBacklightControlDeratingEnable_E eMBLCtrlDeratingEn)
{
    switch(eMBLCtrlDeratingEn)
    {
        default:
            /* Do nothing */
            break;

        case E_MBL_ENABLE_DERATING:
            TemperatureDerating_TurnOnOff_Set(DERATING_ENABLE);
            break;

        case E_MBL_DISABLE_DERATING:
        	TemperatureDerating_TurnOnOff_Set(DERATING_DISABLE);
            break;
    }
}
/**
 * @brief External alert to enable or disable BL.
 * 
 * @param eMBLCtrlExternalEn Enable or disable BL
 */
void MBacklightControl_ExternalTurnOnOffBL(MBacklightControlExternalEnable_E eMBLCtrlExternalEn)
{
    switch(eMBLCtrlExternalEn)
    {
        default:
            break;

        case E_MBL_EXTERNAL_ENABLE_DIMMING:
            StateMachineControl.CallbackExAlertEnComplete.bReceivedAlert = true;
            StateMachineControl.eMBLCtrlExternalEn = E_MBL_EXTERNAL_ENABLE_DIMMING;
            StateMachineControl.eMBLCtrlDimmingEn = E_MBL_DIMMING_EN;
            break;
        
        case E_MBL_EXTERNAL_DISABLE_DIMMING:
            StateMachineControl.CallbackExAlertDisComplete.bReceivedAlert = true;
            StateMachineControl.eMBLCtrlExternalEn = E_MBL_EXTERNAL_DISABLE_DIMMING;
            StateMachineControl.eMBLCtrlDimmingEn = E_MBL_DIMMING_EN;
            break;

        case E_MBL_EXTERNAL_ENABLE_NODIMMNG:
            StateMachineControl.CallbackExAlertEnComplete.bReceivedAlert = true;
            StateMachineControl.eMBLCtrlExternalEn = E_MBL_EXTERNAL_ENABLE_NODIMMNG;
            StateMachineControl.eMBLCtrlDimmingEn = E_MBL_DIMMING_DIS;
            break;

        case E_MBL_EXTERNAL_DISABLE_NODIMMNG:
            StateMachineControl.CallbackExAlertDisComplete.bReceivedAlert = true;
            StateMachineControl.eMBLCtrlExternalEn = E_MBL_EXTERNAL_DISABLE_NODIMMNG;
            StateMachineControl.eMBLCtrlDimmingEn = E_MBL_DIMMING_DIS;
            TemperatureDerating_SaveCalibrationData();
            break;
    }
}
/**
 * @brief 
 * 
 */
bool MBacklightControl_RegisterExternalEnableCallback(\
CALLBACK_EX_ALERT_HANDLER_COMPLETE CallbackExAlertEnComplete,\
CALLBACK_EX_ALERT_HANDLER_COMPLETE CallbackExAlertDisComplete)
{
    if((CallbackExAlertEnComplete == NULL)\
        ||(CallbackExAlertDisComplete == NULL))
    {
        return false;
    }
    else{/* NA */}

    /* Registers External Callback Functions. */
    StateMachineControl.CallbackExAlertEnComplete.CallbackExAlertHandlerComplete = CallbackExAlertEnComplete;
    StateMachineControl.CallbackExAlertDisComplete.CallbackExAlertHandlerComplete = CallbackExAlertDisComplete;

    return true;
}

/**
 * @brief If SDM receive I2C BL Commnad shall call the function to execute BL control state machine.
 * 
 * @details i16AmbientTemperature is condition for changing state.
 * 
 * @note If adding other condition shall be adjuested inputs of this function.
 * 
 * @param i16AmbientTemperature Checks for changing state. 
 * 
 */
MBacklightControlStateMachine_E MBacklightControl_TriggerStateMachine(void)
{
    switch(StateMachineControl.BacklightStateMachine)
    {
        case E_MBL_STATEMACHINE_INIT:
            StateMachineControl.BacklightStateMachine = MBacklightControl_StateInit();
            break;

        case E_MBL_STATEMACHINE_WAITENABLE:
            StateMachineControl.BacklightStateMachine = MBacklightControl_StateWaitEnable();
            break;
        
        case E_MBL_STATEMACHINE_NORMAL:
            StateMachineControl.BacklightStateMachine = MBacklightControl_StateNormal();
            break;
        
        case E_MBL_STATEMACHINE_DERATING:
            StateMachineControl.BacklightStateMachine = MBacklightControl_StateDerating();
            break;
       
       case E_MBL_STATEMACHINE_SHUTDOWM:
            StateMachineControl.BacklightStateMachine = MBacklightControl_StateShutdown();
            break;

        default:
            StateMachineControl.BacklightStateMachine = MBacklightControl_StateNormal();
            break;
    }

    return StateMachineControl.BacklightStateMachine;
}
/**
 * @brief Calls DeratingApp_Routine() to run independent state machine.
 * 
 * @details 1.If sets different routine base time by changing Marco define in M_Derating.h.\n
 * 2.If sets different state machine checking time by changing Marco define in M_Derating.h.
 * 
 * @note 1.The routine base time is 10ms.\n
 * 2.The state machine checking time is 20ms.
 * 
 */
void MBacklightControl_DeratingModuleRoutine10ms(void)
{
    DeratingApp_Routine();
}
/**
 * @brief Calls MPWMDimming_Routine6ms() to run independent state machine.
 * 
 * @details 1.The different routine base time is immutable.\n
 * 2.The different state machine checking time is immutable..
 * 
 * @note 1.The state machine checking time is 6ms.\n
 * 
 */
void MBacklightControl_DimmingModuleRoutine6ms(void)
{
    MPWMDimming_Routine6ms();
}


//#if (BX726_TDDI_NT51926)
/**
 * @brief Calls MPWMDimming_Routine6ms() to run independent state machine.
 *
 * @details 1.The different routine base time is immutable.\n
 * 2.The different state machine checking time is immutable..
 *
 * @note 1.The state machine checking time is 6ms.\n
 *
 */
MBacklightControlStateMachine_E MBacklightControl_GetBacklightState(void)
{
    return StateMachineControl.BacklightStateMachine;
}


#if (HIJACK_ADC)
uint8_t MBacklightControl_ADCHiJack(uint8_t u8OnOff)
{
	gbHijackPCBA = u8OnOff;
	return gbHijackPCBA;
}
uint8_t MBacklightControl_GetADCHiJack(void)
{
	return gbHijackPCBA;
}

uint8_t MBacklightControl_SetHADC(int16_t fakeADC)
{
	gi16FakePCBA = fakeADC;
	return 1U;
}

int16_t MBacklightControl_GetHADC(void)
{
	return gi16FakePCBA;
}
#endif
//#endif


/* -- END -- */
