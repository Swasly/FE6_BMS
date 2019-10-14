/*******************************************************************************
* File Name: LTC68.h
* Version 2.50
*
* Description:
*  Contains the function prototypes, constants and register definition
*  of the SPI Master Component.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SPIM_LTC68_H)
#define CY_SPIM_LTC68_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define LTC68_INTERNAL_CLOCK             (1u)

#if(0u != LTC68_INTERNAL_CLOCK)
    #include "LTC68_IntClock.h"
#endif /* (0u != LTC68_INTERNAL_CLOCK) */

#define LTC68_MODE                       (4u)
#define LTC68_DATA_WIDTH                 (8u)
#define LTC68_MODE_USE_ZERO              (0u)
#define LTC68_BIDIRECTIONAL_MODE         (0u)

/* Internal interrupt handling */
#define LTC68_TX_BUFFER_SIZE             (255u)
#define LTC68_RX_BUFFER_SIZE             (255u)
#define LTC68_INTERNAL_TX_INT_ENABLED    (1u)
#define LTC68_INTERNAL_RX_INT_ENABLED    (1u)

#define LTC68_SINGLE_REG_SIZE            (8u)
#define LTC68_USE_SECOND_DATAPATH        (LTC68_DATA_WIDTH > LTC68_SINGLE_REG_SIZE)

#define LTC68_FIFO_SIZE                  (4u)
#define LTC68_TX_SOFTWARE_BUF_ENABLED    ((0u != LTC68_INTERNAL_TX_INT_ENABLED) && \
                                                     (LTC68_TX_BUFFER_SIZE > LTC68_FIFO_SIZE))

#define LTC68_RX_SOFTWARE_BUF_ENABLED    ((0u != LTC68_INTERNAL_RX_INT_ENABLED) && \
                                                     (LTC68_RX_BUFFER_SIZE > LTC68_FIFO_SIZE))


/***************************************
*        Data Struct Definition
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 enableState;
    uint8 cntrPeriod;
} LTC68_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

void  LTC68_Init(void)                           ;
void  LTC68_Enable(void)                         ;
void  LTC68_Start(void)                          ;
void  LTC68_Stop(void)                           ;

void  LTC68_EnableTxInt(void)                    ;
void  LTC68_EnableRxInt(void)                    ;
void  LTC68_DisableTxInt(void)                   ;
void  LTC68_DisableRxInt(void)                   ;

void  LTC68_Sleep(void)                          ;
void  LTC68_Wakeup(void)                         ;
void  LTC68_SaveConfig(void)                     ;
void  LTC68_RestoreConfig(void)                  ;

void  LTC68_SetTxInterruptMode(uint8 intSrc)     ;
void  LTC68_SetRxInterruptMode(uint8 intSrc)     ;
uint8 LTC68_ReadTxStatus(void)                   ;
uint8 LTC68_ReadRxStatus(void)                   ;
void  LTC68_WriteTxData(uint8 txData)  \
                                                            ;
uint8 LTC68_ReadRxData(void) \
                                                            ;
uint8 LTC68_GetRxBufferSize(void)                ;
uint8 LTC68_GetTxBufferSize(void)                ;
void  LTC68_ClearRxBuffer(void)                  ;
void  LTC68_ClearTxBuffer(void)                  ;
void  LTC68_ClearFIFO(void)                              ;
void  LTC68_PutArray(const uint8 buffer[], uint8 byteCount) \
                                                            ;

#if(0u != LTC68_BIDIRECTIONAL_MODE)
    void  LTC68_TxEnable(void)                   ;
    void  LTC68_TxDisable(void)                  ;
#endif /* (0u != LTC68_BIDIRECTIONAL_MODE) */

CY_ISR_PROTO(LTC68_TX_ISR);
CY_ISR_PROTO(LTC68_RX_ISR);


/***************************************
*   Variable with external linkage
***************************************/

extern uint8 LTC68_initVar;


/***************************************
*           API Constants
***************************************/

#define LTC68_TX_ISR_NUMBER     ((uint8) (LTC68_TxInternalInterrupt__INTC_NUMBER))
#define LTC68_RX_ISR_NUMBER     ((uint8) (LTC68_RxInternalInterrupt__INTC_NUMBER))

