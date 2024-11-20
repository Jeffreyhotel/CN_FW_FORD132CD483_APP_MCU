/******************************************************************************
;				Program		:	hal_uart.h
;				Function	:	SysTick Function
;				Chip		:	Cypress CY8C4149AZI-S598
;				Date		:	2021 / 10 / 28
;				Author		:
;				Describe	:
******************************************************************************/
#pragma once

#ifndef HAL_UART_H
#define HAL_UART_H

/*---------------------------- Include File ---------------------------------*/
#include "public.h"

/*---------------------------- Definition -----------------------------------*/
#if(CX430_TDDI_NT51926 || U717_TDDI_NT51926)
//#define DEBUG_UART_EN
#elif(BX726_TDDI_NT51926)
//#define DEBUG_UART_EN
#else
#endif

#define DEBUG_GPIO_EN
#define DEBUG_PORT				DEBUG_UART_HW

#ifdef DEBUG_GPIO_EN
#define DEBUG_TOGGLE()			HAL_GPIO_Toggle(TEST_PIN_PORT, TEST_PIN_PIN);\
								HAL_GPIO_Toggle(TEST_PIN_PORT, TEST_PIN_PIN);
#else
#define DEBUG_TOGGLE()
#endif

#define PRINTF_TIMEOUT 		(10U)
#define MAX_TEXT_LEN  		(256U)

/*---------------------------- Declare Type Define  --------------------------*/
typedef struct
{
	CySCB_Type *pBase;
	const cy_stc_scb_uart_config_t *pConfig;
	cy_stc_scb_uart_context_t tContext;
}debug_uart_tydef;

/*---------------------------- Declare Function -----------------------------*/
void HAL_UART_Init(debug_uart_tydef tDebugUart);
void HAL_UART_DeInit(debug_uart_tydef tDebugUart);
void HAL_UART_Printf(void *fmt, ...);

#endif
