/*
 * EventGroup.c
 *
 *  Created on: 2022年9月7日
 *      Author: JackyHWWu
 */
#include "hal_uart.h"
#include "FIDM_Config.h"
#include "Event_Group.h"


static tsEvenGroup gtsEventGroup[EVENTGROUP_MAX] =
{
//	{
//	  .tuType = EventDisplayManagement,
//	},
//	{
//	  .tuType = EventBacklightManagement,
//	},
	{
	   .tuType = aEVENT_INTB,
	}
};


int32_t EventGroup_Create(void)
{
	uint8_t u8Loop = 0U;
	int32_t	i32Return = true;

	for( u8Loop = 0U ; u8Loop < EVENTGROUP_MAX ; u8Loop++)
	{
		gtsEventGroup[u8Loop].u32Event = 0UL;
	}

	HAL_UART_Printf("EventGroup_Create success\r\n");
	return i32Return;
}
int32_t Event_GroupSetBits(tuEventGroupHandle tuEventHandle, const uint32_t cu32BitsToSet)
{
	uint8_t u8Loop = 0U;
	int32_t	i32Return = -1;

	if( EventGroupEnd > tuEventHandle )
	{
		/* Valid input handle */
		for( u8Loop = 0U ; u8Loop < EVENTGROUP_MAX ; u8Loop++)
		{
			if( tuEventHandle == gtsEventGroup[u8Loop].tuType )
			{
				gtsEventGroup[u8Loop].u32Event |= cu32BitsToSet;
				i32Return = true;
				break;
			}
			else
			{
				;/* not matched */
			}
		}
	}
	else
	{
		/* event group out of range do nothing */
		HAL_UART_Printf("not support event group:%d\r\n",tuEventHandle);
	}
	return i32Return;
}
int32_t Event_GroupClearBits(tuEventGroupHandle tuEventHandle, const uint32_t cu32BitsToClear)
{
	uint8_t u8Loop = 0U;
	int32_t	i32Return = -1;

	if( EventGroupEnd > tuEventHandle )
	{
		/* Valid input handle */
		for( u8Loop = 0U ; u8Loop < EVENTGROUP_MAX ; u8Loop++)
		{
			if( tuEventHandle == gtsEventGroup[u8Loop].tuType )
			{
				gtsEventGroup[u8Loop].u32Event &= ~cu32BitsToClear;
				i32Return = true;
				break;
			}
			else
			{
				;/* not matched */
			}
		}
	}
	else
	{
		/* event group out of range do nothing */
		HAL_UART_Printf("not support event group:%d\r\n",tuEventHandle);
	}

	return i32Return;
}
uint32_t Event_GroupGetBits(tuEventGroupHandle tuEventHandle)
{
	uint8_t u8Loop = 0U;
	uint32_t u32BitsToGet = 0U;

	if( EventGroupEnd > tuEventHandle )
	{
		/* Valid input handle */
		for( u8Loop = 0U ; u8Loop < EVENTGROUP_MAX ; u8Loop++)
		{
			if( tuEventHandle == gtsEventGroup[u8Loop].tuType )
			{
				u32BitsToGet = gtsEventGroup[u8Loop].u32Event;
				break;
			}
			else
			{
				;/* not matched */
			}
		}
	}
	else
	{
		/* event group out of range do nothing */
		HAL_UART_Printf("not support event group:%d\r\n",tuEventHandle);
	}

	return u32BitsToGet;
}

uint32_t Event_GroupWaitBits(tuEventGroupHandle tuEventHandle, const uint32_t cu32BitsToWait, bool bWaitForAll)
{
	uint8_t u8Loop = 0U;
	uint32_t u32ReturnBits = 0U;

	if( EventGroupEnd > tuEventHandle )
	{
		/* Valid input handle */
		for( u8Loop = 0U ; u8Loop < EVENTGROUP_MAX ; u8Loop++)
		{
			if( tuEventHandle == gtsEventGroup[u8Loop].tuType )
			{
				if( true == bWaitForAll )
				{
					if( cu32BitsToWait == gtsEventGroup[u8Loop].u32Event )
					{
						u32ReturnBits = gtsEventGroup[u8Loop].u32Event;
					}
					else
					{
						; /* not match all bits wait for next time */
					}
				}
				else
				{
					u32ReturnBits = ( gtsEventGroup[u8Loop].u32Event & cu32BitsToWait );
				}
				break;
			}
			else
			{
				;/* not matched */
			}
		}
	}
	else
	{
		/* event group out of range do nothing */
		HAL_UART_Printf("not support event group:%d\r\n",tuEventHandle);
	}

	return u32ReturnBits;
}


