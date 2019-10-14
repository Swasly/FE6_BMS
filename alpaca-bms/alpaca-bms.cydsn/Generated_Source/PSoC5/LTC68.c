/*******************************************************************************
* File Name: LTC68.c
* Version 2.50
*
* Description:
*  This file provides all API functionality of the SPI Master component.
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

#if(LTC68_TX_SOFTWARE_BUF_ENABLED)
    volatile uint8 LTC68_txBuffer[LTC68_TX_BUFFER_SIZE];
    volatile uint8 LTC68_txBufferFull;
    volatile uint8 LTC68_txBufferRead;
    volatile uint8 LTC68_txBufferWrite;
#endif /* (LTC68_TX_SOFTWARE_BUF_ENABLED) */

#if(LTC68_RX_SOFTWARE_BUF_ENABLED)
    volatile uint8 LTC68_rxBuffer[LTC68_RX_BUFFER_SIZE];
    volatile uint8 LTC68_rxBufferFull;
    volatile uint8 LTC68_rxBufferRead;
    volatile uint8 LTC68_rxBufferWrite;
#endif /* (LTC68_RX_SOFTWARE_BUF_ENABLED) */

uint8 LTC68_initVar = 0u;

volatile uint8 LTC68_swStatusTx;
volatile uint8 LTC68_swStatusRx;


/*******************************************************************************
* Function Name: LTC68_Init
********************************************************************************
*
* Summary:
*  Inits/Restores default SPIM configuration provided with customizer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  When this function is called it initializes all of the necessary parameters
*  for execution. i.e. setting the initial interrupt mask, configuring the
*  interrupt service routine, configuring the bit-counter parameters and
*  clearing the FIFO and Status Register.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LTC68_Init(void) 
{
    /* Initialize the Bit counter */
    LTC68_COUNTER_PERIOD_REG = LTC68_BITCTR_INIT;

    /* Init TX ISR  */
    #if(0u != LTC68_INTERNAL_TX_INT_ENABLED)
        CyIntDisable         (LTC68_TX_ISR_NUMBER);
        CyIntSetPriority     (LTC68_TX_ISR_NUMBER,  LTC68_TX_ISR_PRIORITY);
        (void) CyIntSetVector(LTC68_TX_ISR_NUMBER, &LTC68_TX_ISR);
    #endif /* (0u != LTC68_INTERNAL_TX_INT_ENABLED) */

    /* Init RX ISR  */
    #if(0u != LTC68_INTERNAL_RX_INT_ENABLED)
        CyIntDisable         (LTC68_RX_ISR_NUMBER);
        CyIntSetPriority     (LTC68_RX_ISR_NUMBER,  LTC68_RX_ISR_PRIORITY);
        (void) CyIntSetVector(LTC68_RX_ISR_NUMBER, &LTC68_RX_ISR);
    #endif /* (0u != LTC68_INTERNAL_RX_INT_ENABLED) */

    /* Clear any stray data from the RX and TX FIFO */
    LTC68_ClearFIFO();

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

    (void) LTC68_ReadTxStatus(); /* Clear Tx status and swStatusTx */
    (void) LTC68_ReadRxStatus(); /* Clear Rx status and swStatusRx */

    /* Configure TX and RX interrupt mask */
    LTC68_TX_STATUS_MASK_REG = LTC68_TX_INIT_INTERRUPTS_MASK;
    LTC68_RX_STATUS_MASK_REG = LTC68_RX_INIT_INTERRUPTS_MASK;
}


/*******************************************************************************
* Function Name: LTC68_Enable
********************************************************************************
*
* Summary:
*  Enable SPIM component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void LTC68_Enable(void) 
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();
    LTC68_COUNTER_CONTROL_REG |= LTC68_CNTR_ENABLE;
    LTC68_TX_STATUS_ACTL_REG  |= LTC68_INT_ENABLE;
    LTC68_RX_STATUS_ACTL_REG  |= LTC68_INT_ENABLE;
    CyExitCriticalSection(enableInterrupts);

    #if(0u != LTC68_INTERNAL_CLOCK)
        LTC68_IntClock_Enable();
    #endif /* (0u != LTC68_INTERNAL_CLOCK) */

    LTC68_EnableTxInt();
    LTC68_EnableRxInt();
}