#define LTC68_TX_ISR_PRIORITY   ((uint8) (LTC68_TxInternalInterrupt__INTC_PRIOR_NUM))
#define LTC68_RX_ISR_PRIORITY   ((uint8) (LTC68_RxInternalInterrupt__INTC_PRIOR_NUM))


/***************************************
*    Initial Parameter Constants
***************************************/

#define LTC68_INT_ON_SPI_DONE    ((uint8) (0u   << LTC68_STS_SPI_DONE_SHIFT))
#define LTC68_INT_ON_TX_EMPTY    ((uint8) (0u   << LTC68_STS_TX_FIFO_EMPTY_SHIFT))
#define LTC68_INT_ON_TX_NOT_FULL ((uint8) (1u << \
                                                                           LTC68_STS_TX_FIFO_NOT_FULL_SHIFT))
#define LTC68_INT_ON_BYTE_COMP   ((uint8) (0u  << LTC68_STS_BYTE_COMPLETE_SHIFT))
#define LTC68_INT_ON_SPI_IDLE    ((uint8) (0u   << LTC68_STS_SPI_IDLE_SHIFT))

/* Disable TX_NOT_FULL if software buffer is used */
#define LTC68_INT_ON_TX_NOT_FULL_DEF ((LTC68_TX_SOFTWARE_BUF_ENABLED) ? \
                                                                        (0u) : (LTC68_INT_ON_TX_NOT_FULL))

/* TX interrupt mask */
#define LTC68_TX_INIT_INTERRUPTS_MASK    (LTC68_INT_ON_SPI_DONE  | \
                                                     LTC68_INT_ON_TX_EMPTY  | \
                                                     LTC68_INT_ON_TX_NOT_FULL_DEF | \
                                                     LTC68_INT_ON_BYTE_COMP | \
                                                     LTC68_INT_ON_SPI_IDLE)

#define LTC68_INT_ON_RX_FULL         ((uint8) (0u << \
                                                                          LTC68_STS_RX_FIFO_FULL_SHIFT))
#define LTC68_INT_ON_RX_NOT_EMPTY    ((uint8) (1u << \
                                                                          LTC68_STS_RX_FIFO_NOT_EMPTY_SHIFT))
#define LTC68_INT_ON_RX_OVER         ((uint8) (0u << \
                                                                          LTC68_STS_RX_FIFO_OVERRUN_SHIFT))

/* RX interrupt mask */
#define LTC68_RX_INIT_INTERRUPTS_MASK    (LTC68_INT_ON_RX_FULL      | \
                                                     LTC68_INT_ON_RX_NOT_EMPTY | \
                                                     LTC68_INT_ON_RX_OVER)
/* Nubmer of bits to receive/transmit */
#define LTC68_BITCTR_INIT            (((uint8) (LTC68_DATA_WIDTH << 1u)) - 1u)


/***************************************
*             Registers
***************************************/
#if(CY_PSOC3 || CY_PSOC5)
    #define LTC68_TXDATA_REG (* (reg8 *) \
                                                LTC68_BSPIM_sR8_Dp_u0__F0_REG)
    #define LTC68_TXDATA_PTR (  (reg8 *) \
                                                LTC68_BSPIM_sR8_Dp_u0__F0_REG)
    #define LTC68_RXDATA_REG (* (reg8 *) \
                                                LTC68_BSPIM_sR8_Dp_u0__F1_REG)
    #define LTC68_RXDATA_PTR (  (reg8 *) \
                                                LTC68_BSPIM_sR8_Dp_u0__F1_REG)
