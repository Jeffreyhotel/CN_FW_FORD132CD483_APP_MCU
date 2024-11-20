/**
 * @file M_PWMDimming.c
 * 
 * @brief When call MPWMDimming_BLBrightness_Ctrl() to calculate dimming PWM value and send PWM signal.  
 * 
 * @details 1. Calls MPWMDimming_Routine6ms() 6ms periodically.\n
 * 2. Registers PWM callback function which sends PWM signal by mcu peripheral.\n
 * 3. Calls MPWMDimming_BLBrightness_Ctrl() to alculate dimming PWM value and send PWM signal.\n
 * 4. Calls MPWMDimming_CurrentBLPWM10bit_Get() to get value of the dimming 10bit PWM.
 * 
 * @note If does not register PWM callback function,dimming PWM could be calculated.
 * You could send PWM signal by your self after calling MPWMDimming_CurrentBLPWM10bit_Get() to get value.
 * 
 * @author Orlando Huang (orlando.huang@auo.com)
 * 
 * @version 0.1
 * 
 * @date 2022-05-25
 * 
 */

/* -- Includes -- */
#include "M_PWMDimming.h"
#include "Memory_Pool.h"

/* -- Marco Define -- */
#define PWMValMergeShift 8U /* For PWM value bit operation. */
#define FIXEDPOINT_10 10U /* For dimming resolution. */
/**
 * @brief 
 * 
 */
typedef enum
{
    eIncrease = 0U, /**< For dimming positive gain. */
    eDecrease /**< For dimming negtive gain. */
}MPWMDimmingOperator_E;
/**
 * @brief 
 * 
 */
typedef enum
{
    eDimmingCalculateIsNotFinished = 0U, /**< It is dimming now. */
    eDimmingCalculateIsFinished /**< Dimming is finished. */
}MPWMDimmingCalculateFinish_E;
/**
 * @brief 
 * 
 */
typedef enum
{
    eDimmingRegisterCallbackIsNotFinished = 0U, /**< Register callback is not vaild. */
    eDimmingRegisterCallbackIsFinished /**< Register callback is vaild. */
}MPWMDimmingRegisterCallback_E;

/* -- Data Type Define -- */
/**
 * @brief Control unit to dimming PWM.
 * 
 */
typedef struct
{
	uint8_t u8PWMpreStep; /**< It is used to record dimming gain. */
    MPWMDimmingOperator_E PWMOperator; /**< It is used to indicate the sign of dimming gain . */
	uint8_t u8DimmingStep; /**< It is used to record dimming step. */
	uint16_t u16TargetBLPWM_10bit; /**< It is used to record target pwm value after dimming. */
    MPWMDimmingCalculateFinish_E u8DimmingCalculateFinish; /**< It is indicated to the finished status of calculating dimming. */
    MPWMDimmingRegisterCallback_E u8IsRegisterCallbackSuccess; /**< It is indicated to the finished status of registering pwm callback func.. */
    MPWMDimmingChangePWM_E eMPWMDimmingChangePWM;
}MPWMDimmingControl_Typedef;

/* -- Global Variables -- */
/**
 * @brief Implements control unit to dimming PWM.
 * 
 * @details 1.Records dimming step.\n
 * 2.Calculates the backlight value at every step.  
 * 
 */
static MPWMDimmingControl_Typedef MPWMDimmingControl = 
{
    .u8PWMpreStep = 0U,
    .PWMOperator = eIncrease,
    .u8DimmingStep = 0U,
    .u16TargetBLPWM_10bit = 0U,
    .u8DimmingCalculateFinish = eDimmingCalculateIsNotFinished,
    .u8IsRegisterCallbackSuccess = eDimmingRegisterCallbackIsNotFinished,
    .eMPWMDimmingChangePWM = MPWMDIMMING_CHANGEPWM_STOP,
};
/**
 * @brief Records backlight dimming value at this step.
 * 
 * @details Records backlight dimming value at this step.
 * 
 */
static uint16_t u16CurrentBLPWM_10bit = 0U;
/**
 * @brief Records the step for calculating the dimminf backlight.
 * 
 * @details Records the step for calculating the dimminf backlight.
 * 
 */
static uint8_t u8DimmingStepCnt = 0U;
/**
 * @brief Records the callback PWM the Ref. of Func.
 * 
 * @details Records the callback PWM the Ref. of Func.
 * 
 */
static CallbackPWMDriverCtrl CallbackPWMDriver = NULL;

