/*
 * FIDM_Config.h
 *
 *  Created on: 2022年11月24日
 *      Author: JackyHWWu
 */

#ifndef PUBLIC_H_
#define PUBLIC_H_


#define aACTION_OK	0U

#define aTRUE		1U
#define aFALSE		0U

#define aCOMPARE_SAME	0U
#define aCOMPARE_DIFF	1U

/* Module Configuration */
#define BX726_NOVA		aFALSE
#define CX430_NOVA		aFALSE
#define U759_U760_NOVA	aTRUE
#define CDX707_HIMAX	aFALSE

/* Display(TDDI) Configuration */
#define TDDI_NT51926	aTRUE

/* TCON Configuration */
#define TCON_NT51366	aTRUE

/* FuSa RC+CRC */
#define FUSA			aTRUE

typedef enum
{
	aU759U760_NOVA = 1U,

}teModuleName;


#define aBIT0	0x00000001UL
#define aBIT1	0x00000002UL
#define aBIT2	0x00000004UL
#define aBIT3	0x00000008UL
#define aBIT4	0x00000010UL
#define aBIT5	0x00000020UL
#define aBIT6	0x00000040UL
#define aBIT7	0x00000080UL
#define aBIT8	0x00000100UL
#define aBIT9	0x00000200UL
#define aBIT10	0x00000400UL
#define aBIT11	0x00000800UL
#define aBIT12	0x00001000UL
#define aBIT13	0x00002000UL
#define aBIT14	0x00004000UL
#define aBIT15	0x00008000UL



#include "cycfg_peripherals.h"
#include "cycfg_pins.h"
//#include "FIDM_Config.h"
#include "simple_cli.h"
#include "Event_Group.h"
#include "SW_Timer.h"
#include "Queue.h"
//#include "I2C_Message.h"
#include "M_INTB.h"

#if(0)
/***********************************
 *             Functions
 ***********************************/
/* uart debug */

extern uint32_t uart_debug_init(void);
extern void uart_printf(void *fmt, ...);

/* component communication (i2c slave) */
extern void C_Communication_Task(uint8_t *, uint8_t );
extern void C_Communication_Init(void);

/* power management */
extern void C_PowerManagement_Task(uint8_t *, uint8_t );
extern void C_PowerManagement_Init(void);

/* timer module */
extern void timer_module_init(void);

/* I2C Message */
extern void Protocol_SupportInit(void);
extern uint32_t Protocol_UpdateBit(en_i2c_msg, en_sdm_bits, uint8_t);
extern uint8_t Protocol_ReadByte(en_i2c_msg, uint8_t *);
extern int8_t Protocol_IsWritable(en_i2c_msg);
//extern uint8_t CRC8_Calculation(const uint8_t *pu8data, const uint8_t u8Length);


/* simple CLI */
//extern uint8_t Get_MCUVersion(uint8_t*);
#endif
#endif /* PUBLIC_H_ */
