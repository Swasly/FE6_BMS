/*******************************************************************************
* File Name: Can_Update_Timer_PM.c
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

#include "Can_Update_Timer.h"

static Can_Update_Timer_backupStruct Can_Update_Timer_backup;


/*******************************************************************************
* Function Name: Can_Update_Timer_SaveConfig
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
*  Can_Update_Timer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Can_Update_Timer_SaveConfig(void) 
{
    #if (!Can_Update_Timer_UsingFixedFunction)
        Can_Update_Timer_backup.TimerUdb = Can_Update_Timer_ReadCounter();
        Can_Update_Timer_backup.InterruptMaskValue = Can_Update_Timer_STATUS_MASK;
        #if (Can_Update_Timer_UsingHWCaptureCounter)
            Can_Update_Timer_backup.TimerCaptureCounter = Can_Update_Timer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Can_Update_Timer_UDB_CONTROL_REG_REMOVED)
            Can_Update_Timer_backup.TimerControlRegister = Can_Update_Timer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Can_Update_Timer_RestoreConfig
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
*  Can_Update_Timer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Can_Update_Timer_RestoreConfig(void) 
{   
    #if (!Can_Update_Timer_UsingFixedFunction)

        Can_Update_Timer_WriteCounter(Can_Update_Timer_backup.TimerUdb);
        Can_Update_Timer_STATUS_MASK =Can_Update_Timer_backup.InterruptMaskValue;
        #if (Can_Update_Timer_UsingHWCaptureCounter)
            Can_Update_Timer_SetCaptureCount(Can_Update_Timer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Can_Update_Timer_UDB_CONTROL_REG_REMOVED)
            Can_Update_Timer_WriteControlRegister(Can_Update_Timer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Can_Update_Timer_Sleep
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
*  Can_Update_Timer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Can_Update_Timer_Sleep(void) 
{
    #if(!Can_Update_Timer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Can_Update_Timer_CTRL_ENABLE == (Can_Update_Timer_CONTROL & Can_Update_Timer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Can_Update_Timer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Can_Update_Timer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Can_Update_Timer_Stop();
    Can_Update_Timer_SaveConfig();
}


/*******************************************************************************
* Function Name: Can_Update_Timer_Wakeup
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
*  Can_Update_Timer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Can_Update_Timer_Wakeup(void) 
{
    Can_Update_Timer_RestoreConfig();
    #if(!Can_Update_Timer_UDB_CONTROL_REG_REMOVED)
        if(Can_Update_Timer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Can_Update_Timer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
