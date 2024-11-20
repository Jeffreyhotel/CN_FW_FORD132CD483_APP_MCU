/******************************************************************************
;				Program		:	hal_systick.h
;				Function	:	SysTick Function
;				Chip		:	Cypress CY8C4149AZI-S598
;				Date		:	2021 / 10 / 28
;				Author		:
;				Describe	:
******************************************************************************/
#pragma once

#ifndef HAL_SYSTICK_H
#define HAL_SYSTICK_H

/*---------------------------- Include File ---------------------------------*/
#include "public.h"

/*---------------------------- Definition -----------------------------------*/
#define SYSTICK_COUNT	48000U

/*---------------------------- Declare Type Define  -------------------------*/
typedef struct
{
	cy_en_systick_clock_source_t tClockSource;
	uint32_t u32Interval;
	Cy_SysTick_Callback pCallback;

}systick_typedef;

/*---------------------------- Declare Function -----------------------------*/
void HAL_SYSTICK_Init(systick_typedef tTick );

#endif
