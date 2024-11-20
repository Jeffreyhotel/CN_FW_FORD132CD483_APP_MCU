/******************************************************************************
;				Program		:	hal_i2c_master.c
;				Function	:	I2C Master Function
;				Chip		:	Cypress CY8C4149AZI-S598
;				Date		:	2021 / 10 / 28
;				Author		:
;				Describe	:
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
#include "hal_i2c_master.h"

static 	i2c_master_typedef tI2CM;

/******************************************************************************
;       Function Name			:	void HAL_I2C_Master_Init(i2c_master_typedef *pI2C)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
void HAL_I2C_Master_Init(i2c_master_typedef tI2C)
{
	tI2CM.pBase = tI2C.pBase;
	tI2CM.pConfig = tI2C.pConfig;

	Cy_SCB_I2C_Init(tI2CM.pBase, tI2CM.pConfig, &tI2CM.tContext);
	/* Enable I2C to operate */
	Cy_SCB_I2C_Enable(tI2CM.pBase,  &tI2CM.tContext);

}
/******************************************************************************
;       Function Name			:	uint8_t HAL_I2C_Master_Write(i2c_master_typedef *tI2C, uint8_t u8Address, uint8_t* pData, uint16_t u16Size, uint32_t u32Timeout)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
uint8_t HAL_I2C_Master_Write(uint8_t u8Address, uint8_t* pData, uint16_t u16Size, uint32_t u32Timeout)
{
	uint8_t u8Result;
	cy_en_scb_i2c_status_t tStatus;

	/* Send Start condition, address and receive ACK/NACK response from slave */
	tStatus = Cy_SCB_I2C_MasterSendStart(tI2CM.pBase, u8Address, CY_SCB_I2C_WRITE_XFER, u32Timeout, &tI2CM.tContext);

	if (CY_SCB_I2C_SUCCESS == tStatus)
	{
		for (uint16_t i = 0 ; i < u16Size ; i ++)
		{
			tStatus = Cy_SCB_I2C_MasterWriteByte(tI2CM.pBase, pData[i], u32Timeout, &tI2CM.tContext);
			  if (tStatus != CY_SCB_I2C_SUCCESS)
			  {
				  break;
			  }
			  else
			  {
				  continue;
			  }
		}
	}

	/* Check tStatus of transaction */
	if ((tStatus == CY_SCB_I2C_SUCCESS)           ||
	    (tStatus == CY_SCB_I2C_MASTER_MANUAL_NAK) ||
	    (tStatus == CY_SCB_I2C_MASTER_MANUAL_ADDR_NAK))
	{
	    /* Send Stop condition on the bus */
		tStatus = Cy_SCB_I2C_MasterSendStop(tI2CM.pBase, u32Timeout, &tI2CM.tContext);
	    if (tStatus == CY_SCB_I2C_SUCCESS)
	    {
	        /* Data has been written into the slave */
	    	u8Result = DRIVER_TRUE;
	    }
	    else
	    {
	    	u8Result = DRIVER_FALSE;
	    }
	}
	else
	{
		/* Other tStatuses do not require any actions */
		u8Result = DRIVER_FALSE;
	}
	return u8Result;
}
/******************************************************************************
;       Function Name			:	uint8_t HAL_I2C_Master_Read (i2c_master_typedef *tI2C, uint8_t u8Address, uint8_t* pData, uint16_t u16Size, uint32_t u32Timeout)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
uint8_t HAL_I2C_Master_Read (uint8_t u8Address, uint8_t* pData, uint16_t u16Size, uint32_t u32Timeout)
{
	uint8_t u8Result;
	cy_en_scb_i2c_status_t tStatus;

	/* Send Start condition, address and receive ACK/NACK response from slave */
	tStatus = Cy_SCB_I2C_MasterSendStart(tI2CM.pBase, u8Address, CY_SCB_I2C_READ_XFER, u32Timeout, &tI2CM.tContext);

	if (CY_SCB_I2C_SUCCESS == tStatus)
	{
	    uint32_t cnt = 0UL;
	    cy_en_scb_i2c_command_t cmd = CY_SCB_I2C_ACK;
	    /* Read data from the slave into the buffer */
	    do
	    {
	        if (cnt == (u16Size - 1UL))
	        {
	            /* The last byte must be NACKed */
	            cmd = CY_SCB_I2C_NAK;
	        }
	        /* Read byte and generate ACK / or prepare for NACK */
	        tStatus = Cy_SCB_I2C_MasterReadByte(tI2CM.pBase, cmd, &pData[cnt], u32Timeout, &tI2CM.tContext);
	        ++cnt;
	    }
	    while( (tStatus == CY_SCB_I2C_SUCCESS) && (cnt < u16Size) );
	}
	/* Check tStatus of transaction */
	if ((tStatus == CY_SCB_I2C_SUCCESS)           ||
	    (tStatus == CY_SCB_I2C_MASTER_MANUAL_NAK) ||
	    (tStatus == CY_SCB_I2C_MASTER_MANUAL_ADDR_NAK))
	{
	    /* Send Stop condition on the bus */
	    tStatus = Cy_SCB_I2C_MasterSendStop(tI2CM.pBase, u32Timeout, &tI2CM.tContext);
	    if (tStatus == CY_SCB_I2C_SUCCESS)
	    {
	        /* Process received data */
	    	u8Result = DRIVER_TRUE;
	    }
	    else
	    {
	    	u8Result = DRIVER_FALSE;
	    }
	}
	else
	{
	    /* Other tStatuses do not require any actions.
	    * The received data should dropped.
	    */
		u8Result = DRIVER_FALSE;
	}
	return u8Result;
}
