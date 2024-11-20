/******************************************************************************
 ;       Program		: MC_Power_Management.h
 ;       Function	: Declare main function & variable
 ;       Chip		: Cypress CY8C4149AZI-S598
 ;       Clock		: IMO Internal 48MHz
 ;       Date		:
 ;       Author		:
 ******************************************************************************/
#pragma once

#ifndef M_DISPLAYMANAGE_H
#define M_DISPLAYMANAGE_H

#include "../../Mymain.h"

/*---------------------------- Declare Type Define  --------------------------*/
/*For TD7800*/
#define TOUCH_ADDRESS				0x2CU
#define TD7800_SLAVE_ADDRESS		0x01U
#define TD7800_MASTER_ADDRESS		0x03U
/*For NT51926*/
#define TOUCH_SLAVE_ADDRESS			0x01U
#define TOUCH_SLAVE_ADDRESS_UP_FW	0x62U	/* touch firmware update */
#define NT51926_SLAVE_ADDRESS		0x6FU
#define NT51926_SLAVE_SECOND_ADDRESS		0x6DU
#define NT51925_STATUS_STANDY		0x00U
#define NT51925_STATUS_NORMAL		0x01U
#define NT51925_STATUS_BIST			0x02U
#define NT51925_STATUS_MUTE			0x03U
//#if(BX726_TDDI_NT51926)
#define CMD1		0x10U
#define CMD2		0x20U
#define CMD3		0x30U
#define CMDD		0xD0U

#define PAGE0		0x00U
#define PAGE1		0x01U
#define PAGE2		0x02U
#define PAGE3		0x03U
#define PAGE4		0x04U
#define PAGE5		0x05U
#define PAGE6		0x06U
#define PAGE7		0x07U
#define PAGE8		0x08U
#define PAGE9		0x09U
#define PAGEA		0x0AU
#define PAGEB		0x0BU
#define PAGEC		0x0CU
#define PAGEF		0x0FU

#define CMD1_P0		(uint8_t)(CMD1 | PAGE0)
//#endif

#define LP8864_ADDRESS				0x3AU		// for LED driver

extern void M_DM_I2cMasterInit(void);
extern void M_DM_BacklightControl(bool bEnable, bool bLockLoss);
extern uint8_t M_DM_TouchControl(uint8_t u8CurrentStatus, uint8_t u8Command);
extern uint8_t M_DM_DisplayControl(uint8_t u8CurrentStatus, uint8_t u8Command, bool bLockLoss);
extern uint8_t M_DM_ScanningControl(uint8_t u8CurrentStatus, uint8_t u8Command);
extern uint8_t M_DM_LcdControl(uint8_t u8CurrentStatus, uint8_t u8Command);
extern uint32_t M_DM_VCOM_Get(void);
extern void M_DM_VCOM_Set(uint8_t *pReturnData);
extern void M_DM_DisplayStatus_Get(uint8_t *pReturnData);
extern void M_DM_FW_DisplayStatus_Get(uint8_t *pReturnData);
extern void M_DM_INTB_Ctrl(uint8_t u8DigitalLevel);
extern uint8_t M_DM_NT51926_Status_Get(void);
extern uint8_t M_DM_NT51926_ATTN_Read (void);

//#if(BX726_TDDI_NT51926)
extern int8_t M_DM_NT51926_VGAMMA_Get(uint8_t *);
//#endif
#endif

