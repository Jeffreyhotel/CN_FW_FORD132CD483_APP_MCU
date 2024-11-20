#include "M_DisplayManage.h"
#include "M_GPIOSense.h"
#include "M_PowerManagement.h"
#include "Memory_Pool.h"

static bool bFlashReloadDisable = false; 
/******************************************************************************
 ;       Function Name			:	void Main_I2cMasterInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void M_DM_I2cMasterInit(void)
{
	i2c_master_typedef tI2CMaster;

	tI2CMaster.pBase = I2C_MASTER_HW;
	tI2CMaster.pConfig = &I2C_MASTER_config;

	(void) HAL_I2C_Master_Init(tI2CMaster);
}
/******************************************************************************
 ;       Function Name			:	void M_DM_BacklightControl( bool bEnable, bool bLockLoss)
 ;       Function Description	:	Backlight function control
 ;       Parameters				:	bool bEnable, bool bLockLoss
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
void M_DM_BacklightControl( bool bEnable, bool bLockLoss)
{
	if (bLockLoss == true)
	{
		HAL_GPIO_Low( U301_LED_EN_PORT, U301_LED_EN_PIN);
	}
	else
	{
		if (bEnable == true)
		{
			HAL_GPIO_High( U301_LED_EN_PORT, U301_LED_EN_PIN);
		}
		else
		{
			HAL_GPIO_Low( U301_LED_EN_PORT, U301_LED_EN_PIN);
		}
	}
}
/******************************************************************************
 ;       Function Name			:	void M_DM_TouchControl(uint8_t u8Command, bool bLockLoss, uint8_t u8LcdCurrentStatus)
 ;       Function Description	:	Touch function control
 ;       Parameters				:	uint8_t u8Command, bool bLockLoss, uint8_t u8LcdCurrentStatus
 ;       Return Values			:	uint8_t u8ReturnStatus
 ;		Source ID				:
 ******************************************************************************/
uint8_t M_DM_TouchControl(uint8_t u8CurrentStatus, uint8_t u8Command)
{
	uint8_t u8ReturnStatus;

	switch (u8Command)
	{
		case DISPLAY_OFF_TOUCH_OFF:
			HAL_GPIO_Low( U301_TSC_RESET_PORT, U301_TSC_RESET_PIN);
			u8ReturnStatus = TOUCH_OFF;
		break;

		case DISPLAY_ON_TOUCH_OFF:
			HAL_GPIO_Low( U301_TSC_RESET_PORT, U301_TSC_RESET_PIN);
			u8ReturnStatus = TOUCH_OFF;
		break;

		case DISPLAY_OFF_TOUCH_ON:	
			HAL_GPIO_High( U301_TSC_RESET_PORT, U301_TSC_RESET_PIN);
			u8ReturnStatus = TOUCH_ON;
		break;

		case DISPLAY_ON_TOUCH_ON:
			HAL_GPIO_High( U301_TSC_RESET_PORT, U301_TSC_RESET_PIN);
			u8ReturnStatus = TOUCH_ON;			
		break;

		default:
			u8ReturnStatus = u8CurrentStatus;
		break;               
	}
	(void) u8CurrentStatus;
	return u8ReturnStatus;
}
/******************************************************************************
 ;       Function Name			:	uint8_t M_DM_DisplayControl(uint8_t u8CurrentStatus, uint8_t u8Command, bool bLockLoss)
 ;       Function Description	:	Display control function
 ;       Parameters				:	uint8_t u8CurrentStatus, uint8_t u8Command, bool bLockLoss
 ;       Return Values			:	uint8_t u8ReturnStatus
 ;		Source ID				:
 ******************************************************************************/
