/*******************************************************************************
* File Name: LTC68_INT.c
* Version 2.50
*
* Description:
*  This file provides all Interrupt Service Routine (ISR) for the SPI Master
*  component.
*
* Note:
*  None.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "LTC68_PVT.h"


/* User code required at start of ISR */
/* `#START LTC68_ISR_START_DEF` */

/* `#END` */


/*******************************************************************************
* Function Name: LTC68_TX_ISR
********************************************************************************
*
* Summary:
*  Interrupt Service Routine for TX portion of the SPI Master.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  LTC68_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer.
*  LTC68_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer, modified when exist data to
*  sending and FIFO Not Full.
*  LTC68_txBuffer[LTC68_TX_BUFFER_SIZE] - used to store
*  data to sending.
*  All described above Global variables are used when Software Buffer is used.
*
*******************************************************************************/
CY_ISR(LTC68_TX_ISR)
{
    #if(LTC68_TX_SOFTWARE_BUF_ENABLED)
        uint8 tmpStatus;
    #endif /* (LTC68_TX_SOFTWARE_BUF_ENABLED) */

    #ifdef LTC68_TX_ISR_ENTRY_CALLBACK
        LTC68_TX_ISR_EntryCallback();
    #endif /* LTC68_TX_ISR_ENTRY_CALLBACK */

    /* User code required at start of ISR */
    /* `#START LTC68_TX_ISR_START` */

    /* `#END` */
    
    #if(LTC68_TX_SOFTWARE_BUF_ENABLED)
        /* Check if TX data buffer is not empty and there is space in TX FIFO */
        while(LTC68_txBufferRead != LTC68_txBufferWrite)
        {
            tmpStatus = LTC68_GET_STATUS_TX(LTC68_swStatusTx);
            LTC68_swStatusTx = tmpStatus;

            if(0u != (LTC68_swStatusTx & LTC68_STS_TX_FIFO_NOT_FULL))
            {
                if(0u == LTC68_txBufferFull)
                {
                   LTC68_txBufferRead++;

                    if(LTC68_txBufferRead >= LTC68_TX_BUFFER_SIZE)
                    {
                        LTC68_txBufferRead = 0u;
                    }
                }
                else
                {
                    LTC68_txBufferFull = 0u;
                }

                /* Put data element into the TX FIFO */
                CY_SET_REG8(LTC68_TXDATA_PTR, 
                                             LTC68_txBuffer[LTC68_txBufferRead]);
            }
            else
            {
                break;
            }
        }

        if(LTC68_txBufferRead == LTC68_txBufferWrite)
        {
            /* TX Buffer is EMPTY: disable interrupt on TX NOT FULL */
            LTC68_TX_STATUS_MASK_REG &= ((uint8) ~LTC68_STS_TX_FIFO_NOT_FULL);
        }

    #endif /* (LTC68_TX_SOFTWARE_BUF_ENABLED) */

    /* User code required at end of ISR (Optional) */
    /* `#START LTC68_TX_ISR_END` */

    /* `#END` */
    
    #ifdef LTC68_TX_ISR_EXIT_CALLBACK
        LTC68_TX_ISR_ExitCallback();
    #endif /* LTC68_TX_ISR_EXIT_CALLBACK */
}


/*******************************************************************************
* Function Name: LTC68_RX_ISR
********************************************************************************
*
* Summary:
*  Interrupt Service Routine for RX portion of the SPI Master.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  LTC68_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer modified when FIFO contains
*  new data.
*  LTC68_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified when overflow occurred.
*  LTC68_rxBuffer[LTC68_RX_BUFFER_SIZE] - used to store
*  received data, modified when FIFO contains new data.
*  All described above Global variables are used when Software Buffer is used.
*
*******************************************************************************/
CY_ISR(LTC68_RX_ISR)
{
    #if(LTC68_RX_SOFTWARE_BUF_ENABLED)
        uint8 tmpStatus;
        uint8 rxData;
    #endif /* (LTC68_RX_SOFTWARE_BUF_ENABLED) */

    #ifdef LTC68_RX_ISR_ENTRY_CALLBACK
        LTC68_RX_ISR_EntryCallback();
    #endif /* LTC68_RX_ISR_ENTRY_CALLBACK */

    /* User code required at start of ISR */
    /* `#START LTC68_RX_ISR_START` */

    /* `#END` */
    
    #if(LTC68_RX_SOFTWARE_BUF_ENABLED)

        tmpStatus = LTC68_GET_STATUS_RX(LTC68_swStatusRx);
        LTC68_swStatusRx = tmpStatus;

        /* Check if RX data FIFO has some data to be moved into the RX Buffer */
        while(0u != (LTC68_swStatusRx & LTC68_STS_RX_FIFO_NOT_EMPTY))
        {
            rxData = CY_GET_REG8(LTC68_RXDATA_PTR);

            /* Set next pointer. */
            LTC68_rxBufferWrite++;
            if(LTC68_rxBufferWrite >= LTC68_RX_BUFFER_SIZE)
            {
                LTC68_rxBufferWrite = 0u;
            }

            if(LTC68_rxBufferWrite == LTC68_rxBufferRead)
            {
                LTC68_rxBufferRead++;
                if(LTC68_rxBufferRead >= LTC68_RX_BUFFER_SIZE)
                {
                    LTC68_rxBufferRead = 0u;
                }

                LTC68_rxBufferFull = 1u;
            }

            /* Move data from the FIFO to the Buffer */
            LTC68_rxBuffer[LTC68_rxBufferWrite] = rxData;

            tmpStatus = LTC68_GET_STATUS_RX(LTC68_swStatusRx);
            LTC68_swStatusRx = tmpStatus;
        }

    #endif /* (LTC68_RX_SOFTWARE_BUF_ENABLED) */

    /* User code required at end of ISR (Optional) */
    /* `#START LTC68_RX_ISR_END` */

    /* `#END` */
    
    #ifdef LTC68_RX_ISR_EXIT_CALLBACK
        LTC68_RX_ISR_ExitCallback();
    #endif /* LTC68_RX_ISR_EXIT_CALLBACK */
}

/* [] END OF FILE */
