
/******************************************************************************
;       Program		: hal_adc.h
;       Function	: Declare main function & variable
;       Chip		: Cypress CY8C4149AZI-S598
;       Clock		: IMO Internal 48MHz
;       Date		:
;       Author		:
******************************************************************************/
#pragma once

#ifndef HAL_EXIT_H
#define HAL_EXIT_H

#include "public.h"

/*---------------------------- Declare Type Define  -------------------------*/

void HAL_EXIT_Init(cy_stc_sysint_t *pSwitchIntrConfig, cy_israddress userIsr);

#endif

