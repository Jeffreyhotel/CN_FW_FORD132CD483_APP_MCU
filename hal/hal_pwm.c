/**
 * @file hal_pwm.c
 * 
 * @author orlando huang (orlando.huang@auo.com)
 * 
 * @brief 
 * 
 * @version 0.1
 * 
 * @date 2022-08-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
/* -- Includes -- */
#include "hal_pwm.h"
#include "../app/Mymain.h"

/* -- Marco Define -- */

/* -- Data Type Define -- */
typedef void (*CALLBACK_IRQ_CC)(void);

typedef struct
{
	cy_stc_sysint_t IntPwmConfig;
	cy_stc_sysint_t *pIntPwmConfig;
	uint32_t u32Compare0Value;
	bool bUpdateLock;
	uint32_t u32InttruptStatus;
	CALLBACK_IRQ_CC CallbackInttruptCC;
	
}HALPwm_Config;

/* -- Global Variables -- */
static HALPwm_Config HALPwmConfig = \
{
	.IntPwmConfig = {.intrSrc = PWM_OUT_DIM_IRQ, .intrPriority = 3U},
	.pIntPwmConfig = NULL,
	.u32Compare0Value = 0U,
	.u32InttruptStatus = 0U,
	.bUpdateLock = false,
	.CallbackInttruptCC = NULL,
};

/* -- Local Functions -- */
/**
 * @brief 
 * 
 */
static void HAL_PWM_CH4_CallbackInterruptCC(void)
{
    /* Get all the enabled pending interrupts */
	HALPwmConfig.u32InttruptStatus = Cy_TCPWM_GetInterruptStatusMasked(PWM_OUT_DIM_HW, PWM_OUT_DIM_NUM);

    if (0UL != (CY_TCPWM_INT_ON_TC & HALPwmConfig.u32InttruptStatus))
    {
        /* Handle the Terminal Count event */
    }
    
    if (0UL != (CY_TCPWM_INT_ON_CC & HALPwmConfig.u32InttruptStatus))
    {
        /* Handle the Compare/Capture event */
        if(!HALPwmConfig.bUpdateLock)
		{
			/* Bypass */
		}
		else
		{
			Cy_TCPWM_PWM_SetCompare0(PWM_OUT_DIM_HW, PWM_OUT_DIM_NUM, HALPwmConfig.u32Compare0Value);
			HALPwmConfig.bUpdateLock = false;
		}
    }
	
	/* Clear the interrupt */
	Cy_TCPWM_ClearInterrupt(PWM_OUT_DIM_HW, PWM_OUT_DIM_NUM, HALPwmConfig.u32InttruptStatus);
}
/**
 * @brief 
 * 
 * @param CallbackIrqCC 
 */
static bool HAL_PWM_CH4_RegisterIntCallback(HALPwm_Config tHalPwmconfig)
{
	bool bResult = false;

	if((tHalPwmconfig.pIntPwmConfig == NULL)\
		|| (tHalPwmconfig.CallbackInttruptCC == NULL))
	{
		bResult = false;
	}
	else
	{
		Cy_SysInt_Init(tHalPwmconfig.pIntPwmConfig, tHalPwmconfig.CallbackInttruptCC);
		NVIC_ClearPendingIRQ(tHalPwmconfig.pIntPwmConfig->intrSrc);
		NVIC_EnableIRQ(tHalPwmconfig.pIntPwmConfig->intrSrc);
		bResult = true;
	}

	return bResult;
}

/* -- Global Functions -- */
/**
 * @brief 
 * 
 * @return uint32_t 
 */
uint32_t HAL_PWM_CH4_Init(void)
{
	cy_rslt_t cyStatus;

    cyStatus = Cy_TCPWM_PWM_Init(PWM_OUT_DIM_HW, PWM_OUT_DIM_NUM, &PWM_OUT_DIM_config);
    
	if(cyStatus == CY_TCPWM_SUCCESS)
    {
		Cy_TCPWM_PWM_Enable(PWM_OUT_DIM_HW, PWM_OUT_DIM_NUM);
		Cy_TCPWM_TriggerStart(PWM_OUT_DIM_HW, PWM_OUT_DIM_MASK);
		HALPwmConfig.pIntPwmConfig = &(HALPwmConfig.IntPwmConfig);
		HALPwmConfig.CallbackInttruptCC = HAL_PWM_CH4_CallbackInterruptCC;
		/* Register CC Inttrupt callback */
		if(!HAL_PWM_CH4_RegisterIntCallback(HALPwmConfig))
		{
			cyStatus = RSLT_FAIL;
		}
		else
		{

		}
    }
	else
	{
		cyStatus = RSLT_FAIL;
	}

	return cyStatus;
}
/**
 * @brief 
 * 
 */
void HAL_PWM_CH4_DeInit(void)
{
	Cy_TCPWM_PWM_SetCompare0(PWM_OUT_DIM_HW, PWM_OUT_DIM_NUM, 0U);
	Cy_TCPWM_PWM_Disable(PWM_OUT_DIM_HW, PWM_OUT_DIM_NUM);
}
/**
 * @brief 
 * 
 * @param u32Channel 
 * 
 * @param u32DutyAdjust 
 * 
 */
void HAL_PWM_Duty_Output_Adjust(uint32_t u32Channel, uint32_t u32DutyAdjust)
{
	uint32_t u32Compare0 = Cy_TCPWM_PWM_GetCompare0(PWM_OUT_DIM_HW, PWM_OUT_DIM_NUM);
	uint32_t u32DutyOutput = 0UL;
	
	if(u32DutyAdjust >= (uint32_t)PWM_OUT_DIM_config.period0)
	{
		u32DutyOutput = (uint32_t)PWM_OUT_DIM_config.period0 + 1UL;
	}
	else
	{
		u32DutyOutput = u32DutyAdjust;
	}

	if(u32Compare0 >= PWM_OUT_DIM_config.period0)
	{
		if(u32Compare0 > u32DutyOutput)
		{
			Cy_TCPWM_PWM_SetCompare0(PWM_OUT_DIM_HW, PWM_OUT_DIM_NUM, u32DutyOutput);
		}
		else
		{
			/* NA */
		}
	}
	else
	{
		if(HALPwmConfig.bUpdateLock)
		{
			/* Wait */
		}
		else
		{
			HALPwmConfig.u32Compare0Value = u32DutyOutput;
			HALPwmConfig.bUpdateLock = true;
		}
	}
	(void)u32Channel;
}
/* -- END -- */
