#include "hal_uart.h"
#include "queue.h"
#include "../app/Mymain.h"


//static uint32_t gu32Queue[QUEUE_MAX];
//static QueueCtrl_t gtdQueue;

static uint8_t gu8NormalQueue[NORMAL_DATA];
static uint8_t gu8MFGQueue[NORMAL_DATA];

static Queue_t gtsQueue[QUEUE_MAX] =
{
	{
		.tuType = NORMAL_QUEUE,
		//.u8Data = gu8NormalQueue,
		//.u8Len = NORMAL_DATA,
	},
	{
		.tuType = MFG_QUEUE,
		//.u8Data = gu8MFGQueue,
		//.u8Len = MFG_DATA,
	}
};


static Queue_t *Queue_GetPHY(QueueType_e teType);


void Queue_Create(void)
{
    uint8_t u8Loop = 0U, u8Clean = 0U, u8Format = 0U;

    for( u8Loop = 0U ; u8Loop < QUEUE_MAX ; u8Loop++ )
    {
    	gtsQueue[u8Loop].tuCtl.i16Size = 0U;
    	gtsQueue[u8Loop].tuCtl.i16Max = QUEUE_MAX;
    	gtsQueue[u8Loop].tuCtl.i16Head = 0U;
    	gtsQueue[u8Loop].tuCtl.i16Tail = -1;

    	for( u8Clean = 0U ; u8Clean < QUEUE_MAX ; u8Clean++ )
    	{
    		for( u8Format = 0U ; u8Format < NORMAL_DATA ; u8Format++ )
    			gtsQueue[u8Loop].u8Data[u8Clean][u8Format] = 0U;
    	}

    }
    return;
}


static Queue_t *Queue_GetPHY(QueueType_e teType)
{
	uint8_t u8Loop = 0U;
	Queue_t *ptdQTmp;

	for( u8Loop = 0U; u8Loop < QUEUE_TYPE_MAX; u8Loop++ )
	{
		if( teType == gtsQueue[u8Loop].tuType )
		{
			ptdQTmp = &gtsQueue[u8Loop];
			break;
		}
		else
		{
			;/* not match */
		}
	}

	return ptdQTmp;
}

void Queue_Go_Trough(QueueType_e teType)
{
    uint8_t u8Loop = 0U, u8Element = 0U;
    Queue_t *ptdQTmp = NULL;

    ptdQTmp = Queue_GetPHY(teType);

    HAL_UART_Printf("Go through %s Queue\r\n", teType == NORMAL_QUEUE? "Normal" : "MFG");
    for( u8Loop = 0U ; u8Loop < QUEUE_MAX ; u8Loop++)
    {
    	for( u8Element = 0U ; u8Element < NORMAL_DATA ; u8Element++ )
    		HAL_UART_Printf("[%02x]",ptdQTmp->u8Data[u8Loop][u8Element]);

    	HAL_UART_Printf("\r\n");
    }
}




int32_t Queue_Send(QueueType_e teType, const uint8_t *pu8Input, uint8_t u8Len)
{
	int32_t i32RtCode = -1;
	uint8_t u8Input = 0U;
	QueueCtrl_t *ptdQCtl = NULL;
	Queue_t *ptdQTmp = NULL;

	ptdQTmp = Queue_GetPHY(teType);
	if( NULL != ptdQTmp )
	{
		ptdQCtl = &ptdQTmp->tuCtl;
		if( NULL != ptdQCtl )
		{
			if( ptdQCtl->i16Max != ptdQCtl->i16Size)
			{
				/* not full */
				ptdQCtl->i16Tail = ( (ptdQCtl->i16Tail + 1U) % ptdQCtl->i16Max );

				for( u8Input = 0U ; u8Input < u8Len ; u8Input++ )
				{
					ptdQTmp->u8Data[ptdQCtl->i16Tail][u8Input] = *pu8Input;
					pu8Input++;
				}

				ptdQCtl->i16Size++;
				i32RtCode = true;
			}
			else
			{
				i32RtCode = QUEUE_FULL; /* Queue full */
			}
		}
		else
		{
			i32RtCode = QUEUE_CTL_ERR; /* Queue control error */
		}
	}
	else
	{
		i32RtCode = QUEUE_NOT_SUPPORT;	/* Un-support queue */
	}



    return i32RtCode;
}

