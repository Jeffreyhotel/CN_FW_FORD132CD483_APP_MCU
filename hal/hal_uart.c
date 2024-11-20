/******************************************************************************
;				Program		:	hal_uart.h
;				Function	:	SysTick Function
;				Chip		:	Cypress CY8C4149AZI-S598
;				Date		:	2021 / 10 / 28
;				Author		:
;				Describe	:
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
#include "hal_uart.h"

static debug_uart_tydef tUART;

/******************************************************************************
;       Function Name			:	void M_DU_Init(void)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
void HAL_UART_Init(debug_uart_tydef tDebugUart)
{
	tUART.pBase = tDebugUart.pBase;
	tUART.pConfig = tDebugUart.pConfig;

	/* Configure and enable the UART peripheral */
	(void)Cy_SCB_UART_Init(tUART.pBase, tUART.pConfig, &tUART.tContext);
	(void)Cy_SCB_UART_Enable(tUART.pBase);
}
/******************************************************************************
;       Function Name			:	void M_DU_DeInit(void)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
void HAL_UART_DeInit(debug_uart_tydef tDebugUart)
{
	tUART.pBase = tDebugUart.pBase;
	tUART.pConfig = tDebugUart.pConfig;


	/* Configure and enable the UART peripheral */
	(void)Cy_SCB_UART_DeInit(tUART.pBase);
}
/******************************************************************************
;       Function Name			:	void M_DU_DeInit(void)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
void HAL_UART_Printf(void *fmt, ...)
{
#ifdef DEBUG_UART_EN
    uint16_t u16Timeout = 0;
    static char u8PrintfTextBuf[MAX_TEXT_LEN + 8u];

    va_list arg;

    /* UART Print */
    va_start(arg, fmt);

    vsnprintf (u8PrintfTextBuf, MAX_TEXT_LEN, (const char *) fmt, arg);

    for ( u16Timeout = 0 ; u16Timeout <= PRINTF_TIMEOUT ; u16Timeout++)
    {
        if (true == Cy_SCB_UART_IsTxComplete(tUART.pBase))
        {
            break;
        }
        else
        {
        	Cy_SysLib_Delay(1);
        }
    }

    if (u16Timeout < PRINTF_TIMEOUT)
    {
        Cy_SCB_UART_PutString(tUART.pBase , (char_t const*)u8PrintfTextBuf);
    }

    va_end(arg);
#else

#endif

}