uint8_t M_DM_DisplayControl(uint8_t u8CurrentStatus, uint8_t u8Command, bool bLockLoss)
{
    uint8_t u8ReturnStatus = 0U;
#if (CX430_TDDI_NT51926 || U717_TDDI_NT51926 || BX726_TDDI_NT51926)
	uint8_t u8SendData1[2] = {0x1EU,0x20U};
	uint8_t u8SendData2[2] = {0x01U,0x06U};

	if (bLockLoss == true)
	{
		if (u8CurrentStatus == DISPLAY_ON)
		{
			u8SendData2[1] = 0x06U;
			/*Switch Page to CMD2_P0.*/
			if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, u8SendData1, sizeof(u8SendData1), 10U) != DRIVER_TRUE)
			{
				Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
			}
			else
			{ /* Nothing */ }
			if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, u8SendData2, sizeof(u8SendData2), 10U) != DRIVER_TRUE)
			{
				Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
			}
			else
			{ /* Nothing */ }			
		}
		else
		{ /* Nothing */ }
		u8ReturnStatus = DISPLAY_OFF;
	}
	else
	{
		switch (u8Command)
		{
			case DISPLAY_OFF_TOUCH_OFF:
				u8ReturnStatus = DISPLAY_OFF;
				if (u8CurrentStatus != DISPLAY_OFF)
				{
					u8SendData2[1] = 0x06U;
					/*Switch Page to CMD2_P0.*/
					if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, u8SendData1, sizeof(u8SendData1), 10U) != DRIVER_TRUE)
					{
						Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
					}
					else
					{ /* Nothing */ }						
					if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, u8SendData2, sizeof(u8SendData2), 10U) != DRIVER_TRUE)
					{
						Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
					}
					else
					{ /* Nothing */ }								
				}
				else
				{ /* Nothing */ }
			break;

			case DISPLAY_ON_TOUCH_OFF:
				u8ReturnStatus = DISPLAY_ON;				
				if (u8CurrentStatus != DISPLAY_ON)
				{
					u8SendData2[1] = 0x07U;
					/*Switch Page to CMD2_P0.*/
					if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, u8SendData1, sizeof(u8SendData1), 10U) != DRIVER_TRUE)
					{
						Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
					}
					else
					{ /* Nothing */ }
					if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, u8SendData2, sizeof(u8SendData2), 10U) != DRIVER_TRUE)
					{
						Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
					}
					else
					{ /* Nothing */ }
				}
				else
				{ /* Nothing */ }
			break;

			case DISPLAY_OFF_TOUCH_ON:
				u8ReturnStatus = u8CurrentStatus;
			break;

			case DISPLAY_ON_TOUCH_ON:
				u8ReturnStatus = DISPLAY_ON;
				if (u8CurrentStatus != DISPLAY_ON)
				{
					u8SendData2[1] = 0x07U;
					/*Switch Page to CMD2_P0.*/
					if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, u8SendData1, sizeof(u8SendData1), 10U) != DRIVER_TRUE)
					{
						Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
					}
					else
					{ /* Nothing */ }
					if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, u8SendData2, sizeof(u8SendData2), 10U) != DRIVER_TRUE)
					{
						Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
					}
					else
					{ /* Nothing */ }
				}
				else
				{ /* Nothing */ }
			break;

			default:
				u8ReturnStatus = u8CurrentStatus;
			break;               
		}
	}
#endif
    return u8ReturnStatus;
}

/******************************************************************************
 ;       Function Name			:	uint8_t M_DM_LcdControl(uint8_t u8CurrentStatus, uint8_t u8Command, bool bLockLoss)
 ;       Function Description	:	Lcd  control function
 ;       Parameters				:	uint8_t u8CurrentStatus, uint8_t u8Command, bool bLockLoss
 ;       Return Values			:	uint8_t u8ReturnStatus
 ;		Source ID				:
 ******************************************************************************/
