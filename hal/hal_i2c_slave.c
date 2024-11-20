/******************************************************************************
;				Program		:	hal_i2c_slave.c
;				Function	:	I2C Slave Function
;				Chip		:	Cypress CY8C4149AZI-S598
;				Date		:	2021 / 10 / 28
;				Author		:
;				Describe	:
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
#include "hal_i2c_slave.h"

static i2c_slave_typedef tI2CS;

/******************************************************************************
;       Function Name			:	void I2C_Slave_Isr(void)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
static void HAL_I2C_Slave_ISR(void)
{
	if (tI2CS.pBase != NULL)
	{
		Cy_SCB_I2C_Interrupt(tI2CS.pBase, &tI2CS.tContext);
	}
}
/******************************************************************************
;       Function Name			:	uint8_t HAL_I2C_Slave_Init(CySCB_Type* pSCB,cy_stc_scb_i2c_config_t* ptsConfig,cy_stc_sysint_t* ptsIRQ)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
void HAL_I2C_Slave_Init(i2c_slave_typedef tI2C)
{
	tI2CS.pBase = tI2C.pBase;
	tI2CS.pConfig = tI2C.pConfig;
	tI2CS.tContext = tI2C.tContext;
	tI2CS.tSysint.intrSrc = tI2C.tSysint.intrSrc;
	tI2CS.tSysint.intrPriority = tI2C.tSysint.intrPriority;

	Cy_SCB_I2C_Init(tI2CS.pBase, tI2CS.pConfig, &tI2CS.tContext);

	/* Hook interrupt service routine */
	Cy_SysInt_Init(&tI2CS.tSysint, HAL_I2C_Slave_ISR);
	NVIC_EnableIRQ((IRQn_Type) tI2CS.tSysint.intrSrc);

	/* Enable I2C to operate */
	Cy_SCB_I2C_Enable(tI2CS.pBase, &tI2CS.tContext);
}
/******************************************************************************
;       Function Name			:	uint8_t HAL_I2C_Slave_WriteBuf_Config (CySCB_Type const* pSCB,uint8_t* pu8Buf,uint32_t u32Size)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
uint8_t HAL_I2C_Slave_TxBuf_Config (uint8_t* pu8Buf,uint32_t u32Size)
{
	uint8_t u8Result;
	if ((NULL != pu8Buf) && (u32Size >=2 ))
	{
		Cy_SCB_I2C_SlaveConfigReadBuf (tI2CS.pBase,pu8Buf,u32Size, &tI2CS.tContext);
		u8Result = DRIVER_TRUE;
	}
	else
	{
		u8Result = DRIVER_FALSE;
	}
	return u8Result;
}
/******************************************************************************
;       Function Name			:	uint8_t HAL_I2C_Slave_ReadBuf_Config  (CySCB_Type const* pSCB,uint8_t* pu8Buf,uint32_t u32Size)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
uint8_t HAL_I2C_Slave_RxBuf_Config  (uint8_t* pu8Buf,uint32_t u32Size)
{
	uint8_t u8Result;
	if ((NULL != pu8Buf) && (u32Size >=2 ))
	{
		Cy_SCB_I2C_SlaveConfigWriteBuf (tI2CS.pBase, pu8Buf, u32Size, &tI2CS.tContext);
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
uint8_t HAL_I2C_Slave_Register_Event_Callback(cy_cb_scb_i2c_handle_events_t pCallback)
{
	uint8_t u8Result;
	if ((NULL != pCallback))
	{
		Cy_SCB_I2C_RegisterEventCallback (tI2CS.pBase, pCallback, &tI2CS.tContext);
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
uint32_t  HAL_I2C_Slave_RxTransferCount_Get(void)
{
	return Cy_SCB_I2C_SlaveGetWriteTransferCount( tI2CS.pBase, &tI2CS.tContext);
}
/******************************************************************************
;       Function Name			:	uint32_t HAL_I2C_Slave_GetReadTransferCount (CySCB_Type const* pSCB)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
uint32_t HAL_I2C_Slave_TxTransferCount_Get(void)
{
	return Cy_SCB_I2C_SlaveGetReadTransferCount( tI2CS.pBase, &tI2CS.tContext);
}
/******************************************************************************
;       Function Name			:	uint32_t HAL_2C_Slave_GetStatus (CySCB_Type const *pSCB)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
uint32_t HAL_I2C_Slave_Status_Get(void)
{
	return Cy_SCB_I2C_SlaveGetStatus( tI2CS.pBase, &tI2CS.tContext);
}
/******************************************************************************
;       Function Name			:	uint32_t HAL_I2C_Slave_ClearReadStatus (CySCB_Type const *pSCB)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
uint32_t HAL_I2C_Slave_TxStatus_Clear(void)
{
	return Cy_SCB_I2C_SlaveClearReadStatus( tI2CS.pBase, &tI2CS.tContext);
}
/******************************************************************************
;       Function Name			:	uint32_t HAL_I2C_Slave_ClearWriteStatus(CySCB_Type const *pSCB)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
uint32_t HAL_I2C_Slave_RxStatus_Clear(void)
{
	return Cy_SCB_I2C_SlaveClearWriteStatus( tI2CS.pBase, &tI2CS.tContext);
}
