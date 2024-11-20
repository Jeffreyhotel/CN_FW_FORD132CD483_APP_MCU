#include "C_Backlight_Management.h"
#include "Memory_Pool.h"
#include "M_BacklightControl.h"
#include "hal_pwm.h"
#include "hal_gpio.h"

static tbacklight_manage_task_def tBacklightManageTask;
/******************************************************************************
 ;       Function Name			:	void C_Backlight_Manage_Init(void)
 ;       Function Description	:	This state will do Backlight management initialize
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Backlight_Manage_Init(void)
{
	(void)HAL_PWM_CH4_Init();
	tBacklightManageTask.u16Timer1 = TIME_DISABLE;
	tBacklightManageTask.u16Timer2 = TIME_DISABLE;
	(void)Task_ChangeState(TYPE_BACKLIGHT_MANAGE, LEVEL5, STATE_BACKLIGHT_MANAGE_CTRL, Backlight_Manage_State_Machine[STATE_BACKLIGHT_MANAGE_CTRL]);
	Task_TaskDone();
}

/******************************************************************************
 ;       Function Name			:	void C_Backlight_Manager_Control(void)
 ;       Function Description	:	This state will do Backlight on/off sequence
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Backlight_Manager_Control(void)
{
	switch (Task_Current_Event_Get())
	{
		case EVENT_FIRST:
			tBacklightManageTask.u16Timer1 = TIME_6ms;
			tBacklightManageTask.u16Timer2 = TIME_10ms;
			(void)Task_ChangeEvent(TYPE_BACKLIGHT_MANAGE, LEVEL4, EVENT_MESSAGE);
		break;

		case EVENT_MESSAGE:
			MBacklightControl_TriggerStateMachine();
		break;

		/* En/DIS derating  */
		case EVENT_MESSAGE1:
			if(Memory_Pool_DeratingEnable_Get())
			{
				MBacklightControl_TurnOnOffDerating(E_MBL_ENABLE_DERATING);
			}
			else
			{
				MBacklightControl_TurnOnOffDerating(E_MBL_DISABLE_DERATING);
			}
		break;

		case EVENT_TIMER1:
			tBacklightManageTask.u16Timer1 = TIME_6ms;
			MBacklightControl_DimmingModuleRoutine6ms();
		break;

		case EVENT_TIMER2:
			tBacklightManageTask.u16Timer2 = TIME_10ms;
			MBacklightControl_DeratingModuleRoutine10ms();
		break;

		default:
			(void)Task_ChangeState(TYPE_BACKLIGHT_MANAGE, LEVEL5, STATE_BACKLIGHT_MANAGE_ERROR, Backlight_Manage_State_Machine[STATE_BACKLIGHT_MANAGE_ERROR]);
		break;
	}

	Task_TaskDone();
}
/******************************************************************************
 ;       Function Name			:	void C_Backlight_Manage_Error(void)
 ;       Function Description	:	This state for error condition
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Backlight_Manage_Error(void)
{
	Task_TaskDone();
}
/******************************************************************************
 ;       Function Name			:	void C_Backlight_Manage_Timer1(void)
 ;       Function Description	:	This function for timing using
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
void C_Backlight_Manage_Timer1(void)
{
	if (tBacklightManageTask.u16Timer1 > TIME_UP)
	{
		tBacklightManageTask.u16Timer1--;
		if (tBacklightManageTask.u16Timer1 == TIME_UP)
		{
			tBacklightManageTask.u16Timer1 = TIME_DISABLE;
			(void)Task_ChangeEvent(TYPE_BACKLIGHT_MANAGE, LEVEL3, EVENT_TIMER1);
		}
		else
		{ /* Nothing */ }
	}
	else
	{ /* Nothing */ }
}
void C_Backlight_Manage_Timer2(void)
{
	if (tBacklightManageTask.u16Timer2 > TIME_UP)
	{
		tBacklightManageTask.u16Timer2--;
		if (tBacklightManageTask.u16Timer2 == TIME_UP)
		{
			tBacklightManageTask.u16Timer2 = TIME_DISABLE;
			(void)Task_ChangeEvent(TYPE_BACKLIGHT_MANAGE, LEVEL3, EVENT_TIMER2);
		}
		else
		{ /* Nothing */ }
	}
	else
	{ /* Nothing */ }
}

void (*const Backlight_Manage_State_Machine[MAX_BL_STATE_NO])(void) =
{   C_Backlight_Manage_Init, C_Backlight_Manager_Control, C_Backlight_Manage_Error };

