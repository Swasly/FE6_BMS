/*******************************************************************************
* File Name: WDT_timer_PM.c
* Version 2.70
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "WDT_timer.h"

static WDT_timer_backupStruct WDT_timer_backup;


/*******************************************************************************
* Function Name: WDT_timer_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  WDT_timer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void WDT_timer_SaveConfig(void) 
{
    #if (!WDT_timer_UsingFixedFunction)
        WDT_timer_backup.TimerUdb = WDT_timer_ReadCounter();
        WDT_timer_backup.InterruptMaskValue = WDT_timer_STATUS_MASK;
        #if (WDT_timer_UsingHWCaptureCounter)
            WDT_timer_backup.TimerCaptureCounter = WDT_timer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!WDT_timer_UDB_CONTROL_REG_REMOVED)
            WDT_timer_backup.TimerControlRegister = WDT_timer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: WDT_timer_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  WDT_timer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void WDT_timer_RestoreConfig(void) 
{   
    #if (!WDT_timer_UsingFixedFunction)

        WDT_timer_WriteCounter(WDT_timer_backup.TimerUdb);
        WDT_timer_STATUS_MASK =WDT_timer_backup.InterruptMaskValue;
        #if (WDT_timer_UsingHWCaptureCounter)
            WDT_timer_SetCaptureCount(WDT_timer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!WDT_timer_UDB_CONTROL_REG_REMOVED)
            WDT_timer_WriteControlRegister(WDT_timer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: WDT_timer_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  WDT_timer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void WDT_timer_Sleep(void) 
{
    #if(!WDT_timer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(WDT_timer_CTRL_ENABLE == (WDT_timer_CONTROL & WDT_timer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            WDT_timer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            WDT_timer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    WDT_timer_Stop();
    WDT_timer_SaveConfig();
}


/*******************************************************************************
* Function Name: WDT_timer_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  WDT_timer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void WDT_timer_Wakeup(void) 
{
    WDT_timer_RestoreConfig();
    #if(!WDT_timer_UDB_CONTROL_REG_REMOVED)
        if(WDT_timer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                WDT_timer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