/*******************************************************************************
* Function Name: LTC68_Start
********************************************************************************
*
* Summary:
*  Initialize and Enable the SPI Master component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  LTC68_initVar - used to check initial configuration, modified on
*  first function call.
*
* Theory:
*  Enable the clock input to enable operation.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LTC68_Start(void) 
{
    if(0u == LTC68_initVar)
    {
        LTC68_Init();
        LTC68_initVar = 1u;
    }

    LTC68_Enable();
}


/*******************************************************************************
* Function Name: LTC68_Stop
********************************************************************************
*
* Summary:
*  Disable the SPI Master component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the clock input to enable operation.
*
*******************************************************************************/
void LTC68_Stop(void) 
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();
    LTC68_TX_STATUS_ACTL_REG &= ((uint8) ~LTC68_INT_ENABLE);
    LTC68_RX_STATUS_ACTL_REG &= ((uint8) ~LTC68_INT_ENABLE);
    CyExitCriticalSection(enableInterrupts);

    #if(0u != LTC68_INTERNAL_CLOCK)
        LTC68_IntClock_Disable();
    #endif /* (0u != LTC68_INTERNAL_CLOCK) */

    LTC68_DisableTxInt();
    LTC68_DisableRxInt();
}


/*******************************************************************************
* Function Name: LTC68_EnableTxInt
********************************************************************************
*
* Summary:
*  Enable internal Tx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Enable the internal Tx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void LTC68_EnableTxInt(void) 
{
    #if(0u != LTC68_INTERNAL_TX_INT_ENABLED)
        CyIntEnable(LTC68_TX_ISR_NUMBER);
    #endif /* (0u != LTC68_INTERNAL_TX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: LTC68_EnableRxInt
********************************************************************************
*
* Summary:
*  Enable internal Rx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Enable the internal Rx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void LTC68_EnableRxInt(void) 
{
    #if(0u != LTC68_INTERNAL_RX_INT_ENABLED)
        CyIntEnable(LTC68_RX_ISR_NUMBER);
    #endif /* (0u != LTC68_INTERNAL_RX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: LTC68_DisableTxInt
********************************************************************************
*
* Summary:
*  Disable internal Tx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the internal Tx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void LTC68_DisableTxInt(void) 
{
    #if(0u != LTC68_INTERNAL_TX_INT_ENABLED)
        CyIntDisable(LTC68_TX_ISR_NUMBER);
    #endif /* (0u != LTC68_INTERNAL_TX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: LTC68_DisableRxInt
********************************************************************************
*
* Summary:
*  Disable internal Rx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the internal Rx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void LTC68_DisableRxInt(void) 
{
    #if(0u != LTC68_INTERNAL_RX_INT_ENABLED)
        CyIntDisable(LTC68_RX_ISR_NUMBER);
    #endif /* (0u != LTC68_INTERNAL_RX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: LTC68_SetTxInterruptMode
********************************************************************************
*
* Summary:
*  Configure which status bits trigger an interrupt event.
*
* Parameters:
*  intSrc: An or'd combination of the desired status bit masks (defined in the
*  header file).
*
* Return:
*  None.
*
* Theory:
*  Enables the output of specific status bits to the interrupt controller.
*
*******************************************************************************/
void LTC68_SetTxInterruptMode(uint8 intSrc) 
{
    LTC68_TX_STATUS_MASK_REG = intSrc;
}


/*******************************************************************************
* Function Name: LTC68_SetRxInterruptMode
********************************************************************************
*
* Summary:
*  Configure which status bits trigger an interrupt event.
*
* Parameters:
*  intSrc: An or'd combination of the desired status bit masks (defined in the
*  header file).
*
* Return:
*  None.
*
* Theory:
*  Enables the output of specific status bits to the interrupt controller.
*
*******************************************************************************/
void LTC68_SetRxInterruptMode(uint8 intSrc) 
{
    LTC68_RX_STATUS_MASK_REG  = intSrc;
}


