#pragma once

#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include <stddef.h>

#define TASK_FALSE				   (uint8_t)0x00U
#define TASK_TRUE                  (uint8_t)0x01U
//----------------------------------------------------------------------------------------------------------------------------
#define TASK_NUM                   (uint8_t)0x08U
#define TASK_QUEUE_SIZE            (uint8_t)0x04U
#define RECORDER_QUEUE_SIZE        (uint8_t)0x04U
//----------------------------------------------------------------------------------------------------------------------------
// Type Define
#define TYPE_NONE                  (uint8_t)0xFFU
//----------------------------------------------------------------------------------------------------------------------------
// State Define
#define STATE_NONE                 (uint8_t)0xFFU
#define STATE_INIT                 (uint8_t)0x00U
//----------------------------------------------------------------------------------------------------------------------------
// Event Define
#define EVENT_NONE                 (uint8_t)0xFFU
#define EVENT_FIRST                (uint8_t)0x01U
#define EVENT_DATA1                (uint8_t)0x02U
#define EVENT_DATA2                (uint8_t)0x03U
#define EVENT_MESSAGE			   (uint8_t)0x04U
#define EVENT_MESSAGE1			   (uint8_t)0x05U
#define EVENT_MESSAGE2			   (uint8_t)0x06U
#define EVENT_MESSAGE3			   (uint8_t)0x07U
#define EVENT_TIMER1               (uint8_t)0x08U
#define EVENT_TIMER2               (uint8_t)0x09U
#define EVENT_TIMER3               (uint8_t)0x0AU
#define EVENT_TIMER4               (uint8_t)0x0BU
#define EVENT_TIMER5               (uint8_t)0x0CU
#define EVENT_TIMER6               (uint8_t)0x0DU
//----------------------------------------------------------------------------------------------------------------------------
// Task Level Define
#define LEVEL_NONE                 (uint8_t)0xFFU
#define LEVEL1                     (uint8_t)0x01U                       // The task is not important and periodic.
#define LEVEL2                     (uint8_t)0x02U                       // The task is a middle level and periodic.
#define LEVEL3                     (uint8_t)0x03U                       // The task is very important and periodic.
#define LEVEL4                     (uint8_t)0x04U                       // The task is very important and nonperiodic.
#define LEVEL5                     (uint8_t)0x05U                       // The task is executed first time.

//----------------------------------------------------------------------------------------------------------------------------
// Handler methods
typedef void (*task_handler_def)(void);
//----------------------------------------------------------------------------------------------------------------------------
typedef struct
{
    uint8_t u8type;
    uint8_t u8state;
    uint8_t u8event;
    uint8_t u8level;
    task_handler_def ttask_handler;
}task_def;
//----------------------------------------------------------------------------------------------------------------------------
typedef struct
{
    task_def ttask[RECORDER_QUEUE_SIZE];
    uint8_t u8current_point;
    uint8_t u8end_point;
    uint8_t u8task_num;
    uint8_t u8queue_full;
    uint8_t u8discard_time;

    uint8_t u8interrupt_data1;
    uint8_t u8data1_level_temp;

    uint8_t u8interrupt_data2;
    uint8_t u8data2_level_temp;
}task_recorder_def;
//----------------------------------------------------------------------------------------------------------------------------
typedef struct
{
    task_def ttask[TASK_QUEUE_SIZE];
    uint8_t u8current_point;
    uint8_t u8end_point;
}queue_task_def;

//----------------------------------------------------------------------------------------------------------------------------
uint8_t Task_ExcludeDone(void);
void Task_AssignedPriority(void);
uint8_t Task_AssignedTask(void);
void Task_QueueManagerInit(void);
void Task_RecorderInit(void);
void Task_TaskInRecorder(void (*call_fun)(void),uint8_t u8type, uint8_t u8counter);
uint8_t Task_ChangeEvent(uint8_t u8type,uint8_t u8level, uint8_t u8event);
uint8_t Task_ChangeState(uint8_t u8type,uint8_t u8level, uint8_t u8state, void (*call_fun)(void));
void Task_TaskDone(void);
void Task_TaskInit(void);
void Task_IntAssign(void);
void Task_ClearTaskQueue(task_def *pttask);
void Task_ClearEvent(task_def *pttask);
void Task_PullTask(void);
void Task_PushTaskInQueue(void);
void Task_ExecuteTask(void);
void Task_PushState(void);
void Task_ProcessTask(void);
void Task_TimerCore(void);
uint8_t Task_Current_Event_Get(void);
#endif