uint8_t M_DM_LcdControl(uint8_t u8CurrentStatus, uint8_t u8Command)
{
	uint8_t u8ReturnStatus = 0U;

	switch (u8Command)
	{
		case DISPLAY_OFF_TOUCH_OFF:
			HAL_GPIO_Low( U301_DISP_GLOBAL_RESET_PORT, U301_DISP_GLOBAL_RESET_PIN);
			bFlashReloadDisable = false;
			u8ReturnStatus = LCD_RESET_LOW;
		break;

		case DISPLAY_ON_TOUCH_OFF:
			HAL_GPIO_High( U301_DISP_GLOBAL_RESET_PORT, U301_DISP_GLOBAL_RESET_PIN);
			u8ReturnStatus = LCD_RESET_HIGH;
		break;

		case DISPLAY_OFF_TOUCH_ON:
			HAL_GPIO_Low( U301_DISP_GLOBAL_RESET_PORT, U301_DISP_GLOBAL_RESET_PIN);
			bFlashReloadDisable = false;
			u8ReturnStatus = LCD_RESET_LOW;
		break;

		case DISPLAY_ON_TOUCH_ON:
			HAL_GPIO_High( U301_DISP_GLOBAL_RESET_PORT, U301_DISP_GLOBAL_RESET_PIN);                
			u8ReturnStatus = LCD_RESET_HIGH;
		break;				

		default:
			u8ReturnStatus = u8CurrentStatus;
		break;				
	}
	(void) u8CurrentStatus;
	return u8ReturnStatus;
}

