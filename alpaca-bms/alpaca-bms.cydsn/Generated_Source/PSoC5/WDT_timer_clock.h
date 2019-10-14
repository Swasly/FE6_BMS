/*******************************************************************************
* File Name: WDT_timer_clock.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_WDT_timer_clock_H)
#define CY_CLOCK_WDT_timer_clock_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component cy_clock_v2_20 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void WDT_timer_clock_Start(void) ;
void WDT_timer_clock_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void WDT_timer_clock_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void WDT_timer_clock_StandbyPower(uint8 state) ;
void WDT_timer_clock_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 WDT_timer_clock_GetDividerRegister(void) ;
void WDT_timer_clock_SetModeRegister(uint8 modeBitMask) ;
void WDT_timer_clock_ClearModeRegister(uint8 modeBitMask) ;
uint8 WDT_timer_clock_GetModeRegister(void) ;
void WDT_timer_clock_SetSourceRegister(uint8 clkSource) ;
uint8 WDT_timer_clock_GetSourceRegister(void) ;
#if defined(WDT_timer_clock__CFG3)
void WDT_timer_clock_SetPhaseRegister(uint8 clkPhase) ;
uint8 WDT_timer_clock_GetPhaseRegister(void) ;
#endif /* defined(WDT_timer_clock__CFG3) */

#define WDT_timer_clock_Enable()                       WDT_timer_clock_Start()
#define WDT_timer_clock_Disable()                      WDT_timer_clock_Stop()
#define WDT_timer_clock_SetDivider(clkDivider)         WDT_timer_clock_SetDividerRegister(clkDivider, 1u)
#define WDT_timer_clock_SetDividerValue(clkDivider)    WDT_timer_clock_SetDividerRegister((clkDivider) - 1u, 1u)
#define WDT_timer_clock_SetMode(clkMode)               WDT_timer_clock_SetModeRegister(clkMode)
#define WDT_timer_clock_SetSource(clkSource)           WDT_timer_clock_SetSourceRegister(clkSource)
#if defined(WDT_timer_clock__CFG3)
#define WDT_timer_clock_SetPhase(clkPhase)             WDT_timer_clock_SetPhaseRegister(clkPhase)
#define WDT_timer_clock_SetPhaseValue(clkPhase)        WDT_timer_clock_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(WDT_timer_clock__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define WDT_timer_clock_CLKEN              (* (reg8 *) WDT_timer_clock__PM_ACT_CFG)
#define WDT_timer_clock_CLKEN_PTR          ((reg8 *) WDT_timer_clock__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define WDT_timer_clock_CLKSTBY            (* (reg8 *) WDT_timer_clock__PM_STBY_CFG)
#define WDT_timer_clock_CLKSTBY_PTR        ((reg8 *) WDT_timer_clock__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define WDT_timer_clock_DIV_LSB            (* (reg8 *) WDT_timer_clock__CFG0)
#define WDT_timer_clock_DIV_LSB_PTR        ((reg8 *) WDT_timer_clock__CFG0)
#define WDT_timer_clock_DIV_PTR            ((reg16 *) WDT_timer_clock__CFG0)

/* Clock MSB divider configuration register. */
#define WDT_timer_clock_DIV_MSB            (* (reg8 *) WDT_timer_clock__CFG1)
#define WDT_timer_clock_DIV_MSB_PTR        ((reg8 *) WDT_timer_clock__CFG1)

/* Mode and source configuration register */
#define WDT_timer_clock_MOD_SRC            (* (reg8 *) WDT_timer_clock__CFG2)
#define WDT_timer_clock_MOD_SRC_PTR        ((reg8 *) WDT_timer_clock__CFG2)

#if defined(WDT_timer_clock__CFG3)
/* Analog clock phase configuration register */
#define WDT_timer_clock_PHASE              (* (reg8 *) WDT_timer_clock__CFG3)
#define WDT_timer_clock_PHASE_PTR          ((reg8 *) WDT_timer_clock__CFG3)
#endif /* defined(WDT_timer_clock__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define WDT_timer_clock_CLKEN_MASK         WDT_timer_clock__PM_ACT_MSK
#define WDT_timer_clock_CLKSTBY_MASK       WDT_timer_clock__PM_STBY_MSK

/* CFG2 field masks */
#define WDT_timer_clock_SRC_SEL_MSK        WDT_timer_clock__CFG2_SRC_SEL_MASK
#define WDT_timer_clock_MODE_MASK          (~(WDT_timer_clock_SRC_SEL_MSK))

#if defined(WDT_timer_clock__CFG3)
/* CFG3 phase mask */
#define WDT_timer_clock_PHASE_MASK         WDT_timer_clock__CFG3_PHASE_DLY_MASK
#endif /* defined(WDT_timer_clock__CFG3) */

#endif /* CY_CLOCK_WDT_timer_clock_H */


/* [] END OF FILE */
