
/******************************************************************************
;       Program		: MC_Power_Management.h
;       Function	: Declare main function & variable
;       Chip		: Cypress CY8C4149AZI-S598
;       Clock		: IMO Internal 48MHz
;       Date		:
;       Author		:
******************************************************************************/
#pragma once

#ifndef M_GPIOSENSE_H
#define M_GPIOSENSE_H

#include "public.h"

bool M_GPIOSense_LevelDeboucne(GPIO_PRT_Type *pPort, uint8_t u8Pin, tgpio_debounce_def *ptDebounce);
uint64_t M_GPIOSense_LED_Driver_Diagnosis(void);
bool M_GPIOSense_LED_Driver_Clear(void);
uint64_t M_GPIOSense_DisplayFault_Read(void);
uint8_t M_GPIOSense_NT51926_Status_Get(void);

#endif

