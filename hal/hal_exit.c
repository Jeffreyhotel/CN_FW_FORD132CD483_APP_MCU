/******************************************************************************
;				Program		:	hal_adc.c
;				Function	:	ADC Sensing Function
;				Chip		:	Cypress CY8C4149AZI-S598
;				Date		:	2021 / 10 / 28
;				Author		:
;				Describe	:	 
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
#include "hal_exit.h"

#include "../app/Mymain.h"

/*---------------------------- Start Program --------------------------------*/
/******************************************************************************
;       Function Name			:	void HAL_ADC_Init(void)
;       Function Description	:	Initialize ADC capture function
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:	U625_ADC-001
******************************************************************************/
void HAL_EXIT_Init(cy_stc_sysint_t *pSwitchIntrConfig, cy_israddress userIsr)
{

	cy_stc_sysclk_context_t tSysClkContext;
    cy_stc_syspm_callback_params_t tSysClkCallbackParams =
    {
        .base       = NULL,
        .context    = (void*)&tSysClkContext
    };

	Cy_SysInt_Init(pSwitchIntrConfig, userIsr);

	/* Clearing and enabling the GPIO interrupt in NVIC */
	NVIC_ClearPendingIRQ(pSwitchIntrConfig->intrSrc);
	NVIC_EnableIRQ(pSwitchIntrConfig->intrSrc);

    /* Callback declaration for Deep Sleep mode */
    cy_stc_syspm_callback_t tSysClkCallback =
    {
        .callback       = &Cy_SysClk_DeepSleepCallback,
        .type           = CY_SYSPM_DEEPSLEEP,
        .skipMode       = 0UL,
        .callbackParams = &tSysClkCallbackParams,
        .prevItm        = NULL,
        .nextItm        = NULL,
        .order          = 0
    };

	Cy_SysPm_RegisterCallback(&tSysClkCallback);
}

