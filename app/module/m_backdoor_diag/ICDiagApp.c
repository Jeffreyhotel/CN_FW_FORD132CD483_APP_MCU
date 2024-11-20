/**
 * @file ICDiagApp.c
 * 
 * @brief Provide interface to diagnosis IC(I2C) & MCU.
 * 
 * @details Provide interface to packed related command to read/write I2C ICs, MCU register and MCU EEProm(NVM).\n
 * Through these functions, FIDM can fetch/control devices/ICs directly without restricted rules.\n
 * Finally, we can analysis issues more effeciency.\n
 * Following is interface functions brief, these function must merge to FIDM process.\n
 * \n
 * 1.ICDIAG_Initialize use to init related valuables.\n
 * 2.ICDIAG_SetCmdResault use to set Fetch/Wirte result for ICDIAG_Main.\n
 * 3.ICDIAG_GetRxBuffer use to get rx buffer data point to feedback diagnostic data.\n
 * 4.ICDIAG_CmdTrigger use get Fetch/Write command and package necessary information.\n
 * 5.ICDIAG_Main is main loop to submit packed command to driver layer and use state machine to control process.\n
 * 
 * @note v0.1(Date:2022-03-30) : First time release.
 * 
 * @author Alvin Fan(Alvin.Fan@auo.com)
 * 
 * @version 0.1
 * 
 * @date 2022-03-31
 * 
 */

/* -- Includes -- */
#include "ICDiagApp.h" /* For necessary include file and macro. */

#if(BACKDOOR_ICDIAG_OPEN)
/* -- Macro Define  -- */
#define	ICDIAG_ICTYPE_I2C			0x01U /* IC with I2C. */
#define	ICDIAG_ICTYPE_SPI			0x02U /* IC with SPI. */
#define	ICDIAG_ICTYPE_MCU_MEM		0x03U /* MCU register. */
#define	ICDIAG_ICTYPE_MCU_NVM		0x04U /* MCU EEProm(NVM). */
#define	ICDIAG_ICTYPE_MCU_REG		0x05U

#define	ICDIAG_STATUS_IDLE			0x00U /* Process idle */
#define	ICDIAG_STATUS_PACKED		0x01U /* Command packed after recieve from host, ready to submit. */
#define	ICDIAG_STATUS_SUBMITTED		0x02U /* Command have submitted to driver layer. */

#define	ICDIAG_FETCH_CTRL_MINLEN	1U /* Min data length for Fetch/Write command. */
#define	ICDIAG_FETCH_CTRL_MAXLEN	64U /* Max data length for Fetch/Write command. */

#define	ICDIAG_ICRX_BUFFERSIZE		64U /* Rx buffer size. */
#define	ICDIAG_ICTX_BUFFERSIZE		65U /* Tx buffer size, 1 more than rx buffer size for device addr. */

/* -- Data Type  -- */
/**
 * @brief <b>Include state machine, command result, command information and read/write data.</b>
 * 
 */
typedef struct
{
	uint8_t u8CommandState; /**< Process state machine. */
	uint8_t u8CommandResult; /**< Fetch/write command result. */
	uint8_t u8ICType; /**< Recognize whitch kind of IC to handle. */
	uint8_t u8Channel; /**< Deivce communication channel. */
	uint8_t u8DeviceAddr; /**< IC device address. */
	uint32_t u32DataAddr; /**< Data address */
	uint8_t u8ICRxLen; /**< How many data bytes we want to read */
	uint8_t u8ICTxLen; /**< How many data bytes we want to fetch/write */
	uint8_t u8ICRxBuf[ICDIAG_ICRX_BUFFERSIZE]; /**< Rx data buffer */
	uint8_t u8ICTxBuf[ICDIAG_ICTX_BUFFERSIZE]; /**< Tx data buffer */
}ICDIAG_TypeDef;  

/* -- Global Variables  -- */
/**
 * @brief <b>Static valuable use to control ICDiagApp.c.</b>
 * 
 * @details Include state machine, command result, command information and read/write data.\n
 * 
 */
static ICDIAG_TypeDef I2CICDiagCtrl={0U};

/**
 * @brief Use to fill in default/invalid data to rx buffer.
 * 
 * @details Set 0xFF as default/invalid data.\n
 * When host read 0xFF means there is no fetch/write command or command fail.\n
 * When this shit happen, please analysis host or driver to find root cause.\n
 *
 * @note Static function.
 * 
 */
static void ICDIAG_FillinDefaultRXData(void)
{
	uint8_t u8Index=0;

	for(u8Index=0U; u8Index<ICDIAG_ICRX_BUFFERSIZE; u8Index++)
	{
		I2CICDiagCtrl.u8ICRxBuf[u8Index]=0xFFU;
	}	
}