#else   /* PSOC4 */
    #if(LTC68_USE_SECOND_DATAPATH)
        #define LTC68_TXDATA_REG (* (reg16 *) \
                                          LTC68_BSPIM_sR8_Dp_u0__16BIT_F0_REG)
        #define LTC68_TXDATA_PTR (  (reg16 *) \
                                          LTC68_BSPIM_sR8_Dp_u0__16BIT_F0_REG)
        #define LTC68_RXDATA_REG (* (reg16 *) \
                                          LTC68_BSPIM_sR8_Dp_u0__16BIT_F1_REG)
        #define LTC68_RXDATA_PTR (  (reg16 *) \
                                          LTC68_BSPIM_sR8_Dp_u0__16BIT_F1_REG)
    #else
        #define LTC68_TXDATA_REG (* (reg8 *) \
                                                LTC68_BSPIM_sR8_Dp_u0__F0_REG)
        #define LTC68_TXDATA_PTR (  (reg8 *) \
                                                LTC68_BSPIM_sR8_Dp_u0__F0_REG)
        #define LTC68_RXDATA_REG (* (reg8 *) \
                                                LTC68_BSPIM_sR8_Dp_u0__F1_REG)
        #define LTC68_RXDATA_PTR (  (reg8 *) \
                                                LTC68_BSPIM_sR8_Dp_u0__F1_REG)
    #endif /* (LTC68_USE_SECOND_DATAPATH) */
#endif     /* (CY_PSOC3 || CY_PSOC5) */

#define LTC68_AUX_CONTROL_DP0_REG (* (reg8 *) \
                                        LTC68_BSPIM_sR8_Dp_u0__DP_AUX_CTL_REG)
#define LTC68_AUX_CONTROL_DP0_PTR (  (reg8 *) \
                                        LTC68_BSPIM_sR8_Dp_u0__DP_AUX_CTL_REG)

#if(LTC68_USE_SECOND_DATAPATH)
    #define LTC68_AUX_CONTROL_DP1_REG  (* (reg8 *) \
                                        LTC68_BSPIM_sR8_Dp_u1__DP_AUX_CTL_REG)
    #define LTC68_AUX_CONTROL_DP1_PTR  (  (reg8 *) \
                                        LTC68_BSPIM_sR8_Dp_u1__DP_AUX_CTL_REG)
#endif /* (LTC68_USE_SECOND_DATAPATH) */

#define LTC68_COUNTER_PERIOD_REG     (* (reg8 *) LTC68_BSPIM_BitCounter__PERIOD_REG)
#define LTC68_COUNTER_PERIOD_PTR     (  (reg8 *) LTC68_BSPIM_BitCounter__PERIOD_REG)
#define LTC68_COUNTER_CONTROL_REG    (* (reg8 *) LTC68_BSPIM_BitCounter__CONTROL_AUX_CTL_REG)
#define LTC68_COUNTER_CONTROL_PTR    (  (reg8 *) LTC68_BSPIM_BitCounter__CONTROL_AUX_CTL_REG)

#define LTC68_TX_STATUS_REG          (* (reg8 *) LTC68_BSPIM_TxStsReg__STATUS_REG)
#define LTC68_TX_STATUS_PTR          (  (reg8 *) LTC68_BSPIM_TxStsReg__STATUS_REG)
#define LTC68_RX_STATUS_REG          (* (reg8 *) LTC68_BSPIM_RxStsReg__STATUS_REG)
#define LTC68_RX_STATUS_PTR          (  (reg8 *) LTC68_BSPIM_RxStsReg__STATUS_REG)

#define LTC68_CONTROL_REG            (* (reg8 *) \
                                      LTC68_BSPIM_BidirMode_CtrlReg__CONTROL_REG)
#define LTC68_CONTROL_PTR            (  (reg8 *) \
                                      LTC68_BSPIM_BidirMode_CtrlReg__CONTROL_REG)

#define LTC68_TX_STATUS_MASK_REG     (* (reg8 *) LTC68_BSPIM_TxStsReg__MASK_REG)
#define LTC68_TX_STATUS_MASK_PTR     (  (reg8 *) LTC68_BSPIM_TxStsReg__MASK_REG)
#define LTC68_RX_STATUS_MASK_REG     (* (reg8 *) LTC68_BSPIM_RxStsReg__MASK_REG)
#define LTC68_RX_STATUS_MASK_PTR     (  (reg8 *) LTC68_BSPIM_RxStsReg__MASK_REG)

#define LTC68_TX_STATUS_ACTL_REG     (* (reg8 *) LTC68_BSPIM_TxStsReg__STATUS_AUX_CTL_REG)
#define LTC68_TX_STATUS_ACTL_PTR     (  (reg8 *) LTC68_BSPIM_TxStsReg__STATUS_AUX_CTL_REG)
#define LTC68_RX_STATUS_ACTL_REG     (* (reg8 *) LTC68_BSPIM_RxStsReg__STATUS_AUX_CTL_REG)
#define LTC68_RX_STATUS_ACTL_PTR     (  (reg8 *) LTC68_BSPIM_RxStsReg__STATUS_AUX_CTL_REG)

