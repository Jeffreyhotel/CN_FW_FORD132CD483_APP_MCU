#include "M_GPIOSense.h"
#include "M_DisplayManage.h"
#include "Memory_Pool.h"

/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
bool M_GPIOSense_LevelDeboucne(GPIO_PRT_Type *pPort, uint8_t u8Pin, tgpio_debounce_def *ptDebounce)
{
	uint8_t u8IOLevel=0U;

	HAL_GPIO_Read(pPort, u8Pin, &u8IOLevel);
#if(BACKDOOR_DIAGNOSIS_SIMULATE)
	if((pPort == U301_LED_INT_PORT) && (u8Pin == U301_LED_INT_PIN))
	{
		u8IOLevel = Memory_Pool_DiagnosisSimulateInfo_Get().u8LedINTPinLevel;
	}
	else if((pPort == U301_DISP_FAULT_PORT) && (u8Pin == U301_DISP_FAULT_PIN))
	{
		u8IOLevel = Memory_Pool_DiagnosisSimulateInfo_Get().u8DispFaultPinLevel;
	}
	else if((pPort == U301_LOCK_PORT) && (u8Pin == U301_LOCK_PIN))
	{
		u8IOLevel = Memory_Pool_DiagnosisSimulateInfo_Get().u8LockPinLevel;
	}
	else if((pPort == U301_P1V2_PGOOD_PORT) && (u8Pin == U301_P1V2_PGOOD_PIN))
	{
		u8IOLevel = Memory_Pool_DiagnosisSimulateInfo_Get().u8PG1V2PinLevel;
	}
	else if((pPort == U301_P3V3_PGOOD_PORT) && (u8Pin == U301_P3V3_PGOOD_PIN))
	{
		u8IOLevel = Memory_Pool_DiagnosisSimulateInfo_Get().u8PG3V3PinLevel;
	}
	else
	{/* Nothing */}
#endif

	return Common_LevelDebounce(u8IOLevel, ptDebounce);
}
/******************************************************************************
 ;       Function Name			:	void Main_I2cSlaveInit(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint64_t M_GPIOSense_LED_Driver_Diagnosis(void)
{
	uint8_t u8SendData = 0x0EU;
	uint8_t mu8Temp[6U] = {0U};
	uint64_t u64ReturnStatus=0UL;

	/* Get the supply status of LED driver*/
	(void)HAL_I2C_Master_Write(LP8864_ADDRESS, &u8SendData, sizeof(u8SendData), 10U);
	(void)HAL_I2C_Master_Read(LP8864_ADDRESS, &mu8Temp[0U], 6U, 10U);

#if (BACKDOOR_DIAGNOSIS_SIMULATE)
	mu8Temp[0U] = Memory_Pool_DiagnosisSimulateInfo_Get().u8LedFaultStatus[0U];
	mu8Temp[1U] = Memory_Pool_DiagnosisSimulateInfo_Get().u8LedFaultStatus[1U];
	mu8Temp[2U] = Memory_Pool_DiagnosisSimulateInfo_Get().u8LedFaultStatus[2U];
	mu8Temp[3U] = Memory_Pool_DiagnosisSimulateInfo_Get().u8LedFaultStatus[3U];
	mu8Temp[4U] = Memory_Pool_DiagnosisSimulateInfo_Get().u8LedFaultStatus[4U];
	mu8Temp[5U] = Memory_Pool_DiagnosisSimulateInfo_Get().u8LedFaultStatus[5U];
#endif

	u64ReturnStatus = (uint64_t)(mu8Temp[0U] & 0xAAU);
	u64ReturnStatus |= (((uint64_t)(mu8Temp[1U] & 0xAAU)) << 8U);

	/* Get the supply status of LED driver*/
	u64ReturnStatus |= (((uint64_t)(mu8Temp[2U] & 0xAAU)) << 16U);
	u64ReturnStatus |= (((uint64_t)(mu8Temp[3U] & 0xAAU)) << 24U);

	/* Get the supply status of LED driver*/
	u64ReturnStatus |= (((uint64_t)(mu8Temp[4U] & 0xCFU)) << 32U);
	u64ReturnStatus |= (((uint64_t)(mu8Temp[5U] & 0x55U)) << 40U);

	return u64ReturnStatus;
}

bool M_GPIOSense_LED_Driver_Clear(void)
{
	uint8_t mu8Temp[3U] = {0x10,0x03,0x00};
	(void)HAL_I2C_Master_Write(LP8864_ADDRESS, &mu8Temp[0U], 3U, 10U);

	return true;
}
/******************************************************************************
 ;       Function Name			:	uint64_t M_GPIOSense_DisplayFault_Read(void)
 ;       Function Description	:
 ;       Parameters				:	void
 ;       Return Values			:
 ;       Source ID				:
 ******************************************************************************/