/*******************************************************************************
* Function Name: LTC68_ReadTxStatus
********************************************************************************
*
* Summary:
*  Read the Tx status register for the component.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the Tx status register.
*
* Global variables:
*  LTC68_swStatusTx - used to store in software status register,
*  modified every function call - resets to zero.
*
* Theory:
*  Allows the user and the API to read the Tx status register for error
*  detection and flow control.
*
* Side Effects:
*  Clear Tx status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 LTC68_ReadTxStatus(void) 
{
    uint8 tmpStatus;

    #if(LTC68_TX_SOFTWARE_BUF_ENABLED)
        /* Disable TX interrupt to protect global veriables */
        LTC68_DisableTxInt();

        tmpStatus = LTC68_GET_STATUS_TX(LTC68_swStatusTx);
        LTC68_swStatusTx = 0u;

        LTC68_EnableTxInt();

    #else

        tmpStatus = LTC68_TX_STATUS_REG;

    #endif /* (LTC68_TX_SOFTWARE_BUF_ENABLED) */

    return(tmpStatus);
}


/*******************************************************************************
* Function Name: LTC68_ReadRxStatus
********************************************************************************
*
* Summary:
*  Read the Rx status register for the component.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the Rx status register.
*
* Global variables:
*  LTC68_swStatusRx - used to store in software Rx status register,
*  modified every function call - resets to zero.
*
* Theory:
*  Allows the user and the API to read the Rx status register for error
*  detection and flow control.
*
* Side Effects:
*  Clear Rx status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 LTC68_ReadRxStatus(void) 
{
    uint8 tmpStatus;

    #if(LTC68_RX_SOFTWARE_BUF_ENABLED)
        /* Disable RX interrupt to protect global veriables */
        LTC68_DisableRxInt();

        tmpStatus = LTC68_GET_STATUS_RX(LTC68_swStatusRx);
        LTC68_swStatusRx = 0u;

        LTC68_EnableRxInt();

    #else

        tmpStatus = LTC68_RX_STATUS_REG;

    #endif /* (LTC68_RX_SOFTWARE_BUF_ENABLED) */

    return(tmpStatus);
}


/*******************************************************************************
* Function Name: LTC68_WriteTxData
********************************************************************************
*
* Summary:
*  Write a byte of data to be sent across the SPI.
*
* Parameters:
*  txDataByte: The data value to send across the SPI.
*
* Return:
*  None.
*
* Global variables:
*  LTC68_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer, modified every function
*  call if TX Software Buffer is used.
*  LTC68_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer.
*  LTC68_txBuffer[LTC68_TX_BUFFER_SIZE] - used to store
*  data to sending, modified every function call if TX Software Buffer is used.
*
* Theory:
*  Allows the user to transmit any byte of data in a single transfer.
*
* Side Effects:
*  If this function is called again before the previous byte is finished then
*  the next byte will be appended to the transfer with no time between
*  the byte transfers. Clear Tx status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LTC68_WriteTxData(uint8 txData) 
{
    #if(LTC68_TX_SOFTWARE_BUF_ENABLED)

        uint8 tempStatus;
        uint8 tmpTxBufferRead;

        /* Block if TX buffer is FULL: don't overwrite */
        do
        {
            tmpTxBufferRead = LTC68_txBufferRead;
            if(0u == tmpTxBufferRead)
            {
                tmpTxBufferRead = (LTC68_TX_BUFFER_SIZE - 1u);
            }
            else
            {
                tmpTxBufferRead--;
            }

        }while(tmpTxBufferRead == LTC68_txBufferWrite);

        /* Disable TX interrupt to protect global veriables */
        LTC68_DisableTxInt();

        tempStatus = LTC68_GET_STATUS_TX(LTC68_swStatusTx);
        LTC68_swStatusTx = tempStatus;


        if((LTC68_txBufferRead == LTC68_txBufferWrite) &&
           (0u != (LTC68_swStatusTx & LTC68_STS_TX_FIFO_NOT_FULL)))
        {
            /* Put data element into the TX FIFO */
            CY_SET_REG8(LTC68_TXDATA_PTR, txData);
        }
        else
        {
            /* Add to the TX software buffer */
            LTC68_txBufferWrite++;
            if(LTC68_txBufferWrite >= LTC68_TX_BUFFER_SIZE)
            {
                LTC68_txBufferWrite = 0u;
            }

            if(LTC68_txBufferWrite == LTC68_txBufferRead)
            {
                LTC68_txBufferRead++;
                if(LTC68_txBufferRead >= LTC68_TX_BUFFER_SIZE)
                {
                    LTC68_txBufferRead = 0u;
                }
                LTC68_txBufferFull = 1u;
            }

            LTC68_txBuffer[LTC68_txBufferWrite] = txData;

            LTC68_TX_STATUS_MASK_REG |= LTC68_STS_TX_FIFO_NOT_FULL;
        }

        LTC68_EnableTxInt();

    #else
        /* Wait until TX FIFO has a place */
        while(0u == (LTC68_TX_STATUS_REG & LTC68_STS_TX_FIFO_NOT_FULL))
        {
        }

        /* Put data element into the TX FIFO */
        CY_SET_REG8(LTC68_TXDATA_PTR, txData);

    #endif /* (LTC68_TX_SOFTWARE_BUF_ENABLED) */
}


