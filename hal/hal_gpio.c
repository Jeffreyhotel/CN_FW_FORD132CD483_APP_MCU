/******************************************************************************
;				Program		:	hal_gpio.c
;				Function	:	GPIO Sensing Function
;				Chip		:	Cypress CY8C4149AZI-S598
;				Date		:	2021 / 10 / 28
;				Author		:
;				Describe	:	 
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
#include "hal_gpio.h"

/******************************************************************************
;       Function Name			:	void HAL_GPIO_Toggle(GPIO_PRT_Type *u8Port, uint8_t u8Pin)
;       Function Description	:	Pin level is inversed.
;       Parameters				:	[*u8Port] - Input GPIO port.
;									[u8Pin] - Input GPIO pin.
;       Return Values			:	void
;		Source ID				:	U625_PINIO-003
******************************************************************************/
void HAL_GPIO_Toggle(GPIO_PRT_Type *u8Port, uint8_t u8Pin)
{	
	if(u8Port != NULL && u8Pin < PIN_NUM_MAX)
	{
		Cy_GPIO_Inv(u8Port, u8Pin);
	}
}

/******************************************************************************
;       Function Name			:	void HAL_GPIO_High(GPIO_PRT_Type *u8Port, uint8_t u8Pin)
;       Function Description	:	Set pin is level high.
;       Parameters				:	[*u8Port] - Input GPIO port.
;									[u8Pin] - Input GPIO pin.
;       Return Values			:	void
;		Source ID				:	U625_PINIO-004
******************************************************************************/
void HAL_GPIO_High(GPIO_PRT_Type *u8Port, uint8_t u8Pin)
{
	if(u8Port != NULL && u8Pin < PIN_NUM_MAX)
	{
		Cy_GPIO_Set(u8Port, u8Pin);
	}
}

/******************************************************************************
;       Function Name			:	void HAL_GPIO_Low(GPIO_PRT_Type *u8Port, uint8_t u8Pin)
;       Function Description	:	Set pin is level low.
;       Parameters				:	[*u8Port] - Input GPIO port.
;									[u8Pin] - Input GPIO pin.
;       Return Values			:	void
;		Source ID				:	U625_PINIO-005
******************************************************************************/
void HAL_GPIO_Low(GPIO_PRT_Type *u8Port, uint8_t u8Pin)
{
	if(u8Port != NULL && u8Pin < PIN_NUM_MAX)
	{
		Cy_GPIO_Clr(u8Port, u8Pin);
	}
}

/******************************************************************************
;       Function Name			:	uint8_t HAL_GPIO_Read(GPIO_PRT_Type *u8Port, uint8_t u8Pin)
;       Function Description	:	Read pin status.
;       Parameters				:	[u8Port] - Input GPIO port.
;									[u8Pin] - Input GPIO pin.
;       Return Values			:	Return pin status.
;									0x01 = Level high
;									0x00 = Level low
;		Source ID				:	U625_PINIO-006
******************************************************************************/
void HAL_GPIO_Read(GPIO_PRT_Type *u8Port, uint8_t u8Pin, uint8_t *pReturn)
{
	if(u8Port != NULL && u8Pin < PIN_NUM_MAX)
	{
		*pReturn = Cy_GPIO_Read(u8Port, u8Pin);
	}
}

/*---------------------------------------------------------------------------*/ 


