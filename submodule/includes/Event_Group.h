/*
 * Event_Group.h
 *
 *  Created on: 2022年9月7日
 *      Author: JackyHWWu
 */
#pragma once
#ifndef INCLUDES_APP_EVENT_GROUP_H_
#define INCLUDES_APP_EVENT_GROUP_H_

#include <stdint.h>

#define aNOTALL		0u
#define aWAITALL	1u


typedef enum{
//	aEVENT_DISPLAY_MANAGEMENT = 1U,
//	aEVENT_BACKLIGHT_MANAGEMENT,
//	aEVENT_TIMER,
//	aEVENT_PWR,
	aEVENT_INTB=1U,
	EventGroupEnd
}tuEventGroupHandle;


#define EVENTGROUP_MAX	EventGroupEnd - 1


typedef struct{
	const tuEventGroupHandle tuType;
	uint32_t	u32Event;
}tsEvenGroup;

int32_t EventGroup_Create(void);
int32_t Event_GroupSetBits(tuEventGroupHandle tuEventHandle, const uint32_t cu32BitsToSet);
int32_t Event_GroupClearBits(tuEventGroupHandle tuEventHandle, const uint32_t cu32BitsToClear);
uint32_t Event_GroupGetBits(tuEventGroupHandle tuEventHandle);
uint32_t Event_GroupWaitBits(tuEventGroupHandle tuEventHandle, const uint32_t cu32BitsToWait, bool bWaitForAll);


#endif /* INCLUDES_APP_EVENT_GROUP_H_ */