/******************************************************************************
 ;       Function Name			:	void M_TC_Control(uint8_t u8Status)
 ;       Function Description	:	This state for error condition
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
uint8_t M_DM_ScanningControl(uint8_t u8CurrentStatus, uint8_t u8Command)
{
	uint8_t mu8SendData[2];	
	uint8_t u8ReturnStatus;
#if (CX430_TDDI_NT51926 || U717_TDDI_NT51926 || BX726_TDDI_NT51926)
	uint8_t u8ScanSel = 0U;
#endif

	if (u8CurrentStatus != u8Command)
	{
#if (CX430_TDDI_NT51926 || U717_TDDI_NT51926 || BX726_TDDI_NT51926)
		bFlashReloadDisable = false;

		u8ScanSel = (u8Command & (BIT_HSD_MASK | BIT_VSD_MASK));

		switch (u8ScanSel)
		{
			case SCAN_VT_HL :  //0x00U
				/* Vertical scan Top to Bottom and Horizontal Left to Right */
				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x21U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x0AU;
				mu8SendData[1] = 0xA5U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x09U;
				mu8SendData[1] = 0xA5U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x20U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1DU;
				mu8SendData[1] = 0x03U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x22U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x04U;
				mu8SendData[1] = 0xDFU;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x05U;
				mu8SendData[1] = 0xE2U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x08U;
				mu8SendData[1] = 0xDFU;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x21U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x0AU;
				mu8SendData[1] = 0x00U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				Cy_SysLib_Delay(10U);

				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x10U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1DU;
				mu8SendData[1] = 0x03U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x1BU;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x05U;
				mu8SendData[1] = 0x30U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

			break;

			case SCAN_VT_HR :  //0x01U
				/* Vertical scan Top to Bottom and Horizontal Right to Left */
				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x21U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x0AU;
				mu8SendData[1] = 0xA5U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x09U;
				mu8SendData[1] = 0xA5U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x20U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1DU;
				mu8SendData[1] = 0x03U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x22U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x04U;
				mu8SendData[1] = 0x9FU;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x05U;
				mu8SendData[1] = 0x02U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x08U;
				mu8SendData[1] = 0x9FU;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				Cy_SysLib_Delay(10U);

				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x10U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1DU;
				mu8SendData[1] = 0x03U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x1BU;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x05U;
				mu8SendData[1] = 0x10U;  //SHLR=0,UD=1
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

			break;

			case SCAN_VB_HL :  //0x02U
                /* Vertical scan Bottom to Top and Horizontal Left to Right */
				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x21U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x0AU;
				mu8SendData[1] = 0xA5U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x09U;
				mu8SendData[1] = 0xA5U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x20U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1DU;
				mu8SendData[1] = 0x03U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x22U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x04U;
				mu8SendData[1] = 0xDFU;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x05U;
				mu8SendData[1] = 0xE2U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x08U;
				mu8SendData[1] = 0xDFU;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x21U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x0AU;
				mu8SendData[1] = 0x00U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				Cy_SysLib_Delay(10U);

				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x10U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1DU;
				mu8SendData[1] = 0x03U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x1BU;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x05U;
				mu8SendData[1] = 0x20U;  //SHLR=1,UD=0
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

			break;

			case SCAN_VB_HR :  //0x03U
				/* Vertical scan Bottom to Top and Horizontal Right to Left */
				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x21U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x0AU;
				mu8SendData[1] = 0xA5U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x09U;
				mu8SendData[1] = 0xA5U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x20U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1DU;
				mu8SendData[1] = 0x03U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x22U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x04U;
				mu8SendData[1] = 0x9FU;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x05U;
				mu8SendData[1] = 0x02U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x08U;
				mu8SendData[1] = 0x9FU;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				Cy_SysLib_Delay(10U);

				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x10U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1DU;
				mu8SendData[1] = 0x03U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x1EU;
				mu8SendData[1] = 0x1BU;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

				mu8SendData[0] = 0x05U;
				mu8SendData[1] = 0x00U;
				if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
				{
					bFlashReloadDisable = false;
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				}
				else
				{ /* Nothing */ }

			break;					
		}

		// if(bFlashReloadDisable == false)
		// {
		// 	HAL_UART_Printf("\r\nNT51926 stop reload, show:%02x!\r\n",u8Command);
		// 	bFlashReloadDisable = true;

		// 	mu8SendData[0] = 0x1EU;
		// 	mu8SendData[1] = 0x21U;
		// 	/*Switch Page to CMD2_P1.*/
		// 	if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
		// 	{
		// 		bFlashReloadDisable = false;
		// 		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
		// 	}
		// 	else
		// 	{ /* Nothing */ }

		// 	/* RELOAD_OFF_KEY .*/
		// 	mu8SendData[0] = 0x0AU;
		// 	mu8SendData[1] = 0xA5U;        
		// 	if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
		// 	{
		// 		bFlashReloadDisable = false;
		// 		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
		// 	}
		// 	else
		// 	{ /* Nothing */ }

		// 	mu8SendData[0] = 0x09U;
		// 	mu8SendData[1] = 0xA5U;        
		// 	if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
		// 	{
		// 		bFlashReloadDisable = false;
		// 		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
		// 	}
		// 	else
		// 	{ /* Nothing */ }

		// 	mu8SendData[0] = 0x1EU;
		// 	mu8SendData[1] = 0x10U;
		// 	/*Switch Page to CMD1_P0.*/
		// 	if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
		// 	{
		// 		bFlashReloadDisable = false;
		// 		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
		// 	}
		// 	else
		// 	{ /* Nothing */ }

		// 	mu8SendData[0] = 0x1DU;
		// 	mu8SendData[1] = 0x03U;
		// 	/* Do not reload.*/
		// 	if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
		// 	{
		// 		bFlashReloadDisable = false;
		// 		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
		// 	}
		// 	else
		// 	{ /* Nothing */ }
		// } 
		// else
		// { /* Nothing */ }

		u8ReturnStatus = u8Command;

		mu8SendData[0] = 0x1EU;
		mu8SendData[1] = 0x21U;
		/*Switch Page to CMD1_PB.*/
		if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
		{
			u8ReturnStatus = u8CurrentStatus;
			Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
		}
		else
		{ /* Nothing */ }

		mu8SendData[0] = 0x0AU;
		mu8SendData[1] = 0x00U;
		/*Switch Page to CMD1_PB.*/
		if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, sizeof(mu8SendData), 10U) != DRIVER_TRUE)
		{
			u8ReturnStatus = u8CurrentStatus;
			Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
		}
		else
		{ /* Nothing */ }

