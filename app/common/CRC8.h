/******************************************************************************
;       Program		: crc8.h
;       Function	:
;       Chip		: Cypress CY8C4149AZI-S598
;       Clock		: IMO Internal 48MHz
;       Date		: 2021 / 10 / 07
;       Author		:
******************************************************************************/
#pragma once

#ifndef CRC8_H
#define CRC8_H

/*---------------------------- Include File ---------------------------------*/
#include "../Mymain.h"

/*---------------------------- Declare Function -----------------------------*/
uint8_t CRC8_Calculation(uint8_t *pu8data,uint8_t u8Length);

#endif




