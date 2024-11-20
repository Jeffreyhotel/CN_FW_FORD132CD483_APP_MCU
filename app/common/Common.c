/******************************************************************************
;       Program  : Crc8.c
;       Function : CRC8 Check Function
;       Chip     : Cypress CYT2B6
******************************************************************************/

/*---------------------------- Include File ---------------------------------*/
#include "Common.h"

/******************************************************************************
 ;       Function Name			:	void C_TD7800_Manage_Init(void)
 ;       Function Description	:	This state will do power management initialize
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
bool Common_Compare(uint32_t u32New,uint32_t u32Backup, uint32_t u32Mask)
{
	bool bReturn;

	if((u32New & u32Mask) == (u32Backup & u32Mask))
		bReturn = true;
	else
		bReturn = false;

	return bReturn;
}
/******************************************************************************
 ;       Function Name			:	void C_TD7800_Manage_Init(void)
 ;       Function Description	:	This state will do power management initialize
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
bool Common_LevelDebounce(uint8_t u8IOLevel, tgpio_debounce_def *ptDebounce)
{
	bool bReturn;

	if(u8IOLevel == GPIO_LOW)
	{
		ptDebounce->u8NewGPIOStatus = GPIO_LOW;
		ptDebounce->u8DebounceHigh = 0U;
		if(ptDebounce->u8DebounceLow < ptDebounce->u8DebounceMax)
		{
			ptDebounce->u8DebounceLow += 1U;
			bReturn = false;
		}
		else
		{
 			bReturn = true;
		}
	}
	else
	{
		ptDebounce->u8NewGPIOStatus = GPIO_HIGH;
		ptDebounce->u8DebounceLow = 0U;
		if(ptDebounce->u8DebounceHigh < ptDebounce->u8DebounceMax)
		{
			ptDebounce->u8DebounceHigh += 1U;
			bReturn = false;
		}
		else
		{
 			bReturn = true;
		}
	}

	return bReturn;
}
/******************************************************************************
 ;       Function Name			:	void C_TD7800_Manage_Init(void)
 ;       Function Description	:	This state will do power management initialize
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
uint8_t Common_Checksum_Calculation(uint8_t *pu8data,uint8_t u8Length)
{
	uint8_t u8ChecksumTemp= 0U;
	uint8_t u8Counter;

	for(u8Counter = 0; u8Counter < u8Length; u8Counter++)
	{
		u8ChecksumTemp = u8ChecksumTemp + *(pu8data + u8Counter);
	}
	return u8ChecksumTemp;
}
/******************************************************************************
 ;       Function Name			:	void C_TD7800_Manage_Init(void)
 ;       Function Description	:	This state will do power management initialize
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
int16_t Common_Maximum(int16_t i16Arg1, int16_t i16Arg2)
{	int16_t i16Result;
	if (i16Arg1 >= i16Arg2)
	{
		i16Result = i16Arg1;
	}
	else
	{
		i16Result = i16Arg2;
	}
	return i16Result;
}

