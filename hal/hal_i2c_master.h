
/******************************************************************************
;				Program		:	hal_i2c_master.h
;				Function	:	I2C Master Function
;				Chip		:	Cypress CY8C4149AZI-S598
;				Date		:	2021 / 10 / 28
;				Author		:
;				Describe	:
******************************************************************************/
#pragma once

#ifndef HAL_I2C_MASTER_H
#define HAL_I2C_MASTER_H

/*---------------------------- Include File ---------------------------------*/
#include "public.h"

/*---------------------------- Definition -----------------------------------*/

/*---------------------------- Declare Type Define  -------------------------*/
typedef struct
{
	CySCB_Type *pBase;
	const cy_stc_scb_i2c_config_t *pConfig;
	cy_stc_scb_i2c_context_t  tContext;
}i2c_master_typedef;

/*---------------------------- Declare Function -----------------------------*/
void HAL_I2C_Master_Init(i2c_master_typedef tI2C);
uint8_t HAL_I2C_Master_Write(uint8_t u8Address, uint8_t *pData, uint16_t u16Size, uint32_t u32Timeout);
uint8_t HAL_I2C_Master_Read (uint8_t u8Address, uint8_t* pData, uint16_t u16Size, uint32_t u32Timeout);

#endif
