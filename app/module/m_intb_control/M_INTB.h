#ifndef M_INTB_H
#define M_INTB_H

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
	aSETUP_TIME = 2U,
	aHOLD_TIME = 2U
}enTimeConstant;

typedef enum
{
	INTB_IDLE = 0x00000001UL,
	INTB_CHECK = 0x00000002UL,
	INTB_WAIT = 0x00000004UL,
	INTB_ASSERT = 0x00000008UL,
	INTB_DEASSERT = 0x00000010UL,
}enINTBState;

typedef enum
{
	aINTB_TIME_RDY = 0x00000001UL,
}enINTBFlag;


typedef struct
{
	int8_t (*Init)(uint8_t );
	int8_t (*StateMachine)(uint8_t *);
	int8_t (*Trigger)(uint8_t);
}tdINTBIF;

typedef struct
{
	unsigned bTrigger:1;
	unsigned bTimerStart:1;
	uint8_t u8State;
	uint32_t u32SetupTime;
	uint32_t u32HoldTime;
}tdINTBCtl;

extern tdINTBIF* GetINTB_Instance(void);

#if 1
/* -- Includes -- */
#include "../../Mymain.h"

/* -- Marco Define -- */
#define MINTB_SATISFIED_DEASSERTED_TIME (uint16_t)2u
#define MINTB_SATISFIED_ASSERTED_TIME (uint16_t)2u

typedef enum
{
    STRATEGY_CTRL_CANCEL = 0U,
    STRATEGY_CTRL_START,
    STRATEGY_CTRL_INIT,
    STRATEGY_CTRL_DEINIT,
}MINTB_StrategyCtrl_E;

typedef enum
{
    INTB_INT_TYPE_RISING = 0U,
    INTB_INT_TYPE_FALLING,
    INTB_INT_TYPE_DEFAULT,
}MINTB_InttruptType_E;

/* -- Data Type Define -- */
typedef void (*CALLBACK_SDM_INTB_DO_SET)(uint8_t u8DigitalLevel); 

/* -- Extern Global Variables -- */

/* -- Extern Functions -- */
extern bool MINTB_Register(CALLBACK_SDM_INTB_DO_SET CallbackSDMIntbDoSet, MINTB_InttruptType_E eInterruptType);
extern bool MINTB_StrategyControl(MINTB_StrategyCtrl_E eStrategyCtrl);
extern void MINTB_Routine(uint16_t u16RoutineTimeMs);
#endif
#endif
/* -- END -- */
