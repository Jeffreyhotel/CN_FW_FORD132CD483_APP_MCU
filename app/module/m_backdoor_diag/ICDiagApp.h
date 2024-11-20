/* -- Avoid Re-Includes -- */
#ifndef ICDIAGAPP_H
#define ICDIAGAPP_H

/* -- Includes -- */
#include <string.h> /* For string data type and memory handle. */
#include <stdint.h> /* For int data type. */
#include <stdbool.h> /* For bool data type. */
#include <stddef.h> /* For common macro. */
#include <stdlib.h> /* For common macro. */

#include "../../Mymain.h"
/* -- Macro Define  -- */
/**
 * @brief <b>Compiler option for IC diagnostic function.</b>
 * 
 * @details Diagnosis backdoor should turn off when release to customer. Turn on it when we need to diagnosis IC directly.\n
 * (For Customer Requirement & MisraC)This compiler option should use in related code, not only in ICDiagApp.c/ICDiagApp.\n
 * Set 0 : Turn OFF.\n
 * Set 1 : Turn ON.\n
 * 
 */

#if(BACKDOOR_ICDIAG_OPEN)

/**
 * @brief <b>Compiler option for GM.</b>
 * 
 * @details Driver API is different by project. Use these compiler option to seperate it.\n
 * (For MisraC)This compiler option might use in related code, not only in ICDiagApp.c/ICDiagApp.\n
 * Set 0 : Turn OFF.\n
 * Set 1 : Turn ON.\n
 * 
 */
#define ICDIAG_USE_GM_DRV_API			0U
/**
 * @brief <b>Compiler option for SGM.</b>
 * 
 * @details Driver API is different by project. Use these compiler option to seperate it.\n
 * (For MisraC)This compiler option might use in related code, not only in ICDiagApp.c/ICDiagApp.\n
 * Set 0 : Turn OFF.\n
 * Set 1 : Turn ON.\n
 * 
 */
#define ICDIAG_USE_SGM_DRV_API			0U
/**
 * @brief <b>Compiler option for FORD 23p6.</b>
 * 
 * @details Driver API is different by project. Use these compiler option to seperate it.\n
 * (For MisraC)This compiler option might use in related code, not only in ICDiagApp.c/ICDiagApp.\n
 * Set 0 : Turn OFF.\n
 * Set 1 : Turn ON.\n
 * 
 */
#define ICDIAG_USE_FORD23P6_DRV_API		0U
/**
 * @brief <b>Compiler option for FORD 13p2.</b>
 * 
 * @details Driver API is different by project. Use these compiler option to seperate it.\n
 * (For MisraC)This compiler option might use in related code, not only in ICDiagApp.c/ICDiagApp.\n
 * Set 0 : Turn OFF.\n
 * Set 1 : Turn ON.\n
 * 
 */
#define ICDIAG_USE_FORD13P2_DRV_API		1U

/* -- Includes -- */
#if ICDIAG_USE_GM_DRV_API
#include "app/inc/I2cMasterApp.h" /* For GM I2C IC driver API. */
#include "app/inc/MCUDiagApp.h" /* For GM MCU register/EEProm(NVM) API. */
#endif

#if ICDIAG_USE_SGM_DRV_API
#include "app/inc/I2cMasterApp.h" /* For GM I2C IC driver API. */
#include "app/inc/MCUDiagApp.h" /* For GM MCU register/EEProm(NVM) API. */
#endif

#if ICDIAG_USE_FORD13P2_DRV_API
#include "MCUDiagApp.h"
#endif

/* -- Macro Define  -- */
#if(CX430_TDDI_NT51926)
// #define	ICDIAG_CMD_ICFETCH				0xEBU /* Host to FIDM Command : Fetch command */
// #define	ICDIAG_CMD_READ					0xECU /* Host to FIDM Command : Read command */
// #define	ICDIAG_CMD_ICCTRL				0xEDU /* Host to FIDM Command : Write command */
#define	ICDIAG_CMD_ICFETCH				0xD0U /* Host to FIDM Command : Fetch command */
#define	ICDIAG_CMD_READ					0xD1U /* Host to FIDM Command : Read command */
#define	ICDIAG_CMD_ICCTRL				0xD2U /* Host to FIDM Command : Write command */
#elif(BX726_TDDI_NT51926)
#define	ICDIAG_CMD_ICFETCH				0xD0U /* Host to FIDM Command : Fetch command */
#define	ICDIAG_CMD_READ					0xD1U /* Host to FIDM Command : Read command */
#define	ICDIAG_CMD_ICCTRL				0xD2U /* Host to FIDM Command : Write command */
#elif(U717_TDDI_NT51926)
#define	ICDIAG_CMD_ICFETCH				0xD0U /* Host to FIDM Command : Fetch command */
#define	ICDIAG_CMD_READ					0xD1U /* Host to FIDM Command : Read command */
#define	ICDIAG_CMD_ICCTRL				0xD2U /* Host to FIDM Command : Write command */
#endif
#define	ICDIAG_RESULT_NONE				0x00U /* Driver command result : None(Default) */
#define	ICDIAG_RESULT_SUCCESS			0x01U /* Driver command result : Success */
#define	ICDIAG_RESULT_FAIL				0x02U /* Driver command result : Fail */

extern void ICDIAG_Initialize(void);
extern void ICDIAG_SetCmdResault(uint8_t u8CmdResault);
extern uint8_t* ICDIAG_GetRxBuffer(void);
extern void ICDIAG_CmdTrigger(uint8_t u8Cmd, uint8_t u8ICType, uint8_t u8Channel, uint8_t u8DeviceAddr, uint32_t u32DataAddr, uint8_t u8DataLength, uint8_t *u8DataBytes);
extern void ICDIAG_Main(void);

#endif
#endif