#if(LTC68_USE_SECOND_DATAPATH)
    #define LTC68_AUX_CONTROLDP1     (LTC68_AUX_CONTROL_DP1_REG)
#endif /* (LTC68_USE_SECOND_DATAPATH) */


/***************************************
*       Register Constants
***************************************/

/* Status Register Definitions */
#define LTC68_STS_SPI_DONE_SHIFT             (0x00u)
#define LTC68_STS_TX_FIFO_EMPTY_SHIFT        (0x01u)
#define LTC68_STS_TX_FIFO_NOT_FULL_SHIFT     (0x02u)
#define LTC68_STS_BYTE_COMPLETE_SHIFT        (0x03u)
#define LTC68_STS_SPI_IDLE_SHIFT             (0x04u)
#define LTC68_STS_RX_FIFO_FULL_SHIFT         (0x04u)
#define LTC68_STS_RX_FIFO_NOT_EMPTY_SHIFT    (0x05u)
#define LTC68_STS_RX_FIFO_OVERRUN_SHIFT      (0x06u)

#define LTC68_STS_SPI_DONE           ((uint8) (0x01u << LTC68_STS_SPI_DONE_SHIFT))
#define LTC68_STS_TX_FIFO_EMPTY      ((uint8) (0x01u << LTC68_STS_TX_FIFO_EMPTY_SHIFT))
#define LTC68_STS_TX_FIFO_NOT_FULL   ((uint8) (0x01u << LTC68_STS_TX_FIFO_NOT_FULL_SHIFT))
#define LTC68_STS_BYTE_COMPLETE      ((uint8) (0x01u << LTC68_STS_BYTE_COMPLETE_SHIFT))
#define LTC68_STS_SPI_IDLE           ((uint8) (0x01u << LTC68_STS_SPI_IDLE_SHIFT))
#define LTC68_STS_RX_FIFO_FULL       ((uint8) (0x01u << LTC68_STS_RX_FIFO_FULL_SHIFT))
#define LTC68_STS_RX_FIFO_NOT_EMPTY  ((uint8) (0x01u << LTC68_STS_RX_FIFO_NOT_EMPTY_SHIFT))
#define LTC68_STS_RX_FIFO_OVERRUN    ((uint8) (0x01u << LTC68_STS_RX_FIFO_OVERRUN_SHIFT))

/* TX and RX masks for clear on read bits */
#define LTC68_TX_STS_CLR_ON_RD_BYTES_MASK    (0x09u)
#define LTC68_RX_STS_CLR_ON_RD_BYTES_MASK    (0x40u)

/* StatusI Register Interrupt Enable Control Bits */
/* As defined by the Register map for the AUX Control Register */
#define LTC68_INT_ENABLE     (0x10u) /* Enable interrupt from statusi */
#define LTC68_TX_FIFO_CLR    (0x01u) /* F0 - TX FIFO */
#define LTC68_RX_FIFO_CLR    (0x02u) /* F1 - RX FIFO */
#define LTC68_FIFO_CLR       (LTC68_TX_FIFO_CLR | LTC68_RX_FIFO_CLR)

/* Bit Counter (7-bit) Control Register Bit Definitions */
/* As defined by the Register map for the AUX Control Register */
#define LTC68_CNTR_ENABLE    (0x20u) /* Enable CNT7 */

/* Bi-Directional mode control bit */
#define LTC68_CTRL_TX_SIGNAL_EN  (0x01u)

/* Datapath Auxillary Control Register definitions */
#define LTC68_AUX_CTRL_FIFO0_CLR         (0x01u)
#define LTC68_AUX_CTRL_FIFO1_CLR         (0x02u)
#define LTC68_AUX_CTRL_FIFO0_LVL         (0x04u)
#define LTC68_AUX_CTRL_FIFO1_LVL         (0x08u)
#define LTC68_STATUS_ACTL_INT_EN_MASK    (0x10u)

