/******************************************************************************
;				Program		:	hal_i2c_slave.c
;				Function	:	I2C Slave Function
;				Chip		:	Cypress CY8C4149AZI-S598
;				Date		:	2021 / 10 / 28
;				Author		:
;				Describe	:
******************************************************************************/
#pragma once

#ifndef HAL_I2C_SLAVE_H
#define HAL_I2C_SLAVE_H

/*---------------------------- Include File ---------------------------------*/
#include "public.h"

/*---------------------------- Definition -----------------------------------*/

/*---------------------------- Declare Type Define  -------------------------*/
typedef struct
{
	CySCB_Type *pBase;
	const cy_stc_scb_i2c_config_t *pConfig;
	cy_stc_scb_i2c_context_t  tContext;
	cy_stc_sysint_t tSysint;
}i2c_slave_typedef;

/*---------------------------- Declare Function -----------------------------*/
void HAL_I2C_Slave_Init(i2c_slave_typedef tI2C);
uint8_t HAL_I2C_Slave_TxBuf_Config (uint8_t* pu8Buf,uint32_t u32Size);
uint8_t HAL_I2C_Slave_RxBuf_Config  (uint8_t* pu8Buf,uint32_t u32Size);
uint8_t HAL_I2C_Slave_Register_Event_Callback(cy_cb_scb_i2c_handle_events_t pCallback);
uint32_t HAL_I2C_Slave_RxTransferCount_Get(void);
uint32_t HAL_I2C_Slave_TxTransferCount_Get(void);
uint32_t HAL_I2C_Slave_Status_Get(void);
uint32_t HAL_I2C_Slave_TxStatus_Clear(void);
uint32_t HAL_I2C_Slave_RxStatus_Clear(void);

#endif

