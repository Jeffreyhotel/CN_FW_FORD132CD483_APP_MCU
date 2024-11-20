/******************************************************************************
;				Program		:	hal_i2c_slave.c
;				Function	:	I2C Slave Function
;				Chip		:	Cypress CY8C4149AZI-S598
;				Date		:	2021 / 10 / 28
;				Author		:
;				Describe	:
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
#include "hal_i2c_master_slave.h"

static i2c_master_slave_typedef tI2CMS;

/******************************************************************************
;       Function Name			:	void I2C_Slave_Isr(void)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
static void HAL_I2C_MS_ISR(void)
{
	if (tI2CMS.pBase != NULL)
	{
		Cy_SCB_I2C_Interrupt(tI2CMS.pBase, &tI2CMS.tContext);
	}
}
/******************************************************************************
;       Function Name			:	uint8_t HAL_I2C_Slave_Init(CySCB_Type* pSCB,cy_stc_scb_i2c_config_t* ptsConfig,cy_stc_sysint_t* ptsIRQ)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
void HAL_I2C_MS_Init(i2c_master_slave_typedef tI2C,uint8_t *pTxBuff, uint8_t u8TxSize, uint8_t *pRxBuff, uint8_t u8RxSize)
{
	tI2CMS.pBase = tI2C.pBase;
	tI2CMS.pConfig = tI2C.pConfig;
	tI2CMS.tSysint.intrSrc = tI2C.tSysint.intrSrc;
	tI2CMS.tSysint.intrPriority = tI2C.tSysint.intrPriority;
	tI2CMS.pCallback = tI2C.pCallback;

	Cy_SCB_I2C_Init(tI2CMS.pBase, tI2CMS.pConfig, &tI2CMS.tContext);

	/* Hook interrupt service routine */
	Cy_SysInt_Init(&tI2CMS.tSysint, HAL_I2C_MS_ISR);
	NVIC_EnableIRQ((IRQn_Type) tI2CMS.tSysint.intrSrc);

	HAL_I2C_MS_Register_Event_Callback(tI2CMS.pCallback);

	HAL_I2C_MS_RxBuf_Config(pRxBuff, u8RxSize);

	/* Enable I2C to operate */
	Cy_SCB_I2C_Enable(tI2CMS.pBase, &tI2CMS.tContext);
}
/******************************************************************************
;       Function Name			:	uint8_t HAL_I2C_Slave_Init(CySCB_Type* pSCB,cy_stc_scb_i2c_config_t* ptsConfig,cy_stc_sysint_t* ptsIRQ)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
void HAL_I2C_MS_DeInit(i2c_master_slave_typedef tI2C)
{
	tI2CMS.pBase = tI2C.pBase;
	Cy_SCB_I2C_DeInit(tI2CMS.pBase);
}
/******************************************************************************
;       Function Name			:	uint8_t HAL_I2C_Slave_WriteBuf_Config (CySCB_Type const* pSCB,uint8_t* pu8Buf,uint32_t u32Size)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
uint8_t HAL_I2C_MS_TxBuf_Config (uint8_t* pu8Buf,uint32_t u32Size)
{
	uint8_t b8Result;
	if ((NULL != pu8Buf) && (u32Size >=2 ))
	{
		Cy_SCB_I2C_SlaveConfigReadBuf (tI2CMS.pBase,pu8Buf,u32Size, &tI2CMS.tContext);
		b8Result = DRIVER_TRUE;
	}
	else
	{
		b8Result = DRIVER_FALSE;
	}
	return b8Result;
}
/******************************************************************************
;       Function Name			:	uint8_t HAL_I2C_Slave_ReadBuf_Config  (CySCB_Type const* pSCB,uint8_t* pu8Buf,uint32_t u32Size)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
uint8_t HAL_I2C_MS_RxBuf_Config  (uint8_t* pu8Buf,uint32_t u32Size)
{
	uint8_t u8Result;
	if ((NULL != pu8Buf) && (u32Size >=2 ))
	{
		Cy_SCB_I2C_SlaveConfigWriteBuf (tI2CMS.pBase, pu8Buf, u32Size, &tI2CMS.tContext);
		u8Result = DRIVER_TRUE;
	}
	else
	{
		u8Result = DRIVER_FALSE;
	}
	return u8Result;
}
/******************************************************************************
;       Function Name			:	uint8_t HAL_I2C_Slave_Register_Event_Callback(CySCB_Type const* pSCB,cy_cb_scb_i2c_handle_events_t 	pCallback)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
uint8_t HAL_I2C_MS_Register_Event_Callback(cy_cb_scb_i2c_handle_events_t pCallback)
{
	uint8_t u8Result;
	if ((NULL != pCallback))
	{
		Cy_SCB_I2C_RegisterEventCallback (tI2CMS.pBase, pCallback, &tI2CMS.tContext);
		u8Result = DRIVER_TRUE;
	}
	else
	{
		u8Result = DRIVER_FALSE;
	}
	return u8Result;
}
/******************************************************************************
;       Function Name			:	uint32_t  HAL_2C_Slave_GetWriteTransferCount(CySCB_Type const* pSCB)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
uint32_t  HAL_I2C_MS_RxTransferCount_Get(void)
{
	return Cy_SCB_I2C_SlaveGetWriteTransferCount( tI2CMS.pBase, &tI2CMS.tContext);
}
/******************************************************************************
;       Function Name			:	uint32_t HAL_I2C_Slave_GetReadTransferCount (CySCB_Type const* pSCB)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
uint32_t HAL_I2C_MS_TxTransferCount_Get(void)
{
	return Cy_SCB_I2C_SlaveGetReadTransferCount( tI2CMS.pBase, &tI2CMS.tContext);
}
/******************************************************************************
;       Function Name			:	uint32_t HAL_2C_Slave_GetStatus (CySCB_Type const *pSCB)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
uint32_t HAL_I2C_MS_Status_Get(void)
{
	return Cy_SCB_I2C_SlaveGetStatus( tI2CMS.pBase, &tI2CMS.tContext);
}
/******************************************************************************
;       Function Name			:	uint32_t HAL_I2C_Slave_ClearReadStatus (CySCB_Type const *pSCB)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
uint32_t HAL_I2C_MS_TxStatus_Clear(void)
{
	return Cy_SCB_I2C_SlaveClearReadStatus( tI2CMS.pBase, &tI2CMS.tContext);
}
/******************************************************************************
;       Function Name			:	uint32_t HAL_I2C_Slave_ClearWriteStatus(CySCB_Type const *pSCB)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
uint32_t HAL_I2C_MS_RxStatus_Clear(void)
{
	return Cy_SCB_I2C_SlaveClearWriteStatus( tI2CMS.pBase, &tI2CMS.tContext);
}
/******************************************************************************
;       Function Name			:	uint32_t HAL_I2C_Slave_ClearWriteStatus(CySCB_Type const *pSCB)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
bool HAL_I2C_MS_Write(uint8_t u8Address,uint8_t* pData,uint16_t u16Size,uint32_t u32Timeout)
{
	bool bResult;

	cy_stc_scb_i2c_master_xfer_config_t conf;
	conf.slaveAddress = u8Address;
	conf.buffer = pData;
	conf.bufferSize = u16Size;
	conf.xferPending = false; // The transfer operation is pending - the stop condition will not be generated.
	                          /* IF xferPending = true , Do not generate Stop condition at the end of transaction */

	Cy_SCB_I2C_MasterWrite	(tI2CMS.pBase, &conf, &tI2CMS.tContext);

	/* Wait for transaction completion */
	for (uint32_t i = 0 ; i < u32Timeout ; i++)
	{
		if (0UL != (CY_SCB_I2C_MASTER_BUSY & Cy_SCB_I2C_MasterGetStatus(tI2CMS.pBase, &tI2CMS.tContext)))
		{
			Cy_SysLib_Delay(1);
			bResult = false;
		}
		else
		{
			bResult = true;
			break;
		}
	}
	return bResult;
}
/******************************************************************************
;       Function Name			:	uint32_t HAL_I2C_Slave_ClearWriteStatus(CySCB_Type const *pSCB)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
bool HAL_I2C_MS_Read(uint8_t u8Address,uint8_t* pReadBuffer ,uint16_t u16Size,uint32_t u32Timeout)
{
	bool bResult;
	cy_stc_scb_i2c_master_xfer_config_t conf;
	conf.slaveAddress = u8Address;
	conf.buffer = pReadBuffer;
	conf.bufferSize = u16Size;
	conf.xferPending = false; /* Generate Stop condition the end of transaction */

	Cy_SCB_I2C_MasterRead(tI2CMS.pBase, &conf, &tI2CMS.tContext);

	/* Wait for transaction completion */
	for (uint32_t i = 0; i < u32Timeout; i++)
	{
		if (0UL != (CY_SCB_I2C_MASTER_BUSY & Cy_SCB_I2C_MasterGetStatus(tI2CMS.pBase, &tI2CMS.tContext)))
		{
			Cy_SysLib_Delay(1);
			bResult = false;
		}
		else
		{
			bResult = true;
			break;
		}
	}
	return bResult;
}
