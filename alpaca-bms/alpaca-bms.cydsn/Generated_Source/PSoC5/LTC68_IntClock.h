/*******************************************************************************
* File Name: LTC68_IntClock.h
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

#if !defined(CY_CLOCK_LTC68_IntClock_H)
#define CY_CLOCK_LTC68_IntClock_H

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

void LTC68_IntClock_Start(void) ;
void LTC68_IntClock_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void LTC68_IntClock_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void LTC68_IntClock_StandbyPower(uint8 state) ;
void LTC68_IntClock_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 LTC68_IntClock_GetDividerRegister(void) ;
void LTC68_IntClock_SetModeRegister(uint8 modeBitMask) ;
void LTC68_IntClock_ClearModeRegister(uint8 modeBitMask) ;
uint8 LTC68_IntClock_GetModeRegister(void) ;
void LTC68_IntClock_SetSourceRegister(uint8 clkSource) ;
uint8 LTC68_IntClock_GetSourceRegister(void) ;
#if defined(LTC68_IntClock__CFG3)
void LTC68_IntClock_SetPhaseRegister(uint8 clkPhase) ;
uint8 LTC68_IntClock_GetPhaseRegister(void) ;
#endif /* defined(LTC68_IntClock__CFG3) */

#define LTC68_IntClock_Enable()                       LTC68_IntClock_Start()
#define LTC68_IntClock_Disable()                      LTC68_IntClock_Stop()
#define LTC68_IntClock_SetDivider(clkDivider)         LTC68_IntClock_SetDividerRegister(clkDivider, 1u)
#define LTC68_IntClock_SetDividerValue(clkDivider)    LTC68_IntClock_SetDividerRegister((clkDivider) - 1u, 1u)
#define LTC68_IntClock_SetMode(clkMode)               LTC68_IntClock_SetModeRegister(clkMode)
#define LTC68_IntClock_SetSource(clkSource)           LTC68_IntClock_SetSourceRegister(clkSource)
#if defined(LTC68_IntClock__CFG3)
#define LTC68_IntClock_SetPhase(clkPhase)             LTC68_IntClock_SetPhaseRegister(clkPhase)
#define LTC68_IntClock_SetPhaseValue(clkPhase)        LTC68_IntClock_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(LTC68_IntClock__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define LTC68_IntClock_CLKEN              (* (reg8 *) LTC68_IntClock__PM_ACT_CFG)
#define LTC68_IntClock_CLKEN_PTR          ((reg8 *) LTC68_IntClock__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define LTC68_IntClock_CLKSTBY            (* (reg8 *) LTC68_IntClock__PM_STBY_CFG)
#define LTC68_IntClock_CLKSTBY_PTR        ((reg8 *) LTC68_IntClock__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define LTC68_IntClock_DIV_LSB            (* (reg8 *) LTC68_IntClock__CFG0)
#define LTC68_IntClock_DIV_LSB_PTR        ((reg8 *) LTC68_IntClock__CFG0)
#define LTC68_IntClock_DIV_PTR            ((reg16 *) LTC68_IntClock__CFG0)

/* Clock MSB divider configuration register. */
#define LTC68_IntClock_DIV_MSB            (* (reg8 *) LTC68_IntClock__CFG1)
#define LTC68_IntClock_DIV_MSB_PTR        ((reg8 *) LTC68_IntClock__CFG1)

/* Mode and source configuration register */
#define LTC68_IntClock_MOD_SRC            (* (reg8 *) LTC68_IntClock__CFG2)
#define LTC68_IntClock_MOD_SRC_PTR        ((reg8 *) LTC68_IntClock__CFG2)

#if defined(LTC68_IntClock__CFG3)
/* Analog clock phase configuration register */
#define LTC68_IntClock_PHASE              (* (reg8 *) LTC68_IntClock__CFG3)
#define LTC68_IntClock_PHASE_PTR          ((reg8 *) LTC68_IntClock__CFG3)
#endif /* defined(LTC68_IntClock__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define LTC68_IntClock_CLKEN_MASK         LTC68_IntClock__PM_ACT_MSK
#define LTC68_IntClock_CLKSTBY_MASK       LTC68_IntClock__PM_STBY_MSK

/* CFG2 field masks */
#define LTC68_IntClock_SRC_SEL_MSK        LTC68_IntClock__CFG2_SRC_SEL_MASK
#define LTC68_IntClock_MODE_MASK          (~(LTC68_IntClock_SRC_SEL_MSK))

#if defined(LTC68_IntClock__CFG3)
/* CFG3 phase mask */
#define LTC68_IntClock_PHASE_MASK         LTC68_IntClock__CFG3_PHASE_DLY_MASK
#endif /* defined(LTC68_IntClock__CFG3) */

#endif /* CY_CLOCK_LTC68_IntClock_H */


/* [] END OF FILE */
