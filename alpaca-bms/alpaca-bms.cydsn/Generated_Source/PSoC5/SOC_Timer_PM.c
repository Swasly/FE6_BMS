/*******************************************************************************
* File Name: SOC_Timer_PM.c
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

#include "SOC_Timer.h"

static SOC_Timer_backupStruct SOC_Timer_backup;


/*******************************************************************************
* Function Name: SOC_Timer_SaveConfig
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
*  SOC_Timer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void SOC_Timer_SaveConfig(void) 
{
    #if (!SOC_Timer_UsingFixedFunction)
        SOC_Timer_backup.TimerUdb = SOC_Timer_ReadCounter();
        SOC_Timer_backup.InterruptMaskValue = SOC_Timer_STATUS_MASK;
        #if (SOC_Timer_UsingHWCaptureCounter)
            SOC_Timer_backup.TimerCaptureCounter = SOC_Timer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!SOC_Timer_UDB_CONTROL_REG_REMOVED)
            SOC_Timer_backup.TimerControlRegister = SOC_Timer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: SOC_Timer_RestoreConfig
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
*  SOC_Timer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void SOC_Timer_RestoreConfig(void) 
{   
    #if (!SOC_Timer_UsingFixedFunction)

        SOC_Timer_WriteCounter(SOC_Timer_backup.TimerUdb);
        SOC_Timer_STATUS_MASK =SOC_Timer_backup.InterruptMaskValue;
        #if (SOC_Timer_UsingHWCaptureCounter)
            SOC_Timer_SetCaptureCount(SOC_Timer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!SOC_Timer_UDB_CONTROL_REG_REMOVED)
            SOC_Timer_WriteControlRegister(SOC_Timer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: SOC_Timer_Sleep
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
*  SOC_Timer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void SOC_Timer_Sleep(void) 
{
    #if(!SOC_Timer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(SOC_Timer_CTRL_ENABLE == (SOC_Timer_CONTROL & SOC_Timer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            SOC_Timer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            SOC_Timer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    SOC_Timer_Stop();
    SOC_Timer_SaveConfig();
}


/*******************************************************************************
* Function Name: SOC_Timer_Wakeup
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
*  SOC_Timer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void SOC_Timer_Wakeup(void) 
{
    SOC_Timer_RestoreConfig();
    #if(!SOC_Timer_UDB_CONTROL_REG_REMOVED)
        if(SOC_Timer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                SOC_Timer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
