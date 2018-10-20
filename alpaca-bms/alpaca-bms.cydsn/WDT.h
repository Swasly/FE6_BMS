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

#ifndef WDT_H
#define WDT_H

#include <stdbool.h>

/**
 * @brief Initialize watchdog timer
 * @details Do neceesary setup for WDT, start a timer which trigger once per second
 * to inform the main program to clear WDT flag
 */
void WDT_init();

/**
 * @brief Clear WDT flag
 * @details [long description]
 */
void WDT_clear();

/**
 * @brief If 1 seconds is reached since last time WDT is cleared
 * @details WDT reset the board if WDT flag is not cleared in 2.3 seconds, we
 * clear the WDT once per seconds
 * @return bool to indicate if 1 sec is reached since last time WDT is cleared
 */
bool WDT_should_clear();

#endif

/* [] END OF FILE */
