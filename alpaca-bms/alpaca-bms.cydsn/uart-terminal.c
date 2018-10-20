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

#include "uart-terminal.h"

 /**
  * @brief ASCII control code
  */
#define BEL (7)
#define BS (8)
#define DEL (127)
#define CR (13)
#define ESC (27)

#define TERMINAL_MAX_ROUTINE (10)
#define TERMINAL_ERROR_PROP "ERROR!!! "


static char* terminal_commandTable[TERMINAL_MAX_ROUTINE];
func_ptr_t terminal_routineTable[TERMINAL_MAX_ROUTINE];
static uint8_t terminal_currentCommandCount = 0;
static bool terminal_inited = false;
static bool terminal_error_flag = false;
static func_ptr_t currentFunc = NULL;

void terminal_parse(char serial_in[]);
bool terminal_helpFunc();
inline void terminal_printPrompt();


void terminal_init() {
    if (terminal_inited) {
        return;
    }
    
    USBUART_Start(0u, USBUART_3V_OPERATION);

    // char* help = "help\0";
    terminal_commandTable[0] = "help\0";

    terminal_routineTable[0] = &terminal_helpFunc;

    terminal_currentCommandCount = 1;
    terminal_inited = true;
}

uint8_t terminal_registerCommand(char newCommand[], func_ptr_t routine) {
    if (!terminal_inited) {
        terminal_init();
    }
    if (terminal_currentCommandCount >= TERMINAL_MAX_ROUTINE) {
        //Error handling when the vector table is full
        return 0;
    }
    terminal_commandTable[terminal_currentCommandCount] = newCommand;
    terminal_routineTable[terminal_currentCommandCount] = routine;
    return terminal_currentCommandCount++;
}

void terminal_executeCommand(uint8_t routineID) {
    if(USBUART_IsConfigurationChanged() != 0u) /* Host could send double SET_INTERFACE request */
    {
        if(USBUART_GetConfiguration() != 0u)   /* Init IN endpoints when device configured */
        {
            /* Enumeration is done, enable OUT endpoint for receive data from Host */
            USBUART_CDC_Init();
        }
    }

    if(USBUART_GetConfiguration() != 0u)    /* Service USB CDC when device configured */
    {
        if(USBUART_DataIsReady() != 0u)               /* Check for input data from PC */
        {
            if (routineID >= terminal_currentCommandCount) {
                return;
            }
            if ((*terminal_routineTable[routineID])()) {
                currentFunc = NULL;
                terminal_printPrompt();
            } else {
                currentFunc = terminal_routineTable[routineID];
            }
        }
    } else {
        currentFunc = NULL;
    }
}

bool terminal_helpFunc() {
    while(USBUART_CDCIsReady() == 0u);
    USBUART_PutChar('\n');
    uint8_t i = 0;
    for (i = 0; i < terminal_currentCommandCount; i++) {
        while(USBUART_CDCIsReady() == 0u);
        USBUART_PutChar(i + '0');
        while(USBUART_CDCIsReady() == 0u);
        USBUART_PutString(". ");
        while(USBUART_CDCIsReady() == 0u);
        USBUART_PutString(terminal_commandTable[i]);
        while(USBUART_CDCIsReady() == 0u);
        USBUART_PutString("\n");
    }
    return true;
}