#endif        
	}
	else
	{
		u8ReturnStatus = u8CurrentStatus;
	}
	return u8ReturnStatus;
}
/******************************************************************************
 ;       Function Name			:	void M_TC_Control(uint8_t u8Status)
 ;       Function Description	:	This state for error condition
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
uint32_t M_DM_VCOM_Get(void)
{
	uint8_t u8PageSwitch[2]={0x1EU,0x10U};
	uint8_t u8DataAddress=0x02U;
	uint8_t u8ReadData[3];
	uint32_t u32ReadData;

	/* Switch Page to CMD1_P0.*/
	if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, u8PageSwitch, sizeof(u8PageSwitch), 10U) != DRIVER_TRUE)
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}
	else
	{ /* Nothing */ }
	/*Write data address.*/
	if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, &u8DataAddress, 1U, 10U) != DRIVER_TRUE)
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}
	else
	{ /* Nothing */ }	
	/*Read data from 02h.*/
	if(HAL_I2C_Master_Read(NT51926_SLAVE_ADDRESS, &u8ReadData[0], 1U, 10U) != DRIVER_TRUE)
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}
	else
	{ /* Nothing */ }	
	/*Write data address.*/
	u8DataAddress=0x03U;
	if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, &u8DataAddress, 1U, 10U) != DRIVER_TRUE)
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}
	else
	{ /* Nothing */ }	

	/*Read data from 03h.*/
	if(HAL_I2C_Master_Read(NT51926_SLAVE_ADDRESS, &u8ReadData[1], 1U, 10U) != DRIVER_TRUE)
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}	
	else
	{ /* Nothing */ }	
	/*Write data address.*/
	u8DataAddress=0x01U;
	if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, &u8DataAddress, 1U, 10U) != DRIVER_TRUE)
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}	
	else
	{ /* Nothing */ }

	/*Read data from 01h.*/
	if(HAL_I2C_Master_Read(NT51926_SLAVE_ADDRESS, &u8ReadData[2], 1U, 10U) != DRIVER_TRUE)
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}
	else
	{ /* Nothing */ }

	if((u8ReadData[1] & 0x02U) == 0x02U)
	{
		u8ReadData[2] |= 0x40U;
	}
	else
	{ /* Nothing */ }
	if((u8ReadData[1] & 0x10U) == 0x10U)
	{
		u8ReadData[2] |= 0x80U;
	}
	else
	{ /* Nothing */ }

	u8ReadData[1] &= 0x01U;	

	u32ReadData = ((((uint32_t)u8ReadData[2])<<16U) | (((uint32_t)u8ReadData[1])<<8U) | ((uint32_t)u8ReadData[0]));
	return u32ReadData;
}
/******************************************************************************
 ;       Function Name			:	void M_TC_Control(uint8_t u8Status)
 ;       Function Description	:	This state for error condition
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
void M_DM_VCOM_Set(uint8_t *pSetData)
{
#if (CX430_TDDI_NT51926 || U717_TDDI_NT51926 || BX726_TDDI_NT51926)
	uint8_t mu8SendData[4];

	mu8SendData[0] = 0x1EU;
	mu8SendData[1] = 0x10U;
	/*Switch Page to CMD1_P0.*/
	(void)HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, mu8SendData, (sizeof(mu8SendData)-1U), 10U);

	mu8SendData[2] = 0x03U;
	mu8SendData[3] = pSetData[1];	
	(void)HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, &mu8SendData[2], (sizeof(mu8SendData[2])+ 1U), 10U);

	mu8SendData[2] = 0x02U;
	mu8SendData[3] = pSetData[0];
	(void)HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, &mu8SendData[2], (sizeof(mu8SendData[2]) + 1U), 10U);
#endif
}
/******************************************************************************
 ;       Function Name			:	uint8_t M_DM_NT51926_Status_Get(void)
 ;       Function Description	:	This state for get  NT51926 display status
 ;       Parameters				:	void
 ;       Return Values			:	uint8_t u8ReadData
 ;		 Source ID				:
 ******************************************************************************/