/* -- Local Functions -- */
/**
 * @brief Transfers from two bytes pwm to one short pwm
 * 
 * @details None.
 * 
 * @note It's local Func.
 * 
 * @param u8BLPWMVal_8_9 It's two high bits of pwm.
 *  
 * @param u8BLPWMVal_0_7 It's eight low bits of pwm.
 *  
 * @return uint16_t It's one short pwm.
 * 
 */
static uint16_t MPWMDimming_PWM10bit_Merge(uint8_t u8BLPWMVal_8_9,uint8_t u8BLPWMVal_0_7)
{
    uint16_t u16MergeResult = 0U;

    u16MergeResult |= ((uint16_t)(u8BLPWMVal_8_9) << PWMValMergeShift);
	u16MergeResult |= u8BLPWMVal_0_7;

    return u16MergeResult;
    
}
/**
 * @brief Sets the target pwm when dimming has been finished. 
 * 
 * @details None.
 * 
 * @note It's local Func.
 * 
 * @param u16BLPWMtargetVal The target pwm.
 * 
 * @return uint8_t Has it been success to set. 
 *  
 */
static uint8_t MPWMDimming_BLPWMTargetVal_Set(uint16_t u16BLPWMtargetVal)
{
	uint8_t u8ProcessStatus = 0U;

	MPWMDimmingControl_Typedef TempMPWMDimmingControl = MPWMDimmingControl;

    if(u16BLPWMtargetVal >= 0x3ff)
    {

        TempMPWMDimmingControl.u16TargetBLPWM_10bit = 0x3ff;

    }
    else
    {
        TempMPWMDimmingControl.u16TargetBLPWM_10bit = u16BLPWMtargetVal;
    }
  
    MPWMDimmingControl = TempMPWMDimmingControl;
   
    u8ProcessStatus = 1U; 
	
    return u8ProcessStatus;
}
/**
 * @brief Calculates the difference pwm between current and target pwms.
 * 
 * @details 1.Calculates the difference pwm between current and target pwms.\n
 * 2.Calculates the sign of difference pwm.
 * 
 * @note It's local Func.
 * 
 * @param u16CurrentBLPWM The current pwm. 
 * 
 * @param u16TargetBLPWM The target pwm.
 * 
 * @return uint16_t The difference pwm between current and target pwm.
 * 
 */
static uint16_t MPWMDimming_BLPWMDifference_Calculate(uint16_t u16CurrentBLPWM,uint16_t u16TargetBLPWM)
{
    uint16_t u16DiffPwmValResult = 0U;
    (void)u16DiffPwmValResult;

    if (u16TargetBLPWM > u16CurrentBLPWM)
    {
        /* code */
        u16DiffPwmValResult = u16TargetBLPWM - u16CurrentBLPWM;
        MPWMDimmingControl.PWMOperator = eIncrease;
    }
    else if((u16TargetBLPWM < u16CurrentBLPWM))
    {
        /* code */
        u16DiffPwmValResult = u16CurrentBLPWM - u16TargetBLPWM;
        MPWMDimmingControl.PWMOperator = eDecrease;
    }
    else
    {
        u16DiffPwmValResult = 0U;
    }
    
    return u16DiffPwmValResult;
}
/**
 * @brief To initialize MPWMDimmingControl.
 * 
 * @details To initialize MPWMDimmingControl if it is not excuting dimming.
 * 
 * @note It's local Func.
 * 
 * @return uint8_t It is finished calculating status.
 * 
 */
static uint8_t MPWMDimming_NonDimming_Set(void)
{  
    MPWMDimmingControl.u8PWMpreStep = 0U;
    MPWMDimmingControl.u8DimmingStep = 0U;
    MPWMDimmingControl.u8DimmingCalculateFinish = eDimmingCalculateIsFinished;
   
    return MPWMDimmingControl.u8DimmingCalculateFinish; 
}
/**
 * @brief Calculates dimming backlight step and difference.
 * 
 * @details None.
 * 
 * @note It's local Func.
 * 
 * @param u16BLPWMDiffVal The difference at every step.  
 * 
 * @return uint8_t Is it finished for dimming every step.
 * 
 */