uint64_t M_GPIOSense_DisplayFault_Read(void)
{
	uint64_t u64ReturnStatus=0UL;
	/*Switch Page to CMD2_P2.*/
	uint8_t u8PageSwitch[2]={0x1EU,0x2BU};
	uint8_t u8DataAddress=0x00U;
	uint8_t u8ReadData[8];
	uint8_t u8ReadDataSencond[8];

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

	/*Read data from 00h.*/
	/*Primary IC & Secondary IC*/
	if((HAL_I2C_Master_Read(NT51926_SLAVE_ADDRESS, &u8ReadData[0], 1U, 10U) != DRIVER_TRUE) 
	|| (HAL_I2C_Master_Read(NT51926_SLAVE_SECOND_ADDRESS, &u8ReadDataSencond[0], 1U, 10U) != DRIVER_TRUE))
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
	/*Primary IC & Secondary IC*/
	if((HAL_I2C_Master_Read(NT51926_SLAVE_ADDRESS, &u8ReadData[1], 1U, 10U) != DRIVER_TRUE) 
	|| (HAL_I2C_Master_Read(NT51926_SLAVE_SECOND_ADDRESS, &u8ReadDataSencond[1], 1U, 10U) != DRIVER_TRUE))
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
	/*Primary IC & Secondary IC*/
	if((HAL_I2C_Master_Read(NT51926_SLAVE_ADDRESS, &u8ReadData[2], 1U, 10U) != DRIVER_TRUE) 
	|| (HAL_I2C_Master_Read(NT51926_SLAVE_SECOND_ADDRESS, &u8ReadDataSencond[2], 1U, 10U) != DRIVER_TRUE))
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}
	else
	{ /* Nothing */ }

	/*Write data address.*/
	u8DataAddress=0x04U;
	if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, &u8DataAddress, 1U, 10U) != DRIVER_TRUE)
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}
	else
	{ /* Nothing */ }

	/*Read data from 04h.*/
	/*Primary IC & Secondary IC*/
	if((HAL_I2C_Master_Read(NT51926_SLAVE_ADDRESS, &u8ReadData[3], 1U, 10U) != DRIVER_TRUE) 
	|| (HAL_I2C_Master_Read(NT51926_SLAVE_SECOND_ADDRESS, &u8ReadDataSencond[3], 1U, 10U) != DRIVER_TRUE))
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}
	else
	{ /* Nothing */ }

	/*Write data address.*/
	u8DataAddress=0x07U;
	if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, &u8DataAddress, 1U, 10U) != DRIVER_TRUE)
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}
	else
	{ /* Nothing */ }

	/*Read data from 07h.*/
	/*Primary IC & Secondary IC*/
	if((HAL_I2C_Master_Read(NT51926_SLAVE_ADDRESS, &u8ReadData[4], 1U, 10U) != DRIVER_TRUE) 
	|| (HAL_I2C_Master_Read(NT51926_SLAVE_SECOND_ADDRESS, &u8ReadDataSencond[4], 1U, 10U) != DRIVER_TRUE))
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}
	else
	{ /* Nothing */ }

  	/*Write data address.*/
    u8DataAddress=0x0AU;
	if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, &u8DataAddress, 1U, 10U) != DRIVER_TRUE)
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}
	else
	{ /* Nothing */ }

	/*Read data from 0Ah.*/
	/*Primary IC & Secondary IC*/
	if((HAL_I2C_Master_Read(NT51926_SLAVE_ADDRESS, &u8ReadData[5], 1U, 10U) != DRIVER_TRUE) 
	|| (HAL_I2C_Master_Read(NT51926_SLAVE_SECOND_ADDRESS, &u8ReadDataSencond[5], 1U, 10U) != DRIVER_TRUE))
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}
	else
	{ /* Nothing */ }

	/*Write data address.*/
    u8DataAddress=0x1FU;
	if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, &u8DataAddress, 1U, 10U) != DRIVER_TRUE)
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}
	else
	{ /* Nothing */ }

	/*Read data from 1Fh.*/
	/*Primary IC & Secondary IC*/
	if((HAL_I2C_Master_Read(NT51926_SLAVE_ADDRESS, &u8ReadData[6], 1U, 10U) != DRIVER_TRUE) 
	|| (HAL_I2C_Master_Read(NT51926_SLAVE_SECOND_ADDRESS, &u8ReadDataSencond[6], 1U, 10U) != DRIVER_TRUE))
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
		u8ReadData[6] = 0xFFU;
	}
	else
	{ /* Nothing */ }

	/* Touch error status */	
	/*Switch Page to CMD4_P1.*/
    u8PageSwitch[1]=0x41U;
	if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, u8PageSwitch, 2U, 10U) != DRIVER_TRUE)
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}
	else
	{ /* Nothing */ }

	/*Write data address.*/
	u8DataAddress=0x1CU;
    if(HAL_I2C_Master_Write(NT51926_SLAVE_ADDRESS, &u8DataAddress, 1U, 10U) != DRIVER_TRUE)
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}
	else
	{ /* Nothing */ }

	/*Read data from 1Ch.*/
	/*Primary IC & Secondary IC*/
	if((HAL_I2C_Master_Read(NT51926_SLAVE_ADDRESS, &u8ReadData[7], 1U, 10U) != DRIVER_TRUE) 
	|| (HAL_I2C_Master_Read(NT51926_SLAVE_SECOND_ADDRESS, &u8ReadDataSencond[7], 1U, 10U) != DRIVER_TRUE))
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
	}
	else
	{ /* Nothing */ }

