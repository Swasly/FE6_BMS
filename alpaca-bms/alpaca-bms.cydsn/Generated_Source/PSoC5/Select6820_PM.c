/*******************************************************************************
* File Name: Select6820_PM.c
* Version 1.80
*
* Description:
*  This file contains the setup, control, and status commands to support 
*  the component operation in the low power mode. 
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "Select6820.h"

/* Check for removal by optimization */
#if !defined(Select6820_Sync_ctrl_reg__REMOVED)

static Select6820_BACKUP_STRUCT  Select6820_backup = {0u};

    
/*******************************************************************************
* Function Name: Select6820_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Select6820_SaveConfig(void) 
{
    Select6820_backup.controlState = Select6820_Control;
}


/*******************************************************************************
* Function Name: Select6820_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*
*******************************************************************************/
void Select6820_RestoreConfig(void) 
{
     Select6820_Control = Select6820_backup.controlState;
}


/*******************************************************************************
* Function Name: Select6820_Sleep
********************************************************************************
*
* Summary:
*  Prepares the component for entering the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Select6820_Sleep(void) 
{
    Select6820_SaveConfig();
}


/*******************************************************************************
* Function Name: Select6820_Wakeup
********************************************************************************
*
* Summary:
*  Restores the component after waking up from the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Select6820_Wakeup(void)  
{
    Select6820_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
