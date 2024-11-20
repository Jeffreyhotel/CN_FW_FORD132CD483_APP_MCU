#include "M_Communication.h"

#include "../../Mymain.h"
#include "ICDiagApp.h"
#include "M_TemperatureDerating.h"

static tcommunication_info_def tCommunicationInfo;

/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void M_COM_TxBuffer_Config(uint8_t *pTxBuff, uint8_t u8Length)
{
	HAL_I2C_MS_TxBuf_Config(pTxBuff, u8Length);
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void M_COM_RxBuffer_Config(uint8_t *pRxBuff, uint8_t u8Length)
{
	HAL_I2C_MS_RxBuf_Config(pRxBuff, u8Length);
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void M_COM_TxBufferData_Set(uint8_t *pTxBuff, uint8_t u8Length)
{
	uint8_t u8ResultValue = 0U;
	uint8_t u8LengthTemp = u8Length;

	if (*pTxBuff == 0xF4U)
	{
		u8ResultValue = Common_Checksum_Calculation(pTxBuff, u8LengthTemp) + 0x01U;
		*(pTxBuff + u8LengthTemp) = u8ResultValue;
	}
	else
	{
#if(FORD_SPSS_CRC_ROLL_EN)
		if(tCommunicationInfo.u8TxRollingCounter == 0xFFU)
		{
			tCommunicationInfo.u8TxRollingCounter = 0U;
		}
		else
		{
			tCommunicationInfo.u8TxRollingCounter++;
		}

		*(pTxBuff + u8LengthTemp) = tCommunicationInfo.u8TxRollingCounter;
		u8LengthTemp = u8Length + LEN_ROLLING_COUNTER;
		u8ResultValue = CRC8_Calculation(pTxBuff, u8LengthTemp);
		*(pTxBuff + u8LengthTemp) = u8ResultValue;
#endif
	}
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint32_t M_COM_RxLength_Get(void)
{
	return HAL_I2C_MS_RxTransferCount_Get();
}

/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void M_COM_I2cInit(uint8_t *pTxBuff, uint8_t u8TxSize, uint8_t *pRxBuff, uint8_t u8RxSize, cy_cb_scb_i2c_handle_events_t pCallback)
{
	i2c_master_slave_typedef tI2CMS;

	tCommunicationInfo.u8RxRollingCounter = 0U;
	tCommunicationInfo.u8TxRollingCounter = 0U;

	tI2CMS.pBase = I2C_COM_HW;
	tI2CMS.pConfig = &I2C_COM_config;
	tI2CMS.tSysint.intrSrc = I2C_COM_IRQ;
	tI2CMS.tSysint.intrPriority = 3U;
	tI2CMS.pCallback = pCallback;

	(void) HAL_I2C_MS_Init(tI2CMS, pTxBuff, u8TxSize, pRxBuff, u8RxSize);
}

/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
void M_COM_I2cDeInit(void)
{
	i2c_master_slave_typedef tI2CMS;

	tI2CMS.pBase = I2C_COM_HW;
	HAL_I2C_MS_DeInit(tI2CMS);
}

/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint8_t M_COM_RxMSGFormat_Check(uint8_t u8subaddress, uint8_t u8Length)
{
	uint8_t u8CheckResult=FORMAT_UNSUPPORT_SUBADDRESS;

	/* E4 and F4 without checking fusa. */
	switch(u8subaddress)
	{
		default:
			/* Continues next checking */
		break;

		case CMD_UPDATE_REQUEST:
			if(u8Length == (LEN_SUBADDRESS + LEN_LEN + LEN_UPDATE_REQUEST + LEN_CHECKSUM))
			{
				return FORMAT_WRITE_CHECK_SAFETY;
			}
			else
			{
				return FORMAT_LEN_FAIL;
			}
		break;

		case CMD_JUMP_TO_BOOTLOADER:
			if(u8Length == LEN_SUBADDRESS)
			{
				return FORMAT_READ_CHECK_SAFETY;
			}
			else
			{
				return FORMAT_LEN_FAIL;
			}
		break;
	}

#if(FORD_SPSS_CRC_ROLL_EN)
	u8Length=u8Length-(LEN_ROLLING_COUNTER+LEN_CRC8);
#endif	
	
    switch(u8subaddress)
    {
    	case CMD_DISPLAY_STATUS:
		case CMD_DISPLAY_IDENTIFICATION:
		case CMD_INTERRUPT_STATUS:
		case CMD_CORE_ASSEMBLY:
		case CMD_DELIVERY_ASSEMBLY:
		case CMD_SOFT_PART_NUMBER:
		case CMD_SERIAL_NUMBER:
		case CMD_MAIN_CALI_PART_NUMBER:
		case CMD_DETIAL_DIAGNOSIS_GET:
		case CMD_TEMPERATURE_GET:
		case CMD_VOLTAGE_GET:
		case CMD_PRESENT_BACKLIGHT_PWM_GET:
		case CMD_PRODUCTION_PHASE_BYTE_GET:
		case CMD_VCOM_VALUE_GET:
		case CMD_PCBATEMPINFO:
		case CMD_BLTEMPINFO:
		case CMD_BATTVOLINFO:
		case CMD_SYNCVOLINFO:
		case CMD_FPCTXOUTVOLINFO:
		case CMD_FPCRXOUTVOLINFO:
		case CMD_MCU_VERSION_GET:
#if (BACKDOOR_WRITE_DERATINGDATA)
		case CMD_DERATING_LIMITED_TEMPERATURES_GET:
		case CMD_DERATING_TABLE_GET:
			if(u8Length == LEN_SUBADDRESS)
			{
				u8CheckResult=FORMAT_READ_CHECK_SAFETY;
			}
			else
			{
				u8CheckResult=FORMAT_LEN_FAIL;
			}
		break;
		case CMD_DERATING_DATAS_SET:
			if(u8Length == LEN_SUBADDRESS + LEN_DERATING_SETTING_DATA)
			{
				u8CheckResult=FORMAT_WRITE_CHECK_SAFETY;
			}
			else
			{
				u8CheckResult=FORMAT_LEN_FAIL;
			}
		break;

#endif
		case CMD_BACKLIGHT_PWM:
			if(u8Length == LEN_SUBADDRESS)
			{
				u8CheckResult=FORMAT_READ_CHECK_SAFETY;
			}
			else if(u8Length == (LEN_SUBADDRESS+LEN_BACKLIGHT_PWM))
			{
				u8CheckResult=FORMAT_WRITE_CHECK_SAFETY;
			}
			else
			{
				u8CheckResult=FORMAT_LEN_FAIL;
			}
		break;

		case CMD_DISPLAY_SCANNING:
			if(u8Length == LEN_SUBADDRESS)
			{
				u8CheckResult=FORMAT_READ_CHECK_SAFETY;
			}
			else if(u8Length == (LEN_SUBADDRESS+LEN_DISPLAY_SCANNING))
			{
				u8CheckResult=FORMAT_WRITE_CHECK_SAFETY;
			}
			else
			{
				u8CheckResult=FORMAT_LEN_FAIL;
			}
		break;

		case CMD_DISPLAY_ENABLE:
			if(u8Length == LEN_SUBADDRESS)
			{
				u8CheckResult=FORMAT_READ_CHECK_SAFETY;
			}
			else if(u8Length == (LEN_SUBADDRESS+LEN_DISPLAY_ENABLE))
			{
				u8CheckResult=FORMAT_WRITE_CHECK_SAFETY;
			}
			else
			{
				u8CheckResult=FORMAT_LEN_FAIL;
			}
		break;

		case CMD_DISPLAY_SHUTDOWN:
			if(u8Length == LEN_SUBADDRESS)
			{
				u8CheckResult=FORMAT_READ_CHECK_SAFETY;
			}
			else if(u8Length == (LEN_SUBADDRESS+LEN_DISPLAY_SHUTDOWN))
			{
				u8CheckResult=FORMAT_WRITE_CHECK_SAFETY;
			}
			else
			{
				u8CheckResult=FORMAT_LEN_FAIL;
			}
		break;

		case CMD_FACTORY_MODE:
			if(u8Length == LEN_SUBADDRESS)
			{
				u8CheckResult=FORMAT_READ_CHECK_SAFETY;
			}
			else if(u8Length == (LEN_SUBADDRESS+LEN_FACTORY_MODE))
			{
				u8CheckResult=FORMAT_WRITE_CHECK_SAFETY;
			}
			else
			{
				u8CheckResult=FORMAT_LEN_FAIL;
			}
		break;

		case CMD_DERATING_ENABLE:
			if(u8Length == LEN_SUBADDRESS)
			{
				u8CheckResult=FORMAT_READ_CHECK_SAFETY;
			}
			else if(u8Length == (LEN_SUBADDRESS+LEN_DERATING_ENABLE))
			{
				u8CheckResult=FORMAT_WRITE_CHECK_SAFETY;
			}
			else
			{
				u8CheckResult=FORMAT_LEN_FAIL;
			}
		break;

		case CMD_LOCK_DELIVERY_ASSEMBLY:
			if(u8Length == LEN_SUBADDRESS)
			{
				u8CheckResult=FORMAT_READ_CHECK_SAFETY;
			}
			else if(u8Length == (LEN_SUBADDRESS+LEN_LOCK_DELIVERY_ASSEMBLY))
			{
				u8CheckResult=FORMAT_WRITE_CHECK_SAFETY;
			}
			else
			{
				u8CheckResult=FORMAT_LEN_FAIL;
			}
		break;
		
		case CMD_DELIVERY_ASSEMBLY_DATA:
			if(u8Length == (LEN_SUBADDRESS+LEN_DELIVERY_ASSEMBLY_WRITE))
			{
				u8CheckResult=FORMAT_WRITE_CHECK_SAFETY;
			}
			else
			{
				u8CheckResult=FORMAT_LEN_FAIL;
			}
		break;
			
		case CMD_LOCK_SERIAL_NUMBER:
			if(u8Length == LEN_SUBADDRESS)
			{
				u8CheckResult=FORMAT_READ_CHECK_SAFETY;
			}
			else if(u8Length == (LEN_SUBADDRESS+LEN_LOCK_SERIAL_NUMBER))
			{
				u8CheckResult=FORMAT_WRITE_CHECK_SAFETY;
			}
			else
			{
				u8CheckResult=FORMAT_LEN_FAIL;
			}
		break;
		
		case CMD_SERIAL_NUMBER_DATA:
			if(u8Length == (LEN_SUBADDRESS+LEN_SERIAL_NUMBER_WRITE))
			{
				u8CheckResult=FORMAT_WRITE_CHECK_SAFETY;
			}
			else
			{
				u8CheckResult=FORMAT_LEN_FAIL;
			}
		break;

		case CMD_LOCK_PRODUCTION_PHASE_BYTE:
			if(u8Length == LEN_SUBADDRESS)
			{
				u8CheckResult=FORMAT_READ_CHECK_SAFETY;
			}
			else if(u8Length == (LEN_SUBADDRESS+LEN_PRODUCTION_PHASE_BYTE))
			{
				u8CheckResult=FORMAT_WRITE_CHECK_SAFETY;
			}
			else
			{
				u8CheckResult=FORMAT_LEN_FAIL;
			}
		break;

		case CMD_PRODUCTION_PHASE_BYTE_DATA:
			if(u8Length == (LEN_SUBADDRESS+LEN_PRODUCTION_PHASE_BYTE))
			{
				u8CheckResult=FORMAT_WRITE_CHECK_SAFETY;
			}
			else
			{
				u8CheckResult=FORMAT_LEN_FAIL;
			}
		break;

#if(BACKDOOR_ICDIAG_OPEN)
		case ICDIAG_CMD_ICFETCH:
			if(u8Length == (LEN_SUBADDRESS+LEN_ICDIAG_FETCH))
			{
				u8CheckResult=FORMAT_WRITE_CHECK_SAFETY;
			}
			else
			{
				u8CheckResult=FORMAT_LEN_FAIL;
			}
		break;

		case ICDIAG_CMD_READ:
			if(u8Length == LEN_SUBADDRESS)
			{
				u8CheckResult=FORMAT_READ_CHECK_SAFETY;
			}
			else
			{
				u8CheckResult=FORMAT_LEN_FAIL;
			}
		break;			

		case ICDIAG_CMD_ICCTRL:
			if(u8Length == (LEN_SUBADDRESS+LEN_ICDIAG_ICCTRL))
			{
				u8CheckResult=FORMAT_WRITE_CHECK_SAFETY;
			}
			else if((u8Length >=(LEN_SUBADDRESS + 9U)) || (u8Length<(LEN_SUBADDRESS+LEN_ICDIAG_ICCTRL)))
			{
				u8CheckResult=FORMAT_WRITE_CHECK_SAFETY;
			}
			else
			{
				u8CheckResult=FORMAT_LEN_FAIL;
			}
		break;
#endif

#if(BACKDOOR_DIAGNOSIS_SIMULATE)
		case CMD_DIAGNOSIS_SIMULATE:
			if(u8Length == (LEN_SUBADDRESS+LEN_DIAGNOSIS_SIMULATE))
			{
				u8CheckResult=FORMAT_WRITE_CHECK_SAFETY;
			}
			else
			{
				u8CheckResult=FORMAT_LEN_FAIL;
			}
		break;
#endif
		default:
			u8CheckResult=FORMAT_UNSUPPORT_SUBADDRESS;
		break;
    }
	return u8CheckResult;
}

/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
bool M_COM_RxMSGCRC8Value_Check(uint8_t *pRxBuff, uint8_t u8Length)
{
	bool bCheckResult = false;

#if(FORD_SPSS_CRC_ROLL_EN)
	uint8_t u8CRCinMSG = *(pRxBuff + u8Length - LEN_CRC8);

	u8Length = u8Length-LEN_CRC8;

	if(CRC8_Calculation(pRxBuff, u8Length) == u8CRCinMSG)
	{
		bCheckResult=true;
	}
	else
	{
		bCheckResult=false;
	}
#else
	bCheckResult=true;
#endif

	return bCheckResult;
}

/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
bool M_COM_RxRollingCounter_Check(uint8_t u8HostRollingCounter)
{
	bool bCheckResult=false;
	
#if(FORD_SPSS_CRC_ROLL_EN)
	if (u8HostRollingCounter != tCommunicationInfo.u8RxRollingCounter)
	{
		bCheckResult=true;
	}
	else
	{
		bCheckResult=false;
	}
	
	tCommunicationInfo.u8RxRollingCounter = u8HostRollingCounter;
#else
	bCheckResult=true;
#endif

	return bCheckResult;
}

