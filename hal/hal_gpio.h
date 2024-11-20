/******************************************************************************
;				Program		:	hal_gpio.h
;				Function	:	GPIO Sensing Function
;				Chip		:	Cypress CY8C4149AZI-S598
;				Date		:	2021 / 10 / 28
;				Author		:
;				Describe	:
******************************************************************************/
#pragma once

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

/*---------------------------- Include File ---------------------------------*/
#include "public.h"

/*---------------------------- Definition -----------------------------------*/
#define PIN_NUM_MAX				0x08U

/*---------------------------- Declare Type Define  -------------------------*/

/*---------------------------- Declare Function -----------------------------*/ 
void HAL_GPIO_Read(GPIO_PRT_Type *u8Port, uint8_t u8Pin, uint8_t *pReturn);
void HAL_GPIO_Toggle(GPIO_PRT_Type *u8Port, uint8_t u8Pin);
void HAL_GPIO_High(GPIO_PRT_Type *u8Port, uint8_t u8Pin);
void HAL_GPIO_Low(GPIO_PRT_Type *u8Port, uint8_t u8Pin);

#endif