/**
 * @brief <b>Set command state machine.</b>
 * 
 * @details Set command state machine as ICDIAG_STATUS_IDLE/ICDIAG_STATUS_PACKED/ICDIAG_STATUS_SUBMITTED let process know what to do next.
 *
 * @note Static function.
 * 
 */
static void ICDIAG_SetCmdState(uint8_t u8CmdState)
{
	I2CICDiagCtrl.u8CommandState=u8CmdState;
}

/**
 * @brief <b>Set command result.</b>
 * 
 * @details Set command result after fetch/write command complete.\n
 * When fetch/write complete and health, set ICDIAG_RESULT_SUCCESS.\n
 * When fetch/write fail like timeout or other errors, set ICDIAG_RESULT_FAIL.\n
 * When state machine go back to ICDIAG_STATUS_IDLE, set ICDIAG_RESULT_NONE.\n
 *
 * @note Static function.
 * 
 */
void ICDIAG_SetCmdResault(uint8_t u8CmdResault)
{
	I2CICDiagCtrl.u8CommandResult=u8CmdResault;
}

/**
 * @brief <b>Use to init related valuables.</b>
 * 
 * @details This function must execute when FIDM init.\n
 * 1.Clear I2CICDiagCtrl.\n
 * 2.Clear RX data buffer.\n
 * 3.Set state machine as ICDIAG_STATUS_IDLE.\n
 * 4.Set command result as ICDIAG_RESULT_NONE.\n
 * 
 */
void ICDIAG_Initialize(void)
{
    (void)memset(&I2CICDiagCtrl, 0U, sizeof(ICDIAG_TypeDef));
	ICDIAG_FillinDefaultRXData();
	ICDIAG_SetCmdState(ICDIAG_STATUS_IDLE);
	ICDIAG_SetCmdResault(ICDIAG_RESULT_NONE);
}

/**
 * @brief <b>Feedback recieve data to host.</b>
 * 
 * @details When host ask diagnosis data by ICDIAG_CMD_READ, feedback RX data buffer point.
 * 
 * @return RX data buffer point.
 * 
 * @retval RX data buffer point.
 * 
 */
uint8_t* ICDIAG_GetRxBuffer(void)
{
	return I2CICDiagCtrl.u8ICRxBuf;
}

/**
 * @brief <b>Check, recieve and pack fetch/write command information</b>
 * 
 * @details This function will check, recieve and pack fetch/write command.\n
 * If basic information match definition, command will packed and change process state to ICDIAG_STATUS_PACKED.\n
 * After state ICDIAG_STATUS_PACKED, this function won't accept more command unless state change to ICDIAG_STATUS_IDLE.\n
 * 
 * @note ICDIAG_ICTYPE_SPI doesn't implement, reserve this type for future.
 * 
 * @param u8Cmd ICDIAG_CMD_ICFETCH / ICDIAG_CMD_ICCTRL\n
 * @param u8ICType ICDIAG_ICTYPE_I2C / ICDIAG_ICTYPE_MCU_MEM / ICDIAG_ICTYPE_MCU_NVM / ICDIAG_ICTYPE_SPI(TBD)\n
 * @param u8Channel When ICDIAG_ICTYPE_I2C, this param means communication channel to target IC.\n
 * @param u8DeviceAddr When ICDIAG_ICTYPE_I2C, this param means target IC device address.\n
 * @param u32DataAddr Target data address.\n
 * @param u8DataLength How many data we want to fetch/write.\n
 * @param *u8DataBytes When ICDIAG_CMD_ICCTRL, here is the data pointer.\n
 *
 * @details <b>Command State : ICDIAG_STATUS_IDLE -> ICDIAG_STATUS_PACKED.</b>\n
 * State transfers to ICDIAG_STATUS_PACKED when basic information match.\n
 * 1.CommandState : ICDIAG_STATUS_IDLE\n
 * 2.Data length : 1-64\n
 * 3.Command type : ICDIAG_CMD_ICCTRL or ICDIAG_CMD_ICFETCH\n
 * 4.IC type : ICDIAG_ICTYPE_I2C or ICDIAG_ICTYPE_MCU_MEM or ICDIAG_ICTYPE_MCU_NVM\n
 * 
 * @dot
 * digraph CommandState{ 
 *	"ICDIAG_STATUS_IDLE" -> "ICDIAG_STATUS_PACKED"
 * }
 * @enddot
 * 
 */
