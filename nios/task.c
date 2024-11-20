#include "task.h"
#include "hal_uart.h"

uint8_t gu8undo_num = 0;
queue_task_def gtqueue_manager;
task_recorder_def gttask_recorder[TASK_NUM];

uint8_t Task_Current_Event_Get(void)//得到当前任务的事件值并进行返回
{
	return gtqueue_manager.ttask[gtqueue_manager.u8current_point].u8event;
}
//-------------------------------------------------------------------------------------------------------------------------
//                                           Task Assigner
//-------------------------------------------------------------------------------------------------------------------------
void Task_AssignedPriority(void)//找出系统事件中优先等级更高的任务并进行选择排序，并判断事件的任务数量不为0则进行discard_time++
{
    uint8_t u8row = 0;
    uint8_t u8counter = 0;
    task_recorder_def trecorder_temp;
    
	for(u8row = 0; u8row < gu8undo_num; u8row++)
	{
		for(u8counter = (u8row + 1); u8counter < gu8undo_num; u8counter++)
		{
			if((gttask_recorder[u8row].ttask[gttask_recorder[u8row].u8current_point].u8level + gttask_recorder[u8row].u8discard_time) <
			   (gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8current_point].u8level + gttask_recorder[u8counter].u8discard_time))
			{
				trecorder_temp = gttask_recorder[u8row];
				gttask_recorder[u8row] = gttask_recorder[u8counter];
				gttask_recorder[u8counter] = trecorder_temp;
			}
		}
	}

    for(u8counter = 0; u8counter < TASK_NUM; u8counter++)
    {
        if(gttask_recorder[u8counter].u8task_num != 0)
        	gttask_recorder[u8counter].u8discard_time++;
    }
}
//-------------------------------------------------------------------------------------------------------------------------
uint8_t Task_ExcludeDone(void)//选择排序，找出系统事件中task全部执行完毕的任务的个数并给出返回值 value < 8
{
    uint8_t u8exclude_num = 0;
    uint8_t u8row = 0;
    uint8_t u8counter = 0;
    task_recorder_def trecorder_temp;

    for(u8row = 0; u8row < TASK_NUM - 1; u8row++)
    {
        for(u8counter = (u8row + 1); u8counter < TASK_NUM; u8counter++)
        {
            if(gttask_recorder[u8row].u8task_num == 0)
            {
                if(gttask_recorder[u8counter].u8task_num != 0)
                {
                    trecorder_temp = gttask_recorder[u8row];
                    gttask_recorder[u8row] = gttask_recorder[u8counter];
                    gttask_recorder[u8counter] = trecorder_temp;
                }
            }
        }
    }
    for(u8row = 0; u8row < TASK_NUM; u8row++)
    {
        if(gttask_recorder[u8row].u8task_num == 0)
            u8exclude_num++;
    }
    return u8exclude_num;
}
//-------------------------------------------------------------------------------------------------------------------------
uint8_t Task_AssignedTask(void)//分配8个任务中还没有被执行的任务，并按照优先级进行排序后返回TASK_TRUE,否则返回TASK_FALSE
{
    uint8_t u8exclude_num = 0;
    uint8_t u8return;
    
    u8exclude_num = Task_ExcludeDone();
    gu8undo_num = TASK_NUM - u8exclude_num;

    if(gu8undo_num != 0U)
    {
		Task_AssignedPriority();
    	u8return = TASK_TRUE;
    }
    else
    {
    	u8return = TASK_FALSE;
    }

    return u8return;
}
//-------------------------------------------------------------------------------------------------------------------------
//                                           Task Recorder
//-------------------------------------------------------------------------------------------------------------------------
void Task_QueueManagerInit(void)//任务队列管理器初始化操作 任务队列管理器长度为4
{
    uint8_t u8counter = 0;
    gtqueue_manager.u8current_point = 0;
    gtqueue_manager.u8end_point = 0;
    
    for(u8counter = 0; u8counter < TASK_QUEUE_SIZE; u8counter++)
    {
        Task_ClearTaskQueue(&gtqueue_manager.ttask[u8counter]);
    }
}
//-------------------------------------------------------------------------------------------------------------------------
void Task_RecorderInit(void)//记录任务事件初始化,8个任务初始化后再对每个任务中的具体事件进行初始化
{
    uint8_t u8task_counter = 0;
    uint8_t u8queue_counter = 0;
    
    __disable_irq();
    for(u8task_counter = 0; u8task_counter < TASK_NUM; u8task_counter++)
    {
    	gttask_recorder[u8task_counter].u8discard_time = 0;
    	gttask_recorder[u8task_counter].u8current_point = 0;
    	gttask_recorder[u8task_counter].u8end_point = 0;
    	gttask_recorder[u8task_counter].u8task_num = 0;
    	gttask_recorder[u8task_counter].u8interrupt_data1 = TASK_FALSE;
    	gttask_recorder[u8task_counter].u8interrupt_data2 = TASK_FALSE;
        
        for(u8queue_counter = 0; u8queue_counter < RECORDER_QUEUE_SIZE; u8queue_counter++)
        {
        	gttask_recorder[u8task_counter].ttask[u8queue_counter].u8type = TYPE_NONE;
        	gttask_recorder[u8task_counter].ttask[u8queue_counter].u8level = LEVEL_NONE;
        	gttask_recorder[u8task_counter].ttask[u8queue_counter].u8event = EVENT_NONE;
        	gttask_recorder[u8task_counter].ttask[u8queue_counter].u8state = STATE_NONE;
        	gttask_recorder[u8task_counter].ttask[u8queue_counter].ttask_handler = NULL;
        }
    }
    __enable_irq();
}
//-------------------------------------------------------------------------------------------------------------------------
void Task_TaskInRecorder(void (*call_fun)(void),uint8_t u8type, uint8_t u8task_counter)//具体的某一个（传参）任务中的具体事件记录函数，将具体参数赋值到gttask_recorder的结构体中
{
	__disable_irq();
    gttask_recorder[u8task_counter].u8end_point =  (gttask_recorder[u8task_counter].u8end_point + 1) % RECORDER_QUEUE_SIZE;
    gttask_recorder[u8task_counter].u8task_num++;
    gttask_recorder[u8task_counter].ttask[0].ttask_handler = call_fun;
    gttask_recorder[u8task_counter].ttask[0].u8type = u8type;
    gttask_recorder[u8task_counter].ttask[0].u8state = STATE_INIT;
    gttask_recorder[u8task_counter].ttask[0].u8event = EVENT_FIRST;
    gttask_recorder[u8task_counter].ttask[0].u8level = LEVEL5;
    gttask_recorder[u8task_counter].u8discard_time = 0;
    __enable_irq();
}
//-------------------------------------------------------------------------------------------------------------------------
uint8_t Task_ChangeEvent(uint8_t u8type, uint8_t u8level, uint8_t u8event)//某一個任務中切換執行的事件狀態和level
{
    uint8_t u8counter;

    __disable_irq();

    for(u8counter = 0; u8counter < TASK_NUM; u8counter++)
    {
        if(gttask_recorder[u8counter].ttask[0].u8type  == u8type)
        {
            if(gttask_recorder[u8counter].u8task_num < RECORDER_QUEUE_SIZE)
            {
                gttask_recorder[u8counter].u8discard_time = 0;
                gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].u8type = u8type;
                if(gttask_recorder[u8counter].u8end_point > 0)
                {
                	gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].ttask_handler = gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point-1].ttask_handler;
                	gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].u8state = gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point-1].u8state;
                }
                else
                {
                	gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].ttask_handler = gttask_recorder[u8counter].ttask[RECORDER_QUEUE_SIZE-1].ttask_handler;
                	gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].u8state = gttask_recorder[u8counter].ttask[RECORDER_QUEUE_SIZE-1].u8state;
                }

                gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].u8level = u8level;
                gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].u8event = u8event;
                gttask_recorder[u8counter].u8end_point = (gttask_recorder[u8counter].u8end_point + 1) % RECORDER_QUEUE_SIZE;
                gttask_recorder[u8counter].u8task_num++;
                gttask_recorder[u8counter].u8queue_full = TASK_FALSE;

                return TASK_TRUE;
            }
            gttask_recorder[u8counter].u8queue_full = TASK_TRUE;
        }
    }
    __enable_irq();
    return TASK_FALSE;
} 
//-------------------------------------------------------------------------------------------------------------------------
uint8_t Task_ChangeState(uint8_t u8type,uint8_t u8level, uint8_t u8state, void (*call_fun)(void))
{
    uint8_t u8counter = 0;

    __disable_irq();
    for(u8counter = 0; u8counter < TASK_NUM; u8counter++)
    {
        if(gttask_recorder[u8counter].ttask[0].u8type == u8type)
        {
            if(gttask_recorder[u8counter].u8task_num < RECORDER_QUEUE_SIZE)
            {
                gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].u8type = u8type;
                gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].ttask_handler = call_fun;
                gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].u8state = u8state;
                gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].u8level = LEVEL5;
                gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8end_point].u8event = EVENT_FIRST;
                gttask_recorder[u8counter].u8discard_time = 0;
                gttask_recorder[u8counter].u8end_point = (gttask_recorder[u8counter].u8end_point + 1) % RECORDER_QUEUE_SIZE;
                gttask_recorder[u8counter].u8task_num++;
                gttask_recorder[u8counter].u8queue_full = TASK_FALSE;
                __enable_irq();
                return TASK_TRUE;
            }
            gttask_recorder[u8counter].u8queue_full = TASK_TRUE;
        }
    }
    __enable_irq();
    return TASK_FALSE;
}
//-------------------------------------------------------------------------------------------------------------------------
//                                           Task Executer
//-------------------------------------------------------------------------------------------------------------------------
void Task_PushTaskInQueue(void)//将8个任务的某个任务压入到任务队列的管理器中
{
    uint8_t u8counter;
    uint8_t u8size = 0;
    
    if(gu8undo_num > TASK_QUEUE_SIZE)
    {
    	u8size = TASK_QUEUE_SIZE;
    }
    else
    {
    	u8size = gu8undo_num;
    }
    
    gtqueue_manager.u8current_point = 0;
    gtqueue_manager.u8end_point = 0;
    
    for(u8counter = 0; u8counter < u8size; u8counter++)
    {        
    	gtqueue_manager.ttask[gtqueue_manager.u8end_point] = gttask_recorder[u8counter].ttask[gttask_recorder[u8counter].u8current_point];
    	gtqueue_manager.u8end_point += 1;
    }
}
//-------------------------------------------------------------------------------------------------------------------------
void Task_ClearTaskQueue(task_def *pttask)//清楚任务队列中的一个任务实体并指向NULL
{
    pttask->u8type = TYPE_NONE;
    pttask->u8state = STATE_NONE;
    pttask->u8event = EVENT_NONE;
    pttask->u8level = LEVEL_NONE;
    pttask->ttask_handler = NULL;
}
//-------------------------------------------------------------------------------------------------------------------------
void Task_ClearEvent(task_def *pttask)//任务中的事件清空函数
{
    pttask->u8event = EVENT_NONE;
    pttask->u8level = LEVEL_NONE;
}
//-------------------------------------------------------------------------------------------------------------------------
void Task_TaskDone(void)//具体任务的事件完毕函数，当前任务的的具体事件节点清空，当前任务的总事件数量-1即：u8task_num--
{
	uint8_t u8index;

	__disable_irq();
	u8index = gtqueue_manager.u8current_point;

    Task_ClearEvent(&gttask_recorder[u8index].ttask[gttask_recorder[u8index].u8current_point]);
    gttask_recorder[u8index].u8current_point = (gttask_recorder[u8index].u8current_point + 1) % RECORDER_QUEUE_SIZE;
    
    if(gttask_recorder[u8index].u8task_num > 0)
        gttask_recorder[u8index].u8task_num--;
    __enable_irq();
}
//-------------------------------------------------------------------------------------------------------------------------
void Task_PullTask(void)//将当前的任务从任务队列中出队，将任务队列管理器的current_point++(指向任务队列的后一个任务)
{
	__disable_irq();
    Task_ClearTaskQueue(&gtqueue_manager.ttask[gtqueue_manager.u8current_point]);
    gtqueue_manager.u8current_point++;
    __enable_irq();
}
//-------------------------------------------------------------------------------------------------------------------------
void Task_ExecuteTask(void)//执行任务队列中的任务，并进行出队列操作
{
    gtqueue_manager.ttask[gtqueue_manager.u8current_point].ttask_handler();
    Task_PullTask();
}
//-------------------------------------------------------------------------------------------------------------------------
void Task_ProcessTask(void)//
{
    if(gtqueue_manager.u8current_point < gtqueue_manager.u8end_point)
    {
        Task_ExecuteTask();
    }
    else
    {
    	__disable_irq();
        // The queue is empty and need to push new task into queue.
        if(Task_AssignedTask() == TASK_TRUE)
        {
        	Task_PushTaskInQueue();
        }
        __enable_irq();
    }
}
