#include "C_Power_Management.h"
#include "C_Display_Management.h"
#include "C_Diagnosis.h"
#include "Memory_Pool.h"

static tpower_manage_task_def tPowerManageTask;
static uint8_t u8SyncLowDebounce;

/******************************************************************************
 ;       Function Name			:	void C_TD7800_Manage_Init(void)
 ;       Function Description	:	This state will do power management initialize
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Power_Manage_ParaInit(void)
{
    u8SyncLowDebounce=0U;

    Memory_Pool_SyncStatus_Set(SYNC_UNKNOW);
    Memory_Pool_SoftwareReset_Set(false);
    tPowerManageTask.u16Timer1 = TIME_DISABLE;
}
/******************************************************************************
 ;       Function Name			:	void C_Power_Manage_Init(void)
 ;       Function Description	:	This state will do power management initialize
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Power_Manage_Init(void)
{
    C_Power_Manage_ParaInit();
    (void)Task_ChangeState(TYPE_POWER_MANAGE, LEVEL5, STATE_POWER_MANAGE_CTRL, Power_Manage_State_Machine[STATE_POWER_MANAGE_CTRL]);
    Task_TaskDone();
}
/******************************************************************************
 ;       Function Name			:	void C_Power_Manager_Control(void)
 ;       Function Description	:	This state will do power on/off sequence
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Power_Manager_Control(void)
{
	uint16_t u16Temp=0U;
    uint32_t u32Temp=0U;
	uint32_t u32CommDisplayStatus=0U;
    uint16_t u16SyncVol;
    uint8_t u8PowerStatus;

    switch (Task_Current_Event_Get())
    {
        case EVENT_FIRST :

            /* Switch to  Power On mode. */
            Memory_Pool_PowerState_Set(START_UP_STATE);
            Memory_Pool_PowerStatus_Set(POWER_ON);
            (void)Task_ChangeEvent(TYPE_POWER_MANAGE, LEVEL4, EVENT_MESSAGE);
            break;
        case EVENT_MESSAGE :
            switch (Memory_Pool_PowerState_Get())
            {
                case START_UP_STATE:
                    u8PowerStatus = M_PM_Sequnce_Execute(Memory_Pool_PowerStatus_Get());

                    if (u8PowerStatus == POWER_PASS)
                    {
						/* Enable power system successfully and switch to Normal Run mode. */
						Memory_Pool_PowerState_Set(NORMAL_RUN_STATE);
						Memory_Pool_PowerStatus_Set(POWER_ON_READY);
						(void)Task_ChangeEvent(TYPE_POWER_MANAGE, LEVEL4, EVENT_MESSAGE);
                    }
                    else if (u8PowerStatus == P3V3_FAIL)
                    {
			 			/* Record 0xA3 Status */
						u16Temp = Memory_Pool_GeneralDiagnosis_Get();
						Memory_Pool_GeneralDiagnosis_Set(u16Temp | BIT_A3_POWER_ON_P3V3_ERROR_POS);  

                        /* Disable power system. */
                        Memory_Pool_PowerState_Set(OFF_POWER_STATE);
                        Memory_Pool_PowerStatus_Set(POWER_OFF);
                        (void)Task_ChangeEvent(TYPE_POWER_MANAGE, LEVEL4, EVENT_MESSAGE);
                    }
                    else if (u8PowerStatus == POWER_VBAT_FAIL)
                    {
						/* Disable power system. */
						Memory_Pool_PowerState_Set(OFF_POWER_STATE);
						Memory_Pool_PowerStatus_Set(POWER_OFF);
						(void)Task_ChangeEvent(TYPE_POWER_MANAGE, LEVEL4, EVENT_MESSAGE);
                    }
                    else if (u8PowerStatus == P1V2_FAIL)
                    {
			 			/* Record 0xA3 Status */
						u16Temp = Memory_Pool_GeneralDiagnosis_Get();
						Memory_Pool_GeneralDiagnosis_Set(u16Temp | BIT_A3_POWER_ON_P1V2_ERROR_POS);

                        /* Disable power system. */
                        Memory_Pool_PowerState_Set(OFF_POWER_STATE);
                        Memory_Pool_PowerStatus_Set(POWER_OFF);
                        (void)Task_ChangeEvent(TYPE_POWER_MANAGE, LEVEL4, EVENT_MESSAGE);
                    }
                    else
                    { /*Nothing*/ }
                    break;
                case OFF_POWER_STATE:
                    /* Disable power system. */
					tPowerManageTask.u16Timer1=TIME_DISABLE;				
#if (!BACKDOOR_DIAGNOSIS_SIMULATE)
                    M_PM_Sequnce_Execute(Memory_Pool_PowerStatus_Get());
#endif
                    Memory_Pool_PowerStatus_Set(POWER_OFF_READY);

                    /* Clear INIT bit in Display Status. */
                    u32CommDisplayStatus = Memory_Pool_DisplayStatus_Get();
                    u32Temp = Memory_Pool_ActualDisplayStatus_Get();
                    Memory_Pool_DisplayStatus_Set(u32CommDisplayStatus & ~BIT_INIT_POS);
                    Memory_Pool_ActualDisplayStatus_Set(u32Temp & ~BIT_INIT_POS);

					/* Power off TBD, it won't restore power.*/
                    /* Check software reset request. */
                    if(Memory_Pool_SoftwareReset_Get() == true)
                    {
                        NVIC_SystemReset();
                    }
					else
					{/*Nothing*/}
                    break;
                case SHUTDOWN1OR2_STATE:
					 /* Mask serdes communication bus to avoid that shutdown flow be broken */
#if (!BACKDOOR_DIAGNOSIS_SIMULATE)
                    Memory_Pool_CommunicationMask_Set(true);
#endif
                    /* Disable diagnosis functions*/
                    Memory_Pool_DiagnosisEnable_Set(false);
                    (void)Task_ChangeEvent(TYPE_DIAGNOSIS, LEVEL5, EVENT_MESSAGE_DISANOSIS_ENABLE);

                    /*Disable display functions */
                    Memory_Pool_DisplayEnable_Set(DISPLAY_OFF_TOUCH_OFF);

					/*Disable Backlight functions */
					Memory_Pool_BacklightEnable_Set(false);
                    (void)Task_ChangeEvent(TYPE_DISPLAY_MANAGE, LEVEL4, EVENT_MESSAGE_DISPLAY_ENABLE);
                    break;
                case NORMAL_RUN_STATE:
                    /* Delay 100ms to enable SYNC detection function periodically. */
                    tPowerManageTask.u16Timer1 = TIME_100ms;
                    break;
                case NORMAL_DISABLE_STATE:
                    break;
                default:
                    break;
            }
            break;
        case EVENT_SYNCS_CHECK_PERIOD :
			u16SyncVol=Memory_Pool_SyncVol_Get();

			if(u16SyncVol < SYNC_LOW_VOL)
			{
				u8SyncLowDebounce+=1U;
			}
			else
			{
				u8SyncLowDebounce=0U;
			}
			
			/*Action when over debounce threadhold*/
			if(u8SyncLowDebounce >= SYNC_LOW_DEBOUNCE_MAX)
			{
				u8SyncLowDebounce=SYNC_LOW_DEBOUNCE_MAX;
				Memory_Pool_SyncStatus_Set(SYNC_DISABLE);
				Memory_Pool_PowerState_Set(SHUTDOWN1OR2_STATE);
				(void)Task_ChangeEvent(TYPE_POWER_MANAGE, LEVEL4, EVENT_MESSAGE);
			}
			else
			{
				tPowerManageTask.u16Timer1=TIME_2ms;
			}

			if(Memory_Pool_SyncStatus_Get() == SYNC_DISABLE)
			{
                if(Memory_Pool_PowerState_Get() == OFF_POWER_STATE)
                {
                    tPowerManageTask.u16Timer1=TIME_DISABLE;    
                }
                else
                {
                    tPowerManageTask.u16Timer1=TIME_2ms;
                    Memory_Pool_PowerState_Set(SHUTDOWN1OR2_STATE);
                    (void)Task_ChangeEvent(TYPE_POWER_MANAGE, LEVEL4, EVENT_MESSAGE);
                }
			}
			else
			{/*Nothing*/}
			
			break;
		default:
			(void)Task_ChangeState(TYPE_POWER_MANAGE, LEVEL5, STATE_POWER_MANAGE_ERROR, Power_Manage_State_Machine[STATE_POWER_MANAGE_ERROR]);
			break;
    }
    Task_TaskDone();

}
/******************************************************************************
 ;       Function Name			:	void C_Power_Manage_Error(void)
 ;       Function Description	:	This state for error condition
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
static void C_Power_Manage_Error(void)
{
    Task_TaskDone();
}
/******************************************************************************
 ;       Function Name			:	void C_Power_Manage_Timer1(void)
 ;       Function Description	:	This function for timing using
 ;       Parameters				:	void
 ;       Return Values			:	void
 ;		Source ID				:
 ******************************************************************************/
void C_Power_Manage_Timer1(void)
{
    if (tPowerManageTask.u16Timer1 > TIME_UP)
    {
        tPowerManageTask.u16Timer1--;
        if (tPowerManageTask.u16Timer1 == TIME_UP)
        {
            tPowerManageTask.u16Timer1 = TIME_DISABLE;
            (void)Task_ChangeEvent(TYPE_POWER_MANAGE, LEVEL3, EVENT_TIMER1);
        }
        else
        { /* Nothing */ }
    }
    else
    { /* Nothing */ }
}

void (*const Power_Manage_State_Machine[MAX_PM_STATE_NO])(void) =
{   C_Power_Manage_Init, C_Power_Manager_Control, C_Power_Manage_Error };

