/*******************************************************************************
* File Name: LTC68_PM.c
* Version 2.50
*
* Description:
*  This file contains the setup, control and status commands to support
*  component operations in low power mode.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "LTC68_PVT.h"

static LTC68_BACKUP_STRUCT LTC68_backup =
{
    LTC68_DISABLED,
    LTC68_BITCTR_INIT,
};


/*******************************************************************************
* Function Name: LTC68_SaveConfig
********************************************************************************
*
* Summary:
*  Empty function. Included for consistency with other components.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void LTC68_SaveConfig(void) 
{

}


/*******************************************************************************
* Function Name: LTC68_RestoreConfig
********************************************************************************
*
* Summary:
*  Empty function. Included for consistency with other components.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void LTC68_RestoreConfig(void) 
{

}


/*******************************************************************************
* Function Name: LTC68_Sleep
********************************************************************************
*
* Summary:
*  Prepare SPIM Component goes to sleep.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  LTC68_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LTC68_Sleep(void) 
{
    /* Save components enable state */
    LTC68_backup.enableState = ((uint8) LTC68_IS_ENABLED);

    LTC68_Stop();
}


/*******************************************************************************
* Function Name: LTC68_Wakeup
********************************************************************************
*
* Summary:
*  Prepare SPIM Component to wake up.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  LTC68_backup - used when non-retention registers are restored.
*  LTC68_txBufferWrite - modified every function call - resets to
*  zero.
*  LTC68_txBufferRead - modified every function call - resets to
*  zero.
*  LTC68_rxBufferWrite - modified every function call - resets to
*  zero.
*  LTC68_rxBufferRead - modified every function call - resets to
*  zero.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LTC68_Wakeup(void) 
{
    #if(LTC68_RX_SOFTWARE_BUF_ENABLED)
        LTC68_rxBufferFull  = 0u;
        LTC68_rxBufferRead  = 0u;
        LTC68_rxBufferWrite = 0u;
    #endif /* (LTC68_RX_SOFTWARE_BUF_ENABLED) */

    #if(LTC68_TX_SOFTWARE_BUF_ENABLED)
        LTC68_txBufferFull  = 0u;
        LTC68_txBufferRead  = 0u;
        LTC68_txBufferWrite = 0u;
    #endif /* (LTC68_TX_SOFTWARE_BUF_ENABLED) */

    /* Clear any data from the RX and TX FIFO */
    LTC68_ClearFIFO();

    /* Restore components block enable state */
    if(0u != LTC68_backup.enableState)
    {
        LTC68_Enable();
    }
}


/* [] END OF FILE */
