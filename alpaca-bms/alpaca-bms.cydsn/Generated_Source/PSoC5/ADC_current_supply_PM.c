/*******************************************************************************
* File Name: ADC_current_supply_PM.c
* Version 3.0
*
* Description:
*  This file provides Sleep/WakeUp APIs functionality.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "ADC_current_supply.h"


/***************************************
* Local data allocation
***************************************/

static ADC_current_supply_BACKUP_STRUCT  ADC_current_supply_backup =
{
    ADC_current_supply_DISABLED
};


/*******************************************************************************
* Function Name: ADC_current_supply_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void ADC_current_supply_SaveConfig(void)
{
    /* All configuration registers are marked as [reset_all_retention] */
}


/*******************************************************************************
* Function Name: ADC_current_supply_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void ADC_current_supply_RestoreConfig(void)
{
    /* All congiguration registers are marked as [reset_all_retention] */
}


/*******************************************************************************
* Function Name: ADC_current_supply_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred routine to prepare the component for sleep.
*  The ADC_current_supply_Sleep() routine saves the current component state,
*  then it calls the ADC_Stop() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  ADC_current_supply_backup - The structure field 'enableState' is modified
*  depending on the enable state of the block before entering to sleep mode.
*
*******************************************************************************/
void ADC_current_supply_Sleep(void)
{
    if((ADC_current_supply_PWRMGR_SAR_REG  & ADC_current_supply_ACT_PWR_SAR_EN) != 0u)
    {
        if((ADC_current_supply_SAR_CSR0_REG & ADC_current_supply_SAR_SOF_START_CONV) != 0u)
        {
            ADC_current_supply_backup.enableState = ADC_current_supply_ENABLED | ADC_current_supply_STARTED;
        }
        else
        {
            ADC_current_supply_backup.enableState = ADC_current_supply_ENABLED;
        }
        ADC_current_supply_Stop();
    }
    else
    {
        ADC_current_supply_backup.enableState = ADC_current_supply_DISABLED;
    }
}


/*******************************************************************************
* Function Name: ADC_current_supply_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred routine to restore the component to the state when
*  ADC_current_supply_Sleep() was called. If the component was enabled before the
*  ADC_current_supply_Sleep() function was called, the
*  ADC_current_supply_Wakeup() function also re-enables the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  ADC_current_supply_backup - The structure field 'enableState' is used to
*  restore the enable state of block after wakeup from sleep mode.
*
*******************************************************************************/
void ADC_current_supply_Wakeup(void)
{
    if(ADC_current_supply_backup.enableState != ADC_current_supply_DISABLED)
    {
        ADC_current_supply_Enable();
        #if(ADC_current_supply_DEFAULT_CONV_MODE != ADC_current_supply__HARDWARE_TRIGGER)
            if((ADC_current_supply_backup.enableState & ADC_current_supply_STARTED) != 0u)
            {
                ADC_current_supply_StartConvert();
            }
        #endif /* End ADC_current_supply_DEFAULT_CONV_MODE != ADC_current_supply__HARDWARE_TRIGGER */
    }
}


/* [] END OF FILE */
