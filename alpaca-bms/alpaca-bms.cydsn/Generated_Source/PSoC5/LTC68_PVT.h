/*******************************************************************************
* File Name: .h
* Version 2.50
*
* Description:
*  This private header file contains internal definitions for the SPIM
*  component. Do not use these definitions directly in your application.
*
* Note:
*
********************************************************************************
* Copyright 2012-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SPIM_PVT_LTC68_H)
#define CY_SPIM_PVT_LTC68_H

#include "LTC68.h"


/**********************************
*   Functions with external linkage
**********************************/


/**********************************
*   Variables with external linkage
**********************************/

extern volatile uint8 LTC68_swStatusTx;
extern volatile uint8 LTC68_swStatusRx;

#if(LTC68_TX_SOFTWARE_BUF_ENABLED)
    extern volatile uint8 LTC68_txBuffer[LTC68_TX_BUFFER_SIZE];
    extern volatile uint8 LTC68_txBufferRead;
    extern volatile uint8 LTC68_txBufferWrite;
    extern volatile uint8 LTC68_txBufferFull;
#endif /* (LTC68_TX_SOFTWARE_BUF_ENABLED) */

#if(LTC68_RX_SOFTWARE_BUF_ENABLED)
    extern volatile uint8 LTC68_rxBuffer[LTC68_RX_BUFFER_SIZE];
    extern volatile uint8 LTC68_rxBufferRead;
    extern volatile uint8 LTC68_rxBufferWrite;
    extern volatile uint8 LTC68_rxBufferFull;
#endif /* (LTC68_RX_SOFTWARE_BUF_ENABLED) */

#endif /* CY_SPIM_PVT_LTC68_H */


/* [] END OF FILE */