void ICDIAG_CmdTrigger(uint8_t u8Cmd, uint8_t u8ICType, uint8_t u8Channel, uint8_t u8DeviceAddr, uint32_t u32DataAddr, uint8_t u8DataLength, uint8_t *u8DataBytes)
{
	int8_t i8Index=0;
	
	/* Fill in command info when state IDLE */
	if(I2CICDiagCtrl.u8CommandState == ICDIAG_STATUS_IDLE)
	{
		I2CICDiagCtrl.u8ICRxLen = 0U;
		I2CICDiagCtrl.u8ICTxLen = 0U;
		
		/* Check Data Length Mim&Max */
		if((u8DataLength >= ICDIAG_FETCH_CTRL_MINLEN) && (u8DataLength <= ICDIAG_FETCH_CTRL_MAXLEN))
		{
			if(u8Cmd == ICDIAG_CMD_ICCTRL)
			{
				/*Pack I2C write or control package for driver API*/
				if(u8ICType == ICDIAG_ICTYPE_I2C)
				{
					I2CICDiagCtrl.u8ICType=u8ICType;
					I2CICDiagCtrl.u8Channel=u8Channel;
					I2CICDiagCtrl.u8DeviceAddr=u8DeviceAddr;
					I2CICDiagCtrl.u32DataAddr=u32DataAddr;
					/*Bundle buffer size to avoid over flow*/
					if(u8DataLength >= (ICDIAG_ICTX_BUFFERSIZE-1U))
					{
						I2CICDiagCtrl.u8ICTxLen=ICDIAG_ICTX_BUFFERSIZE;
					}
					else
					{
						I2CICDiagCtrl.u8ICTxLen=u8DataLength+1U;
					}
					/*I2C: First byte, data address*/
					I2CICDiagCtrl.u8ICTxBuf[0]=(uint8_t)((uint8_t)u32DataAddr&(uint8_t)0xFFU);
					/*I2C: Data bytes start from second byte*/
					for(i8Index=0; i8Index<((int8_t)u8DataLength); i8Index++)
					{
						I2CICDiagCtrl.u8ICTxBuf[i8Index+1]=*(u8DataBytes+i8Index);
					}
					ICDIAG_FillinDefaultRXData();
					ICDIAG_SetCmdState(ICDIAG_STATUS_PACKED);
					ICDIAG_SetCmdResault(ICDIAG_RESULT_NONE);
				}
				/*Pack MCU write or control package for driver API*/
				else if((u8ICType == ICDIAG_ICTYPE_MCU_MEM) || (u8ICType == ICDIAG_ICTYPE_MCU_NVM)  || (u8ICType == ICDIAG_ICTYPE_MCU_REG) )
				{
					I2CICDiagCtrl.u8ICType=u8ICType;
					I2CICDiagCtrl.u8Channel=u8Channel;
					I2CICDiagCtrl.u8DeviceAddr=u8DeviceAddr;
					I2CICDiagCtrl.u32DataAddr=u32DataAddr;
					I2CICDiagCtrl.u8ICTxLen=u8DataLength;
					/*Bundle buffer size to avoid over flow*/
					if(u8DataLength >= ICDIAG_ICTX_BUFFERSIZE)
					{
						I2CICDiagCtrl.u8ICTxLen=ICDIAG_ICTX_BUFFERSIZE;
					}
					else
					{
						I2CICDiagCtrl.u8ICTxLen=u8DataLength;
					}
					/*MCU: Data bytes start from first byte*/
					for(i8Index=0; i8Index<((int8_t)u8DataLength); i8Index++)
					{
						I2CICDiagCtrl.u8ICTxBuf[i8Index]=*(u8DataBytes+i8Index);
					}
					ICDIAG_FillinDefaultRXData();
					ICDIAG_SetCmdState(ICDIAG_STATUS_PACKED);
					ICDIAG_SetCmdResault(ICDIAG_RESULT_NONE);
				}
				/*Pack SPI write or control package(TBD!!!)*/
				else
				{/*Nothing*/}
			}
			else if(u8Cmd == ICDIAG_CMD_ICFETCH)
			{
				/*Pack I2C fetch package for driver API*/
				if(u8ICType == ICDIAG_ICTYPE_I2C)
				{
					I2CICDiagCtrl.u8ICType=u8ICType;
					I2CICDiagCtrl.u8Channel=u8Channel;
					I2CICDiagCtrl.u8DeviceAddr=u8DeviceAddr;
					I2CICDiagCtrl.u32DataAddr=u32DataAddr;
					I2CICDiagCtrl.u8ICTxLen=1U;
					I2CICDiagCtrl.u8ICTxBuf[0]=(uint8_t)((uint8_t)u32DataAddr&(uint8_t)0xFFU);
					I2CICDiagCtrl.u8ICRxLen=u8DataLength;
					ICDIAG_FillinDefaultRXData();
					ICDIAG_SetCmdState(ICDIAG_STATUS_PACKED);
					ICDIAG_SetCmdResault(ICDIAG_RESULT_NONE);
				}
				/*Pack MCU fetch package for driver API*/
				else if((u8ICType == ICDIAG_ICTYPE_MCU_MEM) || (u8ICType == ICDIAG_ICTYPE_MCU_NVM) || (u8ICType == ICDIAG_ICTYPE_MCU_REG))
				{
					I2CICDiagCtrl.u8ICType=u8ICType;
					I2CICDiagCtrl.u8Channel=u8Channel;
					I2CICDiagCtrl.u8DeviceAddr=u8DeviceAddr;
					I2CICDiagCtrl.u32DataAddr=u32DataAddr;
					I2CICDiagCtrl.u8ICRxLen=u8DataLength;
					ICDIAG_FillinDefaultRXData();
					ICDIAG_SetCmdState(ICDIAG_STATUS_PACKED);
					ICDIAG_SetCmdResault(ICDIAG_RESULT_NONE);
				}
				/*Pack SPI fetch package(TBD!!!)*/
				else
				{/*Nothing*/}
			}
			else
			{/*Nothing*/}
		}
		else
		{/*Nothing*/}
	}
	else
	{/*Nothing*/}

	(void)i8Index;
}

