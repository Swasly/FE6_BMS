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

#include <project.h>
#include <stdio.h>
#include "WDT.h"

/**
 * @brief flag to indicate if WDT should be cleared
 */
static volatile bool WDT_flag = false;

/**
 * @brief WDT timer ISR routine
 */
CY_ISR(WDT_int)
{
    WDT_flag = true;
}


void WDT_init() {
	CyWdtStart(CYWDT_1024_TICKS,CYWDT_LPMODE_NOCHANGE);
    WDT_flag = false;
    WDT_ISR_StartEx(WDT_int);
    WDT_timer_Start();
}

void WDT_clear() {
    WDT_flag = false;
	CyWdtClear();
}

bool WDT_should_clear() {
    return WDT_flag;
}

/* [] END OF FILE */