/*******************************************************************************
* Function Name: LTC68_ReadRxData
********************************************************************************
*
* Summary:
*  Read the next byte of data received across the SPI.
*
* Parameters:
*  None.
*
* Return:
*  The next byte of data read from the FIFO.
*
* Global variables:
*  LTC68_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer.
*  LTC68_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified every function
*  call if RX Software Buffer is used.
*  LTC68_rxBuffer[LTC68_RX_BUFFER_SIZE] - used to store
*  received data.
*
* Theory:
*  Allows the user to read a byte of data received.
*
* Side Effects:
*  Will return invalid data if the FIFO is empty. The user should Call
*  GetRxBufferSize() and if it returns a non-zero value then it is safe to call
*  ReadByte() function.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 LTC68_ReadRxData(void) 
{
    uint8 rxData;

    #if(LTC68_RX_SOFTWARE_BUF_ENABLED)

        /* Disable RX interrupt to protect global veriables */
        LTC68_DisableRxInt();

        if(LTC68_rxBufferRead != LTC68_rxBufferWrite)
        {
            if(0u == LTC68_rxBufferFull)
            {
                LTC68_rxBufferRead++;
                if(LTC68_rxBufferRead >= LTC68_RX_BUFFER_SIZE)
                {
                    LTC68_rxBufferRead = 0u;
                }
            }
            else
            {
                LTC68_rxBufferFull = 0u;
            }
        }

        rxData = LTC68_rxBuffer[LTC68_rxBufferRead];

        LTC68_EnableRxInt();

    #else

        rxData = CY_GET_REG8(LTC68_RXDATA_PTR);

    #endif /* (LTC68_RX_SOFTWARE_BUF_ENABLED) */

    return(rxData);
}


/*******************************************************************************
* Function Name: LTC68_GetRxBufferSize
********************************************************************************
*
* Summary:
*  Returns the number of bytes/words of data currently held in the RX buffer.
*  If RX Software Buffer not used then function return 0 if FIFO empty or 1 if
*  FIFO not empty. In another case function return size of RX Software Buffer.
*
* Parameters:
*  None.
*
* Return:
*  Integer count of the number of bytes/words in the RX buffer.
*
* Global variables:
*  LTC68_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer.
*  LTC68_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer.
*
* Side Effects:
*  Clear status register of the component.
*
*******************************************************************************/
uint8 LTC68_GetRxBufferSize(void) 
{
    uint8 size;

    #if(LTC68_RX_SOFTWARE_BUF_ENABLED)

        /* Disable RX interrupt to protect global veriables */
        LTC68_DisableRxInt();

        if(LTC68_rxBufferRead == LTC68_rxBufferWrite)
        {
            size = 0u;
        }
        else if(LTC68_rxBufferRead < LTC68_rxBufferWrite)
        {
            size = (LTC68_rxBufferWrite - LTC68_rxBufferRead);
        }
        else
        {
            size = (LTC68_RX_BUFFER_SIZE - LTC68_rxBufferRead) + LTC68_rxBufferWrite;
        }

        LTC68_EnableRxInt();

    #else

        /* We can only know if there is data in the RX FIFO */
        size = (0u != (LTC68_RX_STATUS_REG & LTC68_STS_RX_FIFO_NOT_EMPTY)) ? 1u : 0u;

    #endif /* (LTC68_TX_SOFTWARE_BUF_ENABLED) */

    return(size);
}


