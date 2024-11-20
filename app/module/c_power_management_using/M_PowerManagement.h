
/******************************************************************************
;       Program		: MC_Power_Management.h
;       Function	: Declare main function & variable
;       Chip		: Cypress CY8C4149AZI-S598
;       Clock		: IMO Internal 48MHz
;       Date		:
;       Author		:
******************************************************************************/
#pragma once

#ifndef M_POWERMANAGEMENT_H
#define M_POWERMANAGEMENT_H

#include "public.h"

#define POWER_OFF					0x00U
#define POWER_ON					0x01U
#define POWER_OFF_READY				0x02U
#define POWER_ON_READY				0x03U

#define	P3V3_PGOOD					0x00U
#define	P1V2_PGOOD               	0x01U

#define POWER_VMINRCV_ADC_CFG		727 /*Define as 6.6V*/

#define POWER_VMAXRCV_ADC_CFG		2601 /*Define as 22V*/


extern uint8_t M_PM_Sequnce_Execute(uint8_t u8Action);

#endif