static uint8_t MPWMDimming_BalckLightDimming_Calculate(uint16_t u16BLPWMDiffVal)
{
    if(u16BLPWMDiffVal >= 128U)
    {
        MPWMDimmingControl.u8PWMpreStep = ((uint16_t)(u16BLPWMDiffVal+1)*FIXEDPOINT_10) >> 7U ; /* FIXEDPOINT_10 1 -> 10 , /128 */
        MPWMDimmingControl.u8DimmingStep = 128U;
        MPWMDimmingControl.u8DimmingCalculateFinish = eDimmingCalculateIsFinished;
    }
    else if (0U < u16BLPWMDiffVal && u16BLPWMDiffVal < 128U)
    {
        MPWMDimmingControl.u8PWMpreStep = (uint16_t)FIXEDPOINT_10;
        MPWMDimmingControl.u8DimmingStep = (uint8_t)u16BLPWMDiffVal;
        MPWMDimmingControl.u8DimmingCalculateFinish = eDimmingCalculateIsFinished;
    }
    else
    {
        MPWMDimmingControl.u8DimmingCalculateFinish = eDimmingCalculateIsNotFinished;
    }

    return MPWMDimmingControl.u8DimmingCalculateFinish;    
}
/**
 * @brief Calculates dimming backlight at next step.
 * 
 * @details None.
 * 
 * @note It's local Func.
 * 
 * @param u16CurrentPWMVal The current pwm. 
 * 
 * @return uint16_t The next step pwm. 
 * 
 */
static uint16_t MPWMDimming_NextStepBLPWM_Calculate(uint16_t u16CurrentPWMVal)
{
    uint16_t u16NextStepBLPWMVal = 0U;

    if(MPWMDimmingControl.PWMOperator == eIncrease)
    {
        u16NextStepBLPWMVal = (u16CurrentPWMVal*FIXEDPOINT_10 + (u8DimmingStepCnt+1)*(MPWMDimmingControl.u8PWMpreStep))/FIXEDPOINT_10;
    }
    else if (MPWMDimmingControl.PWMOperator == eDecrease)
    {
        if(u16CurrentPWMVal*FIXEDPOINT_10 >= (u8DimmingStepCnt+1)*(MPWMDimmingControl.u8PWMpreStep))
        {
            u16NextStepBLPWMVal = (u16CurrentPWMVal*FIXEDPOINT_10 - (u8DimmingStepCnt+1)*(MPWMDimmingControl.u8PWMpreStep))/FIXEDPOINT_10;
        }
        else
        {
            u16NextStepBLPWMVal = 0;
        }
    }
    else
    {
        /* code */
    }

    return u16NextStepBLPWMVal;
}
/**
 * @brief Sends pwm signal to TDDI.
 * 
 * @details Calls the registered callback pwm function to send pwm signal.
 * 
 * @note It's local Func.
 * 
 * @param u16TargetBLPWM_10bit This set pwm.
 * 
 * @return uint16_t Updates the current pwm.
 * 
 */
static uint16_t MPWMDimming_BLPWM_Output(uint16_t u16TargetBLPWM_10bit)
{
    uint16_t u16BLPWMVal_16bit = 0U;

    u16BLPWMVal_16bit |= ((u16TargetBLPWM_10bit) << 0); /*0~32768 for backlight duty 0~100%,(u16TargetBLPWM_10bit + 1) * 1  */
    /* Excutes PWM Driver Callback Func. */
    if(CallbackPWMDriver != NULL)
    {
        CallbackPWMDriver(u16BLPWMVal_16bit);
    }
    else
    {
        // TBD
    }
    	
    u16CurrentBLPWM_10bit = u16TargetBLPWM_10bit;

    return u16CurrentBLPWM_10bit;
    
}

/* -- Global Functions -- */
/**
 * @brief Registers PWM callback function which sends PWM signal by mcu peripheral.
 * 
 * @details None.
 * 
 * @note It's external Func.
 * 
 * @param CallbackPWMDriverCtrl PWM callback function.
 * 
 */
void MPWMDimming_RegisterPWMDriverCtrl(CallbackPWMDriverCtrl CallbackPWMDriverFunc)
{
    if(CallbackPWMDriverFunc == NULL)
    {
        // Does not register
        MPWMDimmingControl.u8IsRegisterCallbackSuccess = eDimmingRegisterCallbackIsNotFinished;
    }
    else
    {
       MPWMDimmingControl.u8IsRegisterCallbackSuccess = eDimmingRegisterCallbackIsFinished;
       CallbackPWMDriver = CallbackPWMDriverFunc;
    }
}
/**
 * @brief 
 * 
 * @return uint16_t 
 */
uint16_t MPWMDimming_CurrentBLPWM10bit_Get (void)
{
    return u16CurrentBLPWM_10bit;
}
/**
 * @brief Sets the host backlight pwm to dimming module and enable it.
 * 
 * @details None.
 * 
 * @note It's external Func.
 * 
 * @param u8BLPWMVal_8_9 It's two high bits of backlight control target pwm.
 * 
 * @param u8BLPWMVal_0_7 It's eight low bits of backlight control target pwm.
 *  
 * @param u8DimmingEnable It's use to enable module.
 *  
 * @return MPWMDimmingBLBrightness_E Is it success to set and enable module?
 * 
 */