/*******************************************************************************
* Function Name: LTC68_GetTxBufferSize
********************************************************************************
*
* Summary:
*  Returns the number of bytes/words of data currently held in the TX buffer.
*  If TX Software Buffer not used then function return 0 - if FIFO empty, 1 - if
*  FIFO not full, 4 - if FIFO full. In another case function return size of TX
*  Software Buffer.
*
* Parameters:
*  None.
*
* Return:
*  Integer count of the number of bytes/words in the TX buffer.
*
* Global variables:
*  LTC68_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer.
*  LTC68_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer.
*
* Side Effects:
*  Clear status register of the component.
*
*******************************************************************************/
uint8  LTC68_GetTxBufferSize(void) 
{
    uint8 size;

    #if(LTC68_TX_SOFTWARE_BUF_ENABLED)
        /* Disable TX interrupt to protect global veriables */
        LTC68_DisableTxInt();

        if(LTC68_txBufferRead == LTC68_txBufferWrite)
        {
            size = 0u;
        }
        else if(LTC68_txBufferRead < LTC68_txBufferWrite)
        {
            size = (LTC68_txBufferWrite - LTC68_txBufferRead);
        }
        else
        {
            size = (LTC68_TX_BUFFER_SIZE - LTC68_txBufferRead) + LTC68_txBufferWrite;
        }

        LTC68_EnableTxInt();

    #else

        size = LTC68_TX_STATUS_REG;

        if(0u != (size & LTC68_STS_TX_FIFO_EMPTY))
        {
            size = 0u;
        }
        else if(0u != (size & LTC68_STS_TX_FIFO_NOT_FULL))
        {
            size = 1u;
        }
        else
        {
            size = LTC68_FIFO_SIZE;
        }

    #endif /* (LTC68_TX_SOFTWARE_BUF_ENABLED) */

    return(size);
}


/*******************************************************************************
* Function Name: LTC68_ClearRxBuffer
********************************************************************************
*
* Summary:
*  Clear the RX RAM buffer by setting the read and write pointers both to zero.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  LTC68_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer, modified every function
*  call - resets to zero.
*  LTC68_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified every function call -
*  resets to zero.
*
* Theory:
*  Setting the pointers to zero makes the system believe there is no data to
*  read and writing will resume at address 0 overwriting any data that may have
*  remained in the RAM.
*
* Side Effects:
*  Any received data not read from the RAM buffer will be lost when overwritten.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LTC68_ClearRxBuffer(void) 
{
    /* Clear Hardware RX FIFO */
    while(0u !=(LTC68_RX_STATUS_REG & LTC68_STS_RX_FIFO_NOT_EMPTY))
    {
        (void) CY_GET_REG8(LTC68_RXDATA_PTR);
    }

    #if(LTC68_RX_SOFTWARE_BUF_ENABLED)
        /* Disable RX interrupt to protect global veriables */
        LTC68_DisableRxInt();

        LTC68_rxBufferFull  = 0u;
        LTC68_rxBufferRead  = 0u;
        LTC68_rxBufferWrite = 0u;

        LTC68_EnableRxInt();
    #endif /* (LTC68_RX_SOFTWARE_BUF_ENABLED) */
}