/****************************************************************************
* Function Name: terminal_echo(char serial_in[], uint8* track)
*****************************************************************************
*
* Summary:
*  Echos back to the terminal the current key stroke. If enter key is pressed,
*  this function will call terminal_parse() to get the inputed command
*
* Parameters:
*  serial_in[]: array to store the inputs, array of chars
*  track: pointer to track which keeps track of the number of inputs i.e. chars
*
* Return:
*  None.
*
****************************************************************************/
void terminal_run()
{
    static uint8_t track = 0;
    static char serial_in[100];

    uint8 buffer = 0;
    uint8_t buf[100];

    buffer = 0;

    if(USBUART_IsConfigurationChanged() != 0u) /* Host could send double SET_INTERFACE request */
    {
        if(USBUART_GetConfiguration() != 0u)   /* Init IN endpoints when device configured */
        {
            /* Enumeration is done, enable OUT endpoint for receive data from Host */
            USBUART_CDC_Init();
        }
    }
    if(USBUART_GetConfiguration() != 0u)    /* Service USB CDC when device configured */
    {
        if (currentFunc != NULL) {
            if ((*currentFunc)()) {
                terminal_printPrompt();
                currentFunc = NULL;
            }
            return;
        }
        if(USBUART_DataIsReady() != 0u)               /* Check for input data from PC */
        {
            uint16_t count = USBUART_GetAll(buf);
            uint16_t i = 0;
            for (i = 0; i < count; i++) {
                buffer = buf[i];
                serial_in[track] = (char)buffer;
                while(USBUART_CDCIsReady() == 0u);    /* Wait till component is ready to send more data to the PC */
                USBUART_PutChar((char)buffer);       /* Send data back to PC */

                if(buffer == 127)           // checks for backspace = 127
                {
                    if ((track) != 0) {        // subtract only if not at the front of the array
                        (track)--;
                        while(USBUART_CDCIsReady() == 0u);
                        USBUART_PutString("\b \b");
                    } else {
                        terminal_ringBell();
                    }
                }
                else
                    (track)++;

                if((char)buffer == '\r')        // if enter key was pressed
                {
                    serial_in[track-1] = '\0';
                    terminal_parse(serial_in);
                    track = 0;
                    break;
                }
            }
        }
    } else {
        currentFunc = NULL;
    }
}


void terminal_ringBell() {
    while(USBUART_CDCIsReady() == 0u);
    USBUART_PutChar(BEL); 
}

bool terminal_detectESC() {
    if(USBUART_IsConfigurationChanged() != 0u) /* Host could send double SET_INTERFACE request */
    {
        if(USBUART_GetConfiguration() != 0u)   /* Init IN endpoints when device configured */
        {
            return true;
        }
    }
    static uint8_t buf[100];
    if (USBUART_GetConfiguration() != 0u) {
        if (USBUART_DataIsReady() != 0u) {
            uint16_t count = USBUART_GetAll(buf);
            uint16_t i = 0;
            for (i = 0; i < count; i++) {
                if (buf[i] == ESC) {
                    return true;
                }
            }
        }
        return false;
    }
    return true;
}

/****************************************************************************
* Function Name: terminal_parse(char serial_in[])
*****************************************************************************
*
* Summary:
*  Called when the user press enter. Takes the serial_in[] array and compares
*  it with hard coded commands.
*
* Parameters:
*  serial_in[]: array of inputs
*
* Return:
*  None.
*
****************************************************************************/
void terminal_parse(char serial_in[])
{
    uint8_t i = 0;
    for (i = 0; i < terminal_currentCommandCount; i++) {
        char iStr[2];
        iStr[0] = i + '0';
        iStr[1] = '\0';
        if (stricmp(serial_in, terminal_commandTable[i]) == 0 ||  stricmp(serial_in, iStr) == 0) {
            while(USBUART_CDCIsReady() == 0u);
            USBUART_PutChar('\n');
            if ((*terminal_routineTable[i])()) {
                currentFunc = NULL;
                terminal_printPrompt();
            } else {
                currentFunc = terminal_routineTable[i];
            }
            return; 
        }
    }

    if (stricmp(serial_in, "") != 0) {
        while (USBUART_CDCIsReady() == 0u);
        USBUART_PutString("$");
        while (USBUART_CDCIsReady() == 0u);
        USBUART_PutString(serial_in);
        while(USBUART_CDCIsReady() == 0u);    /* Wait till component is ready to send more data to the PC */
        USBUART_PutString("$ is not a recognized command, please use 'help' to see commands.");       /* Send data to PC */
    }
    terminal_printPrompt();
}

inline void terminal_printPrompt() {
    while(USBUART_CDCIsReady() == 0u);
    USBUART_PutString("\n");
    if (terminal_error_flag) {
        while(USBUART_CDCIsReady() == 0u);
        USBUART_PutString(TERMINAL_ERROR_PROP);
    }
    while(USBUART_CDCIsReady() == 0u);
    USBUART_PutString(TERMINAL_NODE_NAME);
    while(USBUART_CDCIsReady() == 0u);
    USBUART_PutString("> ");
}


void terminal_setErrorFlag() {
    terminal_error_flag = true;
}

void terminal_clearErrorFlag() {
    terminal_error_flag = false;
}

/* [] END OF FILE */
