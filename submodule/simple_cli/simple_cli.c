/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <ctype.h>
#include "hal_uart.h"
#include "simple_cli.h"


#define CLEAR_SCREEN() HAL_UART_Printf("\033[1J" SET_CURSOR_0_0)
#define PROMPT_STR() HAL_UART_Printf("[FIDM]$ ")
#define MAX_BUF_CHAR 64
#define CONFIG_SYS_MAXARGS		16	/* max number of command args	*/
#define ZERO_CMD_BUF() {g_ch_pos = 0 ; memset(g_line_buf,0x00,sizeof(g_line_buf));}

static char g_line_buf[MAX_BUF_CHAR];
static uint16_t g_ch_pos = 0;
static SCLI_initCB g_scb;
static CMD* g_cmd_table;
static int g_size_of_cmd_table = 0;

void SCLI_Init(SCLI_initCB* init_scb, CMD* ptab, int table_size)
{
    g_scb.scb_start   = init_scb->scb_start; 
    g_scb.scb_putchar = init_scb->scb_putchar;
    g_scb.scb_getchar = init_scb->scb_getchar;
    
    //(*g_scb.scb_start)(); // start
    //init_iprintf( (pfn_iprint_PutChar)g_scb.scb_putchar );
    ZERO_CMD_BUF();
    g_cmd_table = ptab;
    g_size_of_cmd_table = table_size;
}

static int make_argv(char *s, int argvsz, char *argv[])
{
	int argc = 0;

	/* split into argv */
	while (argc < argvsz - 1) {

		/* skip any white space */
		while (isblank(*s))
			++s;

		if (*s == '\0')	/* end of s, no more args	*/
			break;

		argv[argc++] = s;	/* begin of argument string	*/

		/* find end of string */
		while (*s && !isblank(*s))
			++s;

		if (*s == '\0')		/* end of s, no more args	*/
			break;

		*s++ = '\0';		/* terminate current arg	 */
	}
	argv[argc] = NULL;

	return argc;
}

static int exec_command()
{
    char *argv[CONFIG_SYS_MAXARGS + 1];		/* NULL terminated	*/
    static char tmp_buf[MAX_BUF_CHAR + 1];	/* copy of console I/O buffer	*/
    int argc = 0;
    int cmd_find = 0;
    int result = -1;
    if ( g_ch_pos != 0)
    {
        //auo_iprintf("-->Exec command %s\r\n",g_line_buf);
        //memset(tmp_buf,0x00,sizeof(tmp_buf));

        /* copy to secondary buffer which will be affected */
	    strcpy(tmp_buf, g_line_buf);
         /* separate into argv */
        argc = make_argv(tmp_buf, sizeof(argv)/sizeof(argv[0]), argv);
        for (int i = 0 ; i < g_size_of_cmd_table ; i++)
        {
            if ( 0 == strcmp (g_cmd_table[i].name , argv[0]) )
            {
                cmd_find = 1;      
                result = (*g_cmd_table[i].lpfCmd)(argc,argv);
            }
        }
        if (0 == cmd_find)
        	HAL_UART_Printf("%s:command not found.\r\n",g_line_buf);
            
        ZERO_CMD_BUF();
    }
    
    PROMPT_STR();
    return result;
}

void SCLI_Run()
{
	uint32_t u32RxNum = 0U;

	u32RxNum = Cy_SCB_UART_GetNumInRxFifo(DEBUG_PORT);
	if( 0  !=  u32RxNum)
	{
		char ch = (*g_scb.scb_getchar)();
		if (ch != 0)
		{
			if (ch == '\r')
			{

				if (g_ch_pos > 0U)
				{
					g_line_buf[g_ch_pos++] = '\0';
				}
				else
				{
					; /* don't insert '\0', if only press "enter" key */
				}

				(*g_scb.scb_putchar)('\r');
				(*g_scb.scb_putchar)('\n');

				exec_command();
			}
			else
			{
				if (g_ch_pos < MAX_BUF_CHAR)
				{
					g_line_buf[g_ch_pos++] = ch;
					(*g_scb.scb_putchar)(ch);
				}
				else
				{
					; /* TODO: overflow */
				}
			}

		}
	}
	else
	{
		; /* do nothing */
	}

}


uint32_t UART_GetChar(void)
{
	return Cy_SCB_UART_Get(DEBUG_PORT);
}

void UART_PutChar(uint32_t ch)
{
	uint16_t u16Timeout;

	while(true != Cy_SCB_UART_IsTxComplete(DEBUG_PORT));

	for ( u16Timeout = 0 ; u16Timeout <= PRINTF_TIMEOUT ; u16Timeout++)
	{
		if (true == Cy_SCB_UART_IsTxComplete(DEBUG_PORT))
		{
			break;
		}
		else
		{
			Cy_SysLib_Delay(1);
		}
	}
	Cy_SCB_UART_Put(DEBUG_PORT, ch);
}
    
/* [] END OF FILE */
