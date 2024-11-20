/******************************************************************************
;				Program		:	hal_systick.c
;				Function	:	SysTick Function
;				Chip		:	Cypress CY8C4149AZI-S598
;				Date		:	2021 / 10 / 28
;				Author		:
;				Describe	:
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
#include "hal_systick.h"

static 	systick_typedef tSysTick;

/******************************************************************************
;       Function Name			:	void M_DU_DeInit(void)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
void HAL_SYSTICK_Init(systick_typedef tTick)
{
	tSysTick.tClockSource = tTick.tClockSource;
	tSysTick.u32Interval = tTick.u32Interval;
	tSysTick.pCallback = tTick.pCallback;

    if(tSysTick.pCallback != NULL)
    {
        (void)Cy_SysTick_Init(tSysTick.tClockSource, tSysTick.u32Interval);
    	(void)Cy_SysTick_SetCallback(0U,tSysTick.pCallback);
    }
    else
    {
    	/* Error */
    }
}