uint8_t M_DM_NT51926_Status_Get(void)
{
	uint8_t u8PageSwitch[2]={0x1EU,0x2BU};
	uint8_t u8ReadData = 0xFFU;
	uint8_t u8DataAddress = 0x1FU;

	/*Switch Page to CMD2_PB.*/
	if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, u8PageSwitch, 2U, 10U) != DRIVER_TRUE)
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}
	else
	{ /* Nothing */ }

	/*Write data address.*/
	if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, &u8DataAddress, 1U, 10U) != DRIVER_TRUE)
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}
	else
	{ /* Nothing */ }

	/*Read data from 1Fh.*/
	if(HAL_I2C_Master_Read(NT51926_SLAVE_ADDRESS, &u8ReadData, 1U, 10U) != DRIVER_TRUE)
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}
	else
	{ /* Nothing */ }

	u8ReadData &= 0x07U;
	return u8ReadData;
}

/******************************************************************************
;       Function Name			:	void M_DM_INTB_Ctrl(uint8_t u8DigitalLevel)
;       Function Description	:
;       Parameters				:	uint8_t u8DigitalLevel
;       Return Values			:   void
;       Source ID				:
******************************************************************************/
void M_DM_INTB_Ctrl(uint8_t u8DigitalLevel)
{
	if(u8DigitalLevel == 0U)
	{
		HAL_GPIO_Low( U301_INTB_IN_PORT,  U301_INTB_IN_PIN);
	}
	else
	{
		if(Memory_Pool_PowerStatus_Get() == POWER_OFF_READY)
		{
			HAL_GPIO_Low( U301_INTB_IN_PORT,  U301_INTB_IN_PIN);
		}
		else
		{
			HAL_GPIO_High( U301_INTB_IN_PORT,  U301_INTB_IN_PIN);
		}
	}
}
/******************************************************************************
;       Function Name			:	uint8_t M_DM_NT51926_ATTN_Read(void)
;       Function Description	:
;       Parameters				:	void
;       Return Values			:
;       Source ID				:
******************************************************************************/
uint8_t M_DM_NT51926_ATTN_Read(void)
{
	uint8_t u8Level;
	HAL_GPIO_Read( U301_TSC_ATTN_PORT,  U301_TSC_ATTN_PIN, &u8Level);

	return u8Level;
}


//#if(BX726_TDDI_NT51926)
int8_t M_DM_NT51926_VGAMMA_Get(uint8_t *pu8Out)
{
	int8_t	i8RtCode = -1, i8I2CErr = 0;
	uint8_t u8PageSwitch[2U] = {0x1EU, CMD1_P0};
	uint8_t u8DataAddress = 0x04U;
	uint8_t u8ReadData[4U] = {0x00U};
	uint8_t u8ReadLoop = 0U, u8ReadLen = sizeof(u8ReadData);

	/* Switch Page to CMD1_P0.*/
	if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, u8PageSwitch, sizeof(u8PageSwitch), 10U) != DRIVER_TRUE)
	{
		/* fail situation */
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}
	else
	{
		/* Switch page success */


		for(u8ReadLoop = 0U ; u8ReadLoop < u8ReadLen ; u8ReadLoop++)
		{
			/*Write data address.*/
			if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, &u8DataAddress, 1U, 10U) != DRIVER_TRUE)
			{
				/* fail situation */
				Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
				i8I2CErr = -1;
				break;
			}
			else
			{
				if(HAL_I2C_Master_Read(NT51926_SLAVE_ADDRESS, pu8Out, 1U, 10U) != DRIVER_TRUE)
				{
					Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
					i8I2CErr = -1;
					break;
				}
				else
				{
					pu8Out++;
				}

				u8DataAddress++;
			}
		}

		if( 0 == i8I2CErr)
		{
			i8RtCode = 1;
		}
		else
		{
			; /* TODO: i2c damage, error handle */
		}

	}
	return i8RtCode;
}
//#endif