MPWMDimmingBLBrightness_E MPWMDimming_BLBrightness_Ctrl(uint8_t u8BLPWMVal_8_9,uint8_t u8BLPWMVal_0_7,uint8_t u8DimmingEnable)
{
    MPWMDimmingBLBrightness_E u8BLBC_Process_Status = eProcess_Ready;
    uint16_t u16PWMValMerged_10bit = 0U;
    uint16_t u16PWMValDifference_10bit = 0U;

    /* Mearge TargetVal */
    u16PWMValMerged_10bit = MPWMDimming_PWM10bit_Merge(u8BLPWMVal_8_9,u8BLPWMVal_0_7);
    
    /* Set target value */
    MPWMDimming_BLPWMTargetVal_Set(u16PWMValMerged_10bit);
    
    /* Calculate difference of current PWM and target PWM */
    u16PWMValDifference_10bit = MPWMDimming_BLPWMDifference_Calculate(u16CurrentBLPWM_10bit,u16PWMValMerged_10bit);

    /* Calculate how much PWM value every step will be changed */
    if(u16PWMValDifference_10bit != 0U)
    {
        /* Set changed pwm status. */
        MPWMDimmingControl.eMPWMDimmingChangePWM = MPWMDIMMING_CHANGEPWM_RUN;
        
        if(u8DimmingEnable)
        {
            MPWMDimming_BalckLightDimming_Calculate(u16PWMValDifference_10bit);
            u8BLBC_Process_Status = eDimmingCal_finished;
        }
        else
        {
            MPWMDimming_NonDimming_Set();
            u8BLBC_Process_Status = eNonDimmingCtrl_finished;
        }
    }
    else
    {
        /* Set changed pwm status. */
        MPWMDimmingControl.eMPWMDimmingChangePWM = MPWMDIMMING_CHANGEPWM_STOP;
    }

    return u8BLBC_Process_Status;
}
/**
 * @brief 
 * 
 * @return MPWMDimmingChangePWM_E 
 */
MPWMDimmingChangePWM_E MPWMDimming_ChangePWMStatus_Get(void)
{
    return MPWMDimmingControl.eMPWMDimmingChangePWM;
}
/**
 * @brief Control to calculate every step pwm per 6ms until the dimming pwm is equal to the target pwm.
 * 
 * @details None.
 * 
 * @note It's external Func.
 * 
 */
void MPWMDimming_Routine6ms(void)
{
	uint16_t PwmDimmingVal10bit;
	static uint16_t u16NowPWMVal = 0U;
    MPWMDimmingControl_Typedef TempMPWMDimmingControl = MPWMDimmingControl;

    if(TempMPWMDimmingControl.u8DimmingCalculateFinish == eDimmingCalculateIsFinished)
    {
        u8DimmingStepCnt = 0U;
        u16NowPWMVal = u16CurrentBLPWM_10bit;
    }
    else
    {
        // TBD
    }

    if(u8DimmingStepCnt < TempMPWMDimmingControl.u8DimmingStep)
    {
        PwmDimmingVal10bit = MPWMDimming_NextStepBLPWM_Calculate(u16NowPWMVal);
        Memory_Pool_NowDuty_Set(MPWMDimming_BLPWM_Output(PwmDimmingVal10bit));
        MPWMDimmingControl.eMPWMDimmingChangePWM = MPWMDIMMING_CHANGEPWM_RUN;
        u8DimmingStepCnt++;
    }
    else if(u8DimmingStepCnt == TempMPWMDimmingControl.u8DimmingStep)
    {
        Memory_Pool_NowDuty_Set(MPWMDimming_BLPWM_Output(TempMPWMDimmingControl.u16TargetBLPWM_10bit));
        MPWMDimmingControl.eMPWMDimmingChangePWM = MPWMDIMMING_CHANGEPWM_STOP;
        u8DimmingStepCnt++;
    }
    else
    {
        MPWMDimmingControl.eMPWMDimmingChangePWM = MPWMDIMMING_CHANGEPWM_STOP;
    }

    TempMPWMDimmingControl.u8DimmingCalculateFinish = eDimmingCalculateIsNotFinished;
    MPWMDimmingControl = TempMPWMDimmingControl;
}

/* -- END -- */
