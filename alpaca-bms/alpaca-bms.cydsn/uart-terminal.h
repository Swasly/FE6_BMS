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
#ifndef UART_TERMINAL_H
#define UART_TERMINAL_H

#include <project.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define BUFFER_LEN 64
#define TERMINAL_NODE_NAME "Pedal-Node"

/**
 * @brief R
 * @details return true to end execution print a prompt, return false to keep running the current routing in the next frame
 * 
 * @param t [description]
 * @return [description]
 */
typedef bool (*func_ptr_t)( void );

void terminal_init();
void terminal_run();
uint8_t terminal_registerCommand(char command[], func_ptr_t routine);
void terminal_executeCommand(uint8_t routineID);
void terminal_ringBell();
bool terminal_detectESC();

/**
 * Error indication
 */
void terminal_setErrorFlag();
void terminal_clearErrorFlag();

#endif


/* [] END OF FILE */