/*******************************************************************************
* Function Name: LTC68_ClearTxBuffer
********************************************************************************
*
* Summary:
*  Clear the TX RAM buffer by setting the read and write pointers both to zero.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  LTC68_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer, modified every function
*  call - resets to zero.
*  LTC68_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer, modified every function call -
*  resets to zero.
*
* Theory:
*  Setting the pointers to zero makes the system believe there is no data to
*  read and writing will resume at address 0 overwriting any data that may have
*  remained in the RAM.
*
* Side Effects:
*  Any data not yet transmitted from the RAM buffer will be lost when
*  overwritten.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LTC68_ClearTxBuffer(void) 
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();
    /* Clear TX FIFO */
    LTC68_AUX_CONTROL_DP0_REG |= ((uint8)  LTC68_TX_FIFO_CLR);
    LTC68_AUX_CONTROL_DP0_REG &= ((uint8) ~LTC68_TX_FIFO_CLR);

    #if(LTC68_USE_SECOND_DATAPATH)
        /* Clear TX FIFO for 2nd Datapath */
        LTC68_AUX_CONTROL_DP1_REG |= ((uint8)  LTC68_TX_FIFO_CLR);
        LTC68_AUX_CONTROL_DP1_REG &= ((uint8) ~LTC68_TX_FIFO_CLR);
    #endif /* (LTC68_USE_SECOND_DATAPATH) */
    CyExitCriticalSection(enableInterrupts);

    #if(LTC68_TX_SOFTWARE_BUF_ENABLED)
        /* Disable TX interrupt to protect global veriables */
        LTC68_DisableTxInt();

        LTC68_txBufferFull  = 0u;
        LTC68_txBufferRead  = 0u;
        LTC68_txBufferWrite = 0u;

        /* Buffer is EMPTY: disable TX FIFO NOT FULL interrupt */
        LTC68_TX_STATUS_MASK_REG &= ((uint8) ~LTC68_STS_TX_FIFO_NOT_FULL);

        LTC68_EnableTxInt();
    #endif /* (LTC68_TX_SOFTWARE_BUF_ENABLED) */
}


#if(0u != LTC68_BIDIRECTIONAL_MODE)
    /*******************************************************************************
    * Function Name: LTC68_TxEnable
    ********************************************************************************
    *
    * Summary:
    *  If the SPI master is configured to use a single bi-directional pin then this
    *  will set the bi-directional pin to transmit.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void LTC68_TxEnable(void) 
    {
        LTC68_CONTROL_REG |= LTC68_CTRL_TX_SIGNAL_EN;
    }


    /*******************************************************************************
    * Function Name: LTC68_TxDisable
    ********************************************************************************
    *
    * Summary:
    *  If the SPI master is configured to use a single bi-directional pin then this
    *  will set the bi-directional pin to receive.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void LTC68_TxDisable(void) 
    {
        LTC68_CONTROL_REG &= ((uint8) ~LTC68_CTRL_TX_SIGNAL_EN);
    }

#endif /* (0u != LTC68_BIDIRECTIONAL_MODE) */


