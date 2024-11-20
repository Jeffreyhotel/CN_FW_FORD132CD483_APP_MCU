#ifndef _QUEUE_H_
#define _QUEUE_H_

/*Queue error code */
#define QUEUE_FULL			-1
#define QUEUE_EMPTY			-2
#define QUEUE_CTL_ERR		-3
#define QUEUE_NOT_SUPPORT	-4


#define QUEUE_MAX		10U
#define NORMAL_DATA   	5U
#define MFG_DATA   		25U

#define QUEUE_TYPE_NUM	2U


typedef enum{
	NORMAL_QUEUE,
	MFG_QUEUE,
	QUEUE_TYPE_MAX
}QueueType_e;


typedef struct {
    int16_t i16Head;
    int16_t i16Tail;
    int16_t i16Size;
    int16_t i16Max;
} QueueCtrl_t;

typedef struct{
	QueueType_e tuType;
	QueueCtrl_t	tuCtl;
	uint8_t	u8Data[QUEUE_MAX][NORMAL_DATA];
}Queue_t;



void Queue_Create(void);
int32_t Queue_Send(QueueType_e teType, const uint8_t *pu8Input, uint8_t u8Len);
int32_t Queue_Receive(QueueType_e teType, uint8_t *pu8Output, uint8_t *u8Len);
bool Queue_IsFull(QueueType_e teType);
void Queue_Go_Trough(QueueType_e teType);


/* TODO */
int TypicalQueue_GetFront(QueueCtrl_t* ptrObj);
int TypicalQueue_GetRear(QueueCtrl_t* ptrObj);
bool TypicalQueue_IsEmpty(QueueCtrl_t* ptrObj);


#endif
