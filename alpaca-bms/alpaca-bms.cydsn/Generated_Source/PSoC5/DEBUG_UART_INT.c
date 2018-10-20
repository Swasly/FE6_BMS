/*******************************************************************************
* File Name: DEBUG_UARTINT.c
* Version 2.50
*
* Description:
*  This file provides all Interrupt Service functionality of the UART component
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "DEBUG_UART.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (DEBUG_UART_RX_INTERRUPT_ENABLED && (DEBUG_UART_RX_ENABLED || DEBUG_UART_HD_ENABLED))
    /*******************************************************************************
    * Function Name: DEBUG_UART_RXISR
    ********************************************************************************
    *
    * Summary:
    *  Interrupt Service Routine for RX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  DEBUG_UART_rxBuffer - RAM buffer pointer for save received data.
    *  DEBUG_UART_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  DEBUG_UART_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  DEBUG_UART_rxBufferOverflow - software overflow flag. Set to one
    *     when DEBUG_UART_rxBufferWrite index overtakes
    *     DEBUG_UART_rxBufferRead index.
    *  DEBUG_UART_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when DEBUG_UART_rxBufferWrite is equal to
    *    DEBUG_UART_rxBufferRead
    *  DEBUG_UART_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  DEBUG_UART_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(DEBUG_UART_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

        /* User code required at start of ISR */
        /* `#START DEBUG_UART_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = DEBUG_UART_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in DEBUG_UART_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (DEBUG_UART_RX_STS_BREAK | 
                            DEBUG_UART_RX_STS_PAR_ERROR |
                            DEBUG_UART_RX_STS_STOP_ERROR | 
                            DEBUG_UART_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                DEBUG_UART_errorStatus |= readStatus & ( DEBUG_UART_RX_STS_BREAK | 
                                                            DEBUG_UART_RX_STS_PAR_ERROR | 
                                                            DEBUG_UART_RX_STS_STOP_ERROR | 
                                                            DEBUG_UART_RX_STS_OVERRUN);
                /* `#START DEBUG_UART_RXISR_ERROR` */

                /* `#END` */
            }
            
            if((readStatus & DEBUG_UART_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = DEBUG_UART_RXDATA_REG;
            #if (DEBUG_UART_RXHW_ADDRESS_ENABLED)
                if(DEBUG_UART_rxAddressMode == (uint8)DEBUG_UART__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & DEBUG_UART_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & DEBUG_UART_RX_STS_ADDR_MATCH) != 0u)
                        {
                            DEBUG_UART_rxAddressDetected = 1u;
                        }
                        else
                        {
                            DEBUG_UART_rxAddressDetected = 0u;
                        }
                    }
                    if(DEBUG_UART_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        DEBUG_UART_rxBuffer[DEBUG_UART_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    DEBUG_UART_rxBuffer[DEBUG_UART_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                DEBUG_UART_rxBuffer[DEBUG_UART_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (DEBUG_UART_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(DEBUG_UART_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        DEBUG_UART_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    DEBUG_UART_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(DEBUG_UART_rxBufferWrite >= DEBUG_UART_RX_BUFFER_SIZE)
                    {
                        DEBUG_UART_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(DEBUG_UART_rxBufferWrite == DEBUG_UART_rxBufferRead)
                    {
                        DEBUG_UART_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (DEBUG_UART_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            DEBUG_UART_RXSTATUS_MASK_REG  &= (uint8)~DEBUG_UART_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(DEBUG_UART_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (DEBUG_UART_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & DEBUG_UART_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START DEBUG_UART_RXISR_END` */

        /* `#END` */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }

#endif /* (DEBUG_UART_RX_INTERRUPT_ENABLED && (DEBUG_UART_RX_ENABLED || DEBUG_UART_HD_ENABLED)) */


#if (DEBUG_UART_TX_INTERRUPT_ENABLED && DEBUG_UART_TX_ENABLED)
    /*******************************************************************************
    * Function Name: DEBUG_UART_TXISR
    ********************************************************************************
    *
    * Summary:
    * Interrupt Service Routine for the TX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  DEBUG_UART_txBuffer - RAM buffer pointer for transmit data from.
    *  DEBUG_UART_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  DEBUG_UART_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(DEBUG_UART_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

        /* User code required at start of ISR */
        /* `#START DEBUG_UART_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((DEBUG_UART_txBufferRead != DEBUG_UART_txBufferWrite) &&
             ((DEBUG_UART_TXSTATUS_REG & DEBUG_UART_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(DEBUG_UART_txBufferRead >= DEBUG_UART_TX_BUFFER_SIZE)
            {
                DEBUG_UART_txBufferRead = 0u;
            }

            DEBUG_UART_TXDATA_REG = DEBUG_UART_txBuffer[DEBUG_UART_txBufferRead];

            /* Set next pointer */
            DEBUG_UART_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START DEBUG_UART_TXISR_END` */

        /* `#END` */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }

#endif /* (DEBUG_UART_TX_INTERRUPT_ENABLED && DEBUG_UART_TX_ENABLED) */


/* [] END OF FILE */
