/*******************************************************************************
* File Name: current_timer_PM.c
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

#include "current_timer.h"

static current_timer_backupStruct current_timer_backup;


/*******************************************************************************
* Function Name: current_timer_SaveConfig
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
*  current_timer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void current_timer_SaveConfig(void) 
{
    #if (!current_timer_UsingFixedFunction)
        current_timer_backup.TimerUdb = current_timer_ReadCounter();
        current_timer_backup.InterruptMaskValue = current_timer_STATUS_MASK;
        #if (current_timer_UsingHWCaptureCounter)
            current_timer_backup.TimerCaptureCounter = current_timer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!current_timer_UDB_CONTROL_REG_REMOVED)
            current_timer_backup.TimerControlRegister = current_timer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: current_timer_RestoreConfig
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
*  current_timer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void current_timer_RestoreConfig(void) 
{   
    #if (!current_timer_UsingFixedFunction)

        current_timer_WriteCounter(current_timer_backup.TimerUdb);
        current_timer_STATUS_MASK =current_timer_backup.InterruptMaskValue;
        #if (current_timer_UsingHWCaptureCounter)
            current_timer_SetCaptureCount(current_timer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!current_timer_UDB_CONTROL_REG_REMOVED)
            current_timer_WriteControlRegister(current_timer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: current_timer_Sleep
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
*  current_timer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void current_timer_Sleep(void) 
{
    #if(!current_timer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(current_timer_CTRL_ENABLE == (current_timer_CONTROL & current_timer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            current_timer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            current_timer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    current_timer_Stop();
    current_timer_SaveConfig();
}


/*******************************************************************************
* Function Name: current_timer_Wakeup
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
*  current_timer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void current_timer_Wakeup(void) 
{
    current_timer_RestoreConfig();
    #if(!current_timer_UDB_CONTROL_REG_REMOVED)
        if(current_timer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                current_timer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
