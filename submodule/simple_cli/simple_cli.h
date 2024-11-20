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
#ifndef SIMPLE_CLI_H___
#define SIMPLE_CLI_H___
    

    
#define UNUSED(x) ( (void)(x) )


// vt100 command
#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define LIGHT_RED "\033[1;31m"
#define GREEN "\033[0;32;32m"
#define LIGHT_GREEN "\033[1;32m"
#define BLUE "\033[0;32;34m"
#define LIGHT_BLUE "\033[1;34m"
#define DARY_GRAY "\033[1;30m"
#define CYAN "\033[0;36m"
#define LIGHT_CYAN "\033[1;36m"
#define PURPLE "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN "\033[0;33m"
#define YELLOW "\033[1;33m"
#define LIGHT_GRAY "\033[0;37m"
#define WHITE "\033[1;37m"
#define SET_CURSOR_0_0 "\033[0;0H"
    
typedef void (*pfnSCB_Start)(void);
typedef uint32_t (*pfnSCB_GetChar)(void);
typedef void (*pfnSCB_PutChar)(uint32_t ch);

typedef int (*fpCMD_FUN) ( int argc , char * argv []);

typedef struct __cmd_s
{
	char*       name;
	fpCMD_FUN   lpfCmd;
} CMD;

typedef struct _SCLI_initCB
{
	pfnSCB_Start scb_start;
	pfnSCB_GetChar scb_getchar;
	pfnSCB_PutChar scb_putchar;
}SCLI_initCB;

void SCLI_Init(SCLI_initCB* init_scb,CMD* ptab,int table_size);
void SCLI_Run();
uint32_t UART_GetChar(void);
void UART_PutChar(uint32_t);

#endif
/* [] END OF FILE */