/**
 * @brief <b>It's the main process to handle fetch/write command.</b>
 * 
 * @details When state ICDIAG_STATUS_IDLE : Do nothing.\n
 * When state ICDIAG_STATUS_PACKED : Submit packed command to driver layer base on IC type.\n
 * When state ICDIAG_STATUS_SUBMITTED : Wait command result, driver should fill in ICDIAG_RESULT_SUCCESS / ICDIAG_RESULT_FAIL to notify process keep on.\n
 * 
 * @note Driver API depend on differnet project and MCU. Use compiler option to seperate.
 * 
 * @dot
 * digraph CommandState{ 
 *	"ICDIAG_STATUS_PACKED" -> "ICDIAG_STATUS_SUBMITTED" [label = "  When packed command submit to driver layer."];
 *	"ICDIAG_STATUS_SUBMITTED" -> "ICDIAG_STATUS_IDLE" [label = "  When command result set ICDIAG_RESULT_SUCCESS or ICDIAG_RESULT_FAIL by driver API."];
 *	"ICDIAG_STATUS_IDLE" -> "ICDIAG_STATUS_IDLE" [label = "  State remain before new command arrival."];
 * }
 * @enddot
 * 
 */
void ICDIAG_Main(void)
{
	switch(I2CICDiagCtrl.u8CommandState)
	{
		case ICDIAG_STATUS_PACKED:
			/*Use I2C driver API to submit fetch/write package*/
			if(I2CICDiagCtrl.u8ICType == ICDIAG_ICTYPE_I2C)
			{
#if ICDIAG_USE_GM_DRV_API
				I2cMasterApp_ICDiag(I2CICDiagCtrl.u8DeviceAddr, I2CICDiagCtrl.u8Channel, I2CICDiagCtrl.u8ICTxBuf, I2CICDiagCtrl.u8ICTxLen, I2CICDiagCtrl.u8ICRxBuf, I2CICDiagCtrl.u8ICRxLen);
#elif ICDIAG_USE_SGM_DRV_API
				ICDIAG_SetCmdResault(ICDIAG_RESULT_SUCCESS);
#elif ICDIAG_USE_FORD23P6_DRV_API
				ICDIAG_SetCmdResault(ICDIAG_RESULT_SUCCESS);
#elif ICDIAG_USE_FORD13P2_DRV_API
				MCUDIAG_I2cMaster_ICDiag(I2CICDiagCtrl.u8DeviceAddr, I2CICDiagCtrl.u8Channel, I2CICDiagCtrl.u8ICTxBuf, I2CICDiagCtrl.u8ICTxLen, I2CICDiagCtrl.u8ICRxBuf, I2CICDiagCtrl.u8ICRxLen);
#endif
			}
			/*Use MCU driver API to submit fetch/write package*/
			else if(I2CICDiagCtrl.u8ICType == ICDIAG_ICTYPE_MCU_MEM)
			{
#if ICDIAG_USE_GM_DRV_API
				MCUDIAG_MemRW(((uint8_t*)I2CICDiagCtrl.u32DataAddr), I2CICDiagCtrl.u8ICTxBuf, I2CICDiagCtrl.u8ICTxLen, I2CICDiagCtrl.u8ICRxBuf, I2CICDiagCtrl.u8ICRxLen);
#elif ICDIAG_USE_SGM_DRV_API
				ICDIAG_SetCmdResault(ICDIAG_RESULT_SUCCESS);
#elif ICDIAG_USE_FORD23P6_DRV_API
				ICDIAG_SetCmdResault(ICDIAG_RESULT_SUCCESS);
#elif ICDIAG_USE_FORD13P2_DRV_API
				MCUDIAG_MemRW(I2CICDiagCtrl.u32DataAddr, I2CICDiagCtrl.u8ICTxBuf, I2CICDiagCtrl.u8ICTxLen, I2CICDiagCtrl.u8ICRxBuf, I2CICDiagCtrl.u8ICRxLen);
#endif
			}
			/*Use MCU driver API to submit fetch/write package*/
			else if(I2CICDiagCtrl.u8ICType == ICDIAG_ICTYPE_MCU_NVM)
			{
#if ICDIAG_USE_GM_DRV_API
				MCUDIAG_NVMRW(I2CICDiagCtrl.u32DataAddr, I2CICDiagCtrl.u8ICTxBuf, I2CICDiagCtrl.u8ICTxLen, I2CICDiagCtrl.u8ICRxBuf, I2CICDiagCtrl.u8ICRxLen);
#elif ICDIAG_USE_SGM_DRV_API
				ICDIAG_SetCmdResault(ICDIAG_RESULT_SUCCESS);
#elif ICDIAG_USE_FORD23P6_DRV_API
				ICDIAG_SetCmdResault(ICDIAG_RESULT_SUCCESS);
#elif ICDIAG_USE_FORD13P2_DRV_API
				MCUDIAG_NVMRW(I2CICDiagCtrl.u32DataAddr, I2CICDiagCtrl.u8ICTxBuf, I2CICDiagCtrl.u8ICTxLen, I2CICDiagCtrl.u8ICRxBuf, I2CICDiagCtrl.u8ICRxLen);
#endif
			}
			/*Use SPI driver API to submit fetch/write package(TBD!!!)*/
			else if(I2CICDiagCtrl.u8ICType == ICDIAG_ICTYPE_SPI)
			{
#if ICDIAG_USE_GM_DRV_API
				ICDIAG_SetCmdResault(ICDIAG_RESULT_SUCCESS);
#elif ICDIAG_USE_SGM_DRV_API
				ICDIAG_SetCmdResault(ICDIAG_RESULT_SUCCESS);
#elif ICDIAG_USE_FORD23P6_DRV_API
				ICDIAG_SetCmdResault(ICDIAG_RESULT_SUCCESS);
#elif ICDIAG_USE_FORD13P2_DRV_API

#endif
			}
			else if(I2CICDiagCtrl.u8ICType == ICDIAG_ICTYPE_MCU_REG)
			{
#if ICDIAG_USE_FORD13P2_DRV_API
				MCUDIAG_RegRW(I2CICDiagCtrl.u32DataAddr, I2CICDiagCtrl.u8ICTxBuf, I2CICDiagCtrl.u8ICTxLen, I2CICDiagCtrl.u8ICRxBuf, I2CICDiagCtrl.u8ICRxLen);
#endif
			}
			else
			{
				ICDIAG_SetCmdResault(ICDIAG_RESULT_SUCCESS);
			}

			ICDIAG_SetCmdState(ICDIAG_STATUS_SUBMITTED);
		break;

		/*Check status after submit, when command complete go back to IDLE*/
		case ICDIAG_STATUS_SUBMITTED:
			if(I2CICDiagCtrl.u8CommandResult == ICDIAG_RESULT_FAIL)
			{
				ICDIAG_FillinDefaultRXData();
				ICDIAG_SetCmdState(ICDIAG_STATUS_IDLE);
			}
			else if(I2CICDiagCtrl.u8CommandResult == ICDIAG_RESULT_SUCCESS)
			{
				ICDIAG_SetCmdState(ICDIAG_STATUS_IDLE);
			}
			else
			{/*Nothing*/}
		break;

		case ICDIAG_STATUS_IDLE:
			ICDIAG_SetCmdResault(ICDIAG_RESULT_NONE);
		break;

		default:
			ICDIAG_SetCmdState(ICDIAG_STATUS_IDLE);
			ICDIAG_SetCmdResault(ICDIAG_RESULT_NONE);
		break;		  
	}
}

#endif

