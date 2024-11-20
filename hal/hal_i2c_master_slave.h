/******************************************************************************
;				Program		:	hal_i2c_master_slave.c
;				Function	:	I2C Master Slave Function
;				Chip		:	Cypress CY8C4149AZI-S598
;				Date		:	2021 / 10 / 28
;				Author		:
;				Describe	:
******************************************************************************/
#pragma once

#ifndef HAL_I2C_MASTER_SLAVE_H
#define HAL_I2C_MASTER_SLAVE_H

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
	cy_cb_scb_i2c_handle_events_t pCallback;
}i2c_master_slave_typedef;
/*---------------------------- Declare Function -----------------------------*/
void HAL_I2C_MS_Init(i2c_master_slave_typedef tI2C,uint8_t *pTxBuff, uint8_t u8TxSize, uint8_t *pRxBuff, uint8_t u8RxSize);
void HAL_I2C_MS_DeInit(i2c_master_slave_typedef tI2C);
uint8_t HAL_I2C_MS_TxBuf_Config (uint8_t* pu8Buf,uint32_t u32Size);
uint8_t HAL_I2C_MS_RxBuf_Config  (uint8_t* pu8Buf,uint32_t u32Size);
uint8_t HAL_I2C_MS_Register_Event_Callback(cy_cb_scb_i2c_handle_events_t pCallback);
uint32_t HAL_I2C_MS_RxTransferCount_Get(void);
uint32_t HAL_I2C_MS_TxTransferCount_Get(void);
uint32_t HAL_2C_MS_Status_Get(void);
uint32_t HAL_I2C_MS_TxStatus_Clear(void);
uint32_t HAL_I2C_MS_RxStatus_Clear(void);
bool HAL_I2C_MS_Write(uint8_t u8Address,uint8_t* pData,uint16_t u16Size,uint32_t u32Timeout);
bool HAL_I2C_MS_Read(uint8_t u8Address,uint8_t* pReadBuffer ,uint16_t u16Size,uint32_t u32Timeout);
uint8_t HAL_I2C_MS_Register_Event_Callback(cy_cb_scb_i2c_handle_events_t pCallback);

#endif
