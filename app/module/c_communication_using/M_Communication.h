
/******************************************************************************
;       Program		: MC_Power_Management.h
;       Function	: Declare main function & variable
;       Chip		: Cypress CY8C4149AZI-S598
;       Clock		: IMO Internal 48MHz
;       Date		:
;       Author		:
******************************************************************************/
#pragma once

#ifndef M_COMMUNICATION_H
#define M_COMMUNICATION_H

#include "public.h"

/*---------------------------- Declare Type Define  --------------------------*/
typedef struct
{
	uint8_t u8RxRollingCounter;
	uint8_t u8TxRollingCounter;
	uint8_t u8Message;
	uint8_t u8ReadWrite;
}tcommunication_info_def;

//Read/Write Action
#define FORMAT_TO_BE_CHECK					0x00U
#define FORMAT_READ_CHECK_SAFETY			0x01U
#define FORMAT_WRITE_CHECK_SAFETY			0x02U
#define FORMAT_UNSUPPORT_SUBADDRESS			0x03U
#define FORMAT_LEN_FAIL						0x04U
#define FORMAT_READ_CRC_FAIL				0x05U
#define FORMAT_READ_RC_FAIL					0x06U
#define FORMAT_WRITE_CRC_FAIL				0x07U
#define FORMAT_WRITE_RC_FAIL				0x08U
#define FORMAT_READ_FORMAT_CORRECT			0x09U
#define FORMAT_WRITE_FORMAT_CORRECT			0x0AU


uint32_t M_COM_RxLength_Get(void);
void M_COM_TxBuffer_Config(uint8_t *pTxBuff, uint8_t u8Length);
void M_COM_RxBuffer_Config(uint8_t *pRxBuff, uint8_t u8Length);
void M_COM_TxBufferData_Set(uint8_t *pTxBuff, uint8_t u8Length);
void M_COM_I2cInit(uint8_t *pTxBuff, uint8_t u8TxSize, uint8_t *pRxBuff, uint8_t u8RxSize, cy_cb_scb_i2c_handle_events_t pCallback);
void M_COM_I2cDeInit(void);
uint8_t M_COM_RxMSGFormat_Check(uint8_t u8subaddress, uint8_t u8Length);
bool M_COM_RxMSGCRC8Value_Check(uint8_t *pRxBuff, uint8_t u8Length);
bool M_COM_RxRollingCounter_Check(uint8_t u8HostRollingCounter);

#endif
