/******************************************************************************
;       Program		: crc8.h
;       Function	:
;       Chip		: Cypress CY8C4149AZI-S598
;       Clock		: IMO Internal 48MHz
;       Date		: 2021 / 10 / 07
;       Author		:
******************************************************************************/
#pragma once

#ifndef COMMON_H
#define COMMON_H

/*---------------------------- Include File ---------------------------------*/
#include "../Mymain.h"

/*---------------------------- Declare Function -----------------------------*/

typedef struct
{
	bool blEnable;
	uint8_t u8DebounceHigh;
	uint8_t u8DebounceLow;
	uint8_t u8CurrentGPIOStatus;
	uint8_t u8NewGPIOStatus;
	uint8_t u8DebounceMax;
}tgpio_debounce_def;

/*---------------------------- Definition -----------------------------------*/
#define GPIO_LOW					0x00U
#define GPIO_HIGH					0x01U

/*---------------------------- Declare Type Define  -------------------------*/
bool Common_Compare(uint32_t u32New,uint32_t u32Backup, uint32_t u32Mask);
bool Common_LevelDebounce(uint8_t u8IOLevel, tgpio_debounce_def *ptDebounce);
uint8_t Common_Checksum_Calculation(uint8_t *pu8data,uint8_t u8Length);
int16_t Common_Maximum(int16_t i16Arg1, int16_t i16Arg2);
int16_t Common_Minimum(int16_t i16Arg1, int16_t i16Arg2);
#endif
