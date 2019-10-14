/*******************************************************************************
* File Name: ADC_current_PM.c
* Version 3.20
*
* Description:
*  This file provides the power manager source code to the API for the
*  Delta-Sigma ADC Component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "ADC_current.h"

static ADC_current_BACKUP_STRUCT ADC_current_backup =
{
    ADC_current_DISABLED,
    ADC_current_CFG1_DEC_CR
};


/*******************************************************************************
* Function Name: ADC_current_SaveConfig
********************************************************************************
*
* Summary:
*  Save the register configuration which are not retention.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  ADC_current_backup:  This global structure variable is used to store
*  configuration registers which are non retention whenever user wants to go
*  sleep mode by calling Sleep() API.
*
*******************************************************************************/
void ADC_current_SaveConfig(void) 
{
    ADC_current_backup.deccr = ADC_current_DEC_CR_REG;
}


/*******************************************************************************
* Function Name: ADC_current_RestoreConfig
********************************************************************************
*
* Summary:
*  Restore the register configurations which are not retention.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  ADC_current_backup:  This global structure variable is used to restore
*  configuration registers which are non retention whenever user wants to switch
*  to active power mode by calling Wakeup() API.
*
*******************************************************************************/
void ADC_current_RestoreConfig(void) 
{
    ADC_current_DEC_CR_REG = ADC_current_backup.deccr;
}


/*******************************************************************************
* Function Name: ADC_current_Sleep
********************************************************************************
*
* Summary:
*  Stops the operation of the block and saves the user configuration.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  ADC_current_backup:  The structure field 'enableState' is modified
*  depending on the enable state of the block before entering to sleep mode.
*
*******************************************************************************/
void ADC_current_Sleep(void) 
{
    /* Save ADC enable state */
    if((ADC_current_ACT_PWR_DEC_EN == (ADC_current_PWRMGR_DEC_REG & ADC_current_ACT_PWR_DEC_EN)) &&
       (ADC_current_ACT_PWR_DSM_EN == (ADC_current_PWRMGR_DSM_REG & ADC_current_ACT_PWR_DSM_EN)))
    {
        /* Component is enabled */
        ADC_current_backup.enableState = ADC_current_ENABLED;
        if((ADC_current_DEC_CR_REG & ADC_current_DEC_START_CONV) != 0u)
        {   
            /* Conversion is started */
            ADC_current_backup.enableState |= ADC_current_STARTED;
        }
		
        /* Stop the configuration */
        ADC_current_Stop();
    }
    else
    {
        /* Component is disabled */
        ADC_current_backup.enableState = ADC_current_DISABLED;
    }

    /* Save the user configuration */
    ADC_current_SaveConfig();
}


/*******************************************************************************
* Function Name: ADC_current_Wakeup
********************************************************************************
*
* Summary:
*  Restores the user configuration and enables the power to the block.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  ADC_current_backup:  The structure field 'enableState' is used to
*  restore the enable state of block after wakeup from sleep mode.
*
*******************************************************************************/
void ADC_current_Wakeup(void) 
{
    /* Restore the configuration */
    ADC_current_RestoreConfig();

    /* Enables the component operation */
    if(ADC_current_backup.enableState != ADC_current_DISABLED)
    {
        ADC_current_Enable();
        if((ADC_current_backup.enableState & ADC_current_STARTED) != 0u)
        {
            ADC_current_StartConvert();
        }
    } /* Do nothing if component was disable before */
}


/* [] END OF FILE */