/*******************************************************************************
* Function Name: LTC68_PutArray
********************************************************************************
*
* Summary:
*  Write available data from ROM/RAM to the TX buffer while space is available
*  in the TX buffer. Keep trying until all data is passed to the TX buffer.
*
* Parameters:
*  *buffer: Pointer to the location in RAM containing the data to send
*  byteCount: The number of bytes to move to the transmit buffer.
*
* Return:
*  None.
*
* Side Effects:
*  Will stay in this routine until all data has been sent.  May get locked in
*  this loop if data is not being initiated by the master if there is not
*  enough room in the TX FIFO.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LTC68_PutArray(const uint8 buffer[], uint8 byteCount)
                                                                          
{
    uint8 bufIndex;

    bufIndex = 0u;

    while(byteCount > 0u)
    {
        LTC68_WriteTxData(buffer[bufIndex]);
        bufIndex++;
        byteCount--;
    }
}


/*******************************************************************************
* Function Name: LTC68_ClearFIFO
********************************************************************************
*
* Summary:
*  Clear the RX and TX FIFO's of all data for a fresh start.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  Clear status register of the component.
*
*******************************************************************************/
void LTC68_ClearFIFO(void) 
{
    uint8 enableInterrupts;

    /* Clear Hardware RX FIFO */
    while(0u !=(LTC68_RX_STATUS_REG & LTC68_STS_RX_FIFO_NOT_EMPTY))
    {
        (void) CY_GET_REG8(LTC68_RXDATA_PTR);
    }

    enableInterrupts = CyEnterCriticalSection();
    /* Clear TX FIFO */
    LTC68_AUX_CONTROL_DP0_REG |= ((uint8)  LTC68_TX_FIFO_CLR);
    LTC68_AUX_CONTROL_DP0_REG &= ((uint8) ~LTC68_TX_FIFO_CLR);

    #if(LTC68_USE_SECOND_DATAPATH)
        /* Clear TX FIFO for 2nd Datapath */
        LTC68_AUX_CONTROL_DP1_REG |= ((uint8)  LTC68_TX_FIFO_CLR);
        LTC68_AUX_CONTROL_DP1_REG &= ((uint8) ~LTC68_TX_FIFO_CLR);
    #endif /* (LTC68_USE_SECOND_DATAPATH) */
    CyExitCriticalSection(enableInterrupts);
}


/* Following functions are for version Compatibility, they are obsolete.
*  Please do not use it in new projects.
*/


/*******************************************************************************
* Function Name: LTC68_EnableInt
********************************************************************************
*
* Summary:
*  Enable internal interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Enable the internal interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void LTC68_EnableInt(void) 
{
    LTC68_EnableRxInt();
    LTC68_EnableTxInt();
}


/*******************************************************************************
* Function Name: LTC68_DisableInt
********************************************************************************
*
* Summary:
*  Disable internal interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the internal interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void LTC68_DisableInt(void) 
{
    LTC68_DisableTxInt();
    LTC68_DisableRxInt();
}


/*******************************************************************************
* Function Name: LTC68_SetInterruptMode
********************************************************************************
*
* Summary:
*  Configure which status bits trigger an interrupt event.
*
* Parameters:
*  intSrc: An or'd combination of the desired status bit masks (defined in the
*  header file).
*
* Return:
*  None.
*
* Theory:
*  Enables the output of specific status bits to the interrupt controller.
*
*******************************************************************************/
void LTC68_SetInterruptMode(uint8 intSrc) 
{
    LTC68_TX_STATUS_MASK_REG  = (intSrc & ((uint8) ~LTC68_STS_SPI_IDLE));
    LTC68_RX_STATUS_MASK_REG  =  intSrc;
}


/*******************************************************************************
* Function Name: LTC68_ReadStatus
********************************************************************************
*
* Summary:
*  Read the status register for the component.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the status register.
*
* Global variables:
*  LTC68_swStatus - used to store in software status register,
*  modified every function call - resets to zero.
*
* Theory:
*  Allows the user and the API to read the status register for error detection
*  and flow control.
*
* Side Effects:
*  Clear status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 LTC68_ReadStatus(void) 
{
    uint8 tmpStatus;

    #if(LTC68_TX_SOFTWARE_BUF_ENABLED || LTC68_RX_SOFTWARE_BUF_ENABLED)

        LTC68_DisableInt();

        tmpStatus  = LTC68_GET_STATUS_RX(LTC68_swStatusRx);
        tmpStatus |= LTC68_GET_STATUS_TX(LTC68_swStatusTx);
        tmpStatus &= ((uint8) ~LTC68_STS_SPI_IDLE);

        LTC68_swStatusTx = 0u;
        LTC68_swStatusRx = 0u;

        LTC68_EnableInt();

    #else

        tmpStatus  = LTC68_RX_STATUS_REG;
        tmpStatus |= LTC68_TX_STATUS_REG;
        tmpStatus &= ((uint8) ~LTC68_STS_SPI_IDLE);

    #endif /* (LTC68_TX_SOFTWARE_BUF_ENABLED || LTC68_RX_SOFTWARE_BUF_ENABLED) */

    return(tmpStatus);
}


/* [] END OF FILE */