int32_t Queue_Receive(QueueType_e teType, uint8_t *pu8Output, uint8_t *u8Len)
{
	int32_t rtCode = -2;
	uint8_t u8Loop = 0U;
	QueueCtrl_t *ptdQCtl = NULL;
	Queue_t *ptdQTmp = NULL;

	ptdQTmp = Queue_GetPHY(teType);
	if( NULL != ptdQTmp)
	{
		ptdQCtl = &ptdQTmp->tuCtl;
		if( ptdQCtl != NULL )
		{
			if( 0U != ptdQCtl->i16Size )
			{
				/* Copy data */
				for( u8Loop = 0U ; u8Loop < NORMAL_DATA ; u8Loop++ )
				{
					*pu8Output = ptdQTmp->u8Data[ptdQCtl->i16Tail][u8Loop];
					ptdQTmp->u8Data[ptdQCtl->i16Tail][u8Loop] = 0U;
					pu8Output++;
				}

				if(ptdQCtl->i16Head == ptdQCtl->i16Tail){
					ptdQCtl->i16Head = 0;
					ptdQCtl->i16Tail = -1;
				}
				else
				{
					ptdQCtl->i16Head = (ptdQCtl->i16Head + 1) % ptdQCtl->i16Max;
				}

				ptdQCtl->i16Size--;
				rtCode = true;
			}
			else
			{
				rtCode = QUEUE_EMPTY; /* Queue is empty */
			}
		}
		else
		{
			rtCode = QUEUE_CTL_ERR;	/* Queue control error */
		}
	}
	else
	{
		rtCode = QUEUE_NOT_SUPPORT; /* Un-support queue */
	}
    
    return rtCode;
}

int TypicalQueue_GetFront(QueueCtrl_t* ptrObj) {
    int rtVal = 0U;
    
    if( NULL != ptrObj ){
        if( 0U == ptrObj->i16Size){
            rtVal = -1;
            //printf("Front empty\r\n");
        }
        else{
            //rtVal = ptrObj->u32ptrQueue[ptrObj->i16Head];
            //printf("Front:%d\r\n",rtVal);
        }
    }
    else{
        ;
    }
    return rtVal;
}

int TypicalQueue_GetRear(QueueCtrl_t* ptrObj) {
    int rtVal = 0U;
    
    if( NULL != ptrObj ){
        if( 0U == ptrObj->i16Size ){
            rtVal = -1;   
            //printf("Rear empty\r\n");
        }
        else{
            //rtVal = ptrObj->u32ptrQueue[ptrObj->i16Tail];
            //printf("Rear:%d\r\n",rtVal);
        }
    }
    else{
        ;
    }
    return rtVal;
}

bool TypicalQueue_IsEmpty(QueueCtrl_t* ptrObj) {
    bool rtCode = false;
    
    if( NULL != ptrObj && (0U == ptrObj->i16Size) ){
        /* empty */
        rtCode = true;
    }
    else{
        ;
    }
    return rtCode;
}

bool Queue_IsFull(QueueType_e teType)
{
    bool rtCode = false;
    Queue_t *ptdQTmp = NULL;
    QueueCtrl_t *ptrObj = NULL;


    ptdQTmp = Queue_GetPHY(teType);
    ptrObj = &ptdQTmp->tuCtl;
    
    if( NULL != ptrObj && (ptrObj->i16Size == ptrObj->i16Max) ){
        rtCode = true;
    }
    else{
        ;
    }
    return rtCode;
}
#if 0
void myCircularQueueFree(QueueCtrl_t* ptrObj) {
    free(ptrObj->u32ptrQueue);
}
#endif