/* Component disabled */
#define LTC68_DISABLED   (0u)


/***************************************
*       Macros
***************************************/

/* Returns true if componentn enabled */
#define LTC68_IS_ENABLED (0u != (LTC68_TX_STATUS_ACTL_REG & LTC68_INT_ENABLE))

/* Retuns TX status register */
#define LTC68_GET_STATUS_TX(swTxSts) ( (uint8)(LTC68_TX_STATUS_REG | \
                                                          ((swTxSts) & LTC68_TX_STS_CLR_ON_RD_BYTES_MASK)) )
/* Retuns RX status register */
#define LTC68_GET_STATUS_RX(swRxSts) ( (uint8)(LTC68_RX_STATUS_REG | \
                                                          ((swRxSts) & LTC68_RX_STS_CLR_ON_RD_BYTES_MASK)) )


/***************************************
* The following code is DEPRECATED and 
* should not be used in new projects.
***************************************/

#define LTC68_WriteByte   LTC68_WriteTxData
#define LTC68_ReadByte    LTC68_ReadRxData
void  LTC68_SetInterruptMode(uint8 intSrc)       ;
uint8 LTC68_ReadStatus(void)                     ;
void  LTC68_EnableInt(void)                      ;
void  LTC68_DisableInt(void)                     ;

#define LTC68_TXDATA                 (LTC68_TXDATA_REG)
#define LTC68_RXDATA                 (LTC68_RXDATA_REG)
#define LTC68_AUX_CONTROLDP0         (LTC68_AUX_CONTROL_DP0_REG)
#define LTC68_TXBUFFERREAD           (LTC68_txBufferRead)
#define LTC68_TXBUFFERWRITE          (LTC68_txBufferWrite)
#define LTC68_RXBUFFERREAD           (LTC68_rxBufferRead)
#define LTC68_RXBUFFERWRITE          (LTC68_rxBufferWrite)

#define LTC68_COUNTER_PERIOD         (LTC68_COUNTER_PERIOD_REG)
#define LTC68_COUNTER_CONTROL        (LTC68_COUNTER_CONTROL_REG)
#define LTC68_STATUS                 (LTC68_TX_STATUS_REG)
#define LTC68_CONTROL                (LTC68_CONTROL_REG)
#define LTC68_STATUS_MASK            (LTC68_TX_STATUS_MASK_REG)
#define LTC68_STATUS_ACTL            (LTC68_TX_STATUS_ACTL_REG)

#define LTC68_INIT_INTERRUPTS_MASK  (LTC68_INT_ON_SPI_DONE     | \
                                                LTC68_INT_ON_TX_EMPTY     | \
                                                LTC68_INT_ON_TX_NOT_FULL_DEF  | \
                                                LTC68_INT_ON_RX_FULL      | \
                                                LTC68_INT_ON_RX_NOT_EMPTY | \
                                                LTC68_INT_ON_RX_OVER      | \
                                                LTC68_INT_ON_BYTE_COMP)
                                                
#define LTC68_DataWidth                  (LTC68_DATA_WIDTH)
#define LTC68_InternalClockUsed          (LTC68_INTERNAL_CLOCK)
#define LTC68_InternalTxInterruptEnabled (LTC68_INTERNAL_TX_INT_ENABLED)
#define LTC68_InternalRxInterruptEnabled (LTC68_INTERNAL_RX_INT_ENABLED)
#define LTC68_ModeUseZero                (LTC68_MODE_USE_ZERO)
#define LTC68_BidirectionalMode          (LTC68_BIDIRECTIONAL_MODE)
#define LTC68_Mode                       (LTC68_MODE)
#define LTC68_DATAWIDHT                  (LTC68_DATA_WIDTH)
#define LTC68_InternalInterruptEnabled   (0u)

#define LTC68_TXBUFFERSIZE   (LTC68_TX_BUFFER_SIZE)
#define LTC68_RXBUFFERSIZE   (LTC68_RX_BUFFER_SIZE)

#define LTC68_TXBUFFER       LTC68_txBuffer
#define LTC68_RXBUFFER       LTC68_rxBuffer

#endif /* (CY_SPIM_LTC68_H) */


/* [] END OF FILE */