#if (BACKDOOR_DIAGNOSIS_SIMULATE)
	u8ReadData[0U] = Memory_Pool_DiagnosisSimulateInfo_Get().u8DispFaultStatus[0U];
	u8ReadData[1U] = Memory_Pool_DiagnosisSimulateInfo_Get().u8DispFaultStatus[1U];
	u8ReadData[2U] = Memory_Pool_DiagnosisSimulateInfo_Get().u8DispFaultStatus[2U];
	u8ReadData[3U] = Memory_Pool_DiagnosisSimulateInfo_Get().u8DispFaultStatus[3U];
	u8ReadData[4U] = Memory_Pool_DiagnosisSimulateInfo_Get().u8DispFaultStatus[4U];
	u8ReadData[5U] = Memory_Pool_DiagnosisSimulateInfo_Get().u8DispFaultStatus[5U];
	u8ReadData[6U] = Memory_Pool_DiagnosisSimulateInfo_Get().u8DispFaultStatus[6U];
	u8ReadData[7U] = Memory_Pool_DiagnosisSimulateInfo_Get().u8DispFaultStatus[7U];
#endif
	
	u64ReturnStatus = (uint64_t)((u8ReadData[0U] | u8ReadDataSencond[0U]) & 0x7DU);
	u64ReturnStatus |= (((uint64_t)((u8ReadData[1U] | u8ReadDataSencond[1U]) & 0x03U)) << 8U);
	u64ReturnStatus |= (((uint64_t)((u8ReadData[2U] | u8ReadDataSencond[2U]) & 0x01U)) << 16U);
	u64ReturnStatus |= (((uint64_t)((u8ReadData[3U] | u8ReadDataSencond[3U]) & 0x01U)) << 24U);
	u64ReturnStatus |= (((uint64_t)((u8ReadData[4U] | u8ReadDataSencond[4U]) & 0x7FU)) << 32U);
	u64ReturnStatus |= (((uint64_t)((u8ReadData[5U] | u8ReadDataSencond[5U]) & 0x02U)) << 40U);
	u64ReturnStatus |= (((uint64_t)((u8ReadData[6U] | u8ReadDataSencond[6U]) & 0x07U)) << 48U);
	u64ReturnStatus |= (((uint64_t)((u8ReadData[7U] | u8ReadDataSencond[7U]) & 0x07U)) << 56U);

	return u64ReturnStatus;
}
/******************************************************************************
 ;       Function Name			:	uint8_t M_GPIOSense_NT51926_Status_Get(void)
 ;       Function Description	:	This state for get  NT51926 display status
 ;       Parameters				:	void
 ;       Return Values			:	uint8_t u8ReadData
 ;		 Source ID				:
 ******************************************************************************/
uint8_t M_GPIOSense_NT51926_Status_Get(void)
{
	uint8_t u8PageSwitch[2]={0x1EU,0x2BU};
	uint8_t u8ReadData = 0xFFU;
	uint8_t u8ReadDataSencond = 0xFFU;
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
	if((HAL_I2C_Master_Read(NT51926_SLAVE_ADDRESS, &u8ReadData, 1U, 10U) != DRIVER_TRUE)
	|| (HAL_I2C_Master_Read(NT51926_SLAVE_SECOND_ADDRESS, &u8ReadDataSencond, 1U, 10U) != DRIVER_TRUE))
	{
		Memory_Pool_IcCommDiagnosis_Set(Memory_Pool_IcCommDiagnosis_Get() + 1U);
		u8ReadData = 0xFFU; /* If I2C master bus read error， Set u8Temp = 0x07U */ 
		u8ReadDataSencond = 0xFF;
	}
	else
	{ /* Nothing */ }

#if (BACKDOOR_DIAGNOSIS_SIMULATE)
	u8ReadData =  Memory_Pool_DiagnosisSimulateInfo_Get().u8DispFaultStatus[6U];
#endif
	u8ReadData &= 0x07U;
	u8ReadDataSencond &= 0x07U;
	
	return (u8ReadData | u8ReadDataSencond);
}


