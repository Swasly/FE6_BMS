/*******************************************************************************
* File Name: Tx_2.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Tx_2_H) /* Pins Tx_2_H */
#define CY_PINS_Tx_2_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Tx_2_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Tx_2__PORT == 15 && ((Tx_2__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Tx_2_Write(uint8 value);
void    Tx_2_SetDriveMode(uint8 mode);
uint8   Tx_2_ReadDataReg(void);
uint8   Tx_2_Read(void);
void    Tx_2_SetInterruptMode(uint16 position, uint16 mode);
uint8   Tx_2_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Tx_2_SetDriveMode() function.
     *  @{
     */
        #define Tx_2_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Tx_2_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Tx_2_DM_RES_UP          PIN_DM_RES_UP
        #define Tx_2_DM_RES_DWN         PIN_DM_RES_DWN
        #define Tx_2_DM_OD_LO           PIN_DM_OD_LO
        #define Tx_2_DM_OD_HI           PIN_DM_OD_HI
        #define Tx_2_DM_STRONG          PIN_DM_STRONG
        #define Tx_2_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Tx_2_MASK               Tx_2__MASK
#define Tx_2_SHIFT              Tx_2__SHIFT
#define Tx_2_WIDTH              1u

/* Interrupt constants */
#if defined(Tx_2__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Tx_2_SetInterruptMode() function.
     *  @{
     */
        #define Tx_2_INTR_NONE      (uint16)(0x0000u)
        #define Tx_2_INTR_RISING    (uint16)(0x0001u)
        #define Tx_2_INTR_FALLING   (uint16)(0x0002u)
        #define Tx_2_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Tx_2_INTR_MASK      (0x01u) 
#endif /* (Tx_2__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Tx_2_PS                     (* (reg8 *) Tx_2__PS)
/* Data Register */
#define Tx_2_DR                     (* (reg8 *) Tx_2__DR)
/* Port Number */
#define Tx_2_PRT_NUM                (* (reg8 *) Tx_2__PRT) 
/* Connect to Analog Globals */                                                  
#define Tx_2_AG                     (* (reg8 *) Tx_2__AG)                       
/* Analog MUX bux enable */
#define Tx_2_AMUX                   (* (reg8 *) Tx_2__AMUX) 
/* Bidirectional Enable */                                                        
#define Tx_2_BIE                    (* (reg8 *) Tx_2__BIE)
/* Bit-mask for Aliased Register Access */
#define Tx_2_BIT_MASK               (* (reg8 *) Tx_2__BIT_MASK)
/* Bypass Enable */
#define Tx_2_BYP                    (* (reg8 *) Tx_2__BYP)
/* Port wide control signals */                                                   
#define Tx_2_CTL                    (* (reg8 *) Tx_2__CTL)
/* Drive Modes */
#define Tx_2_DM0                    (* (reg8 *) Tx_2__DM0) 
#define Tx_2_DM1                    (* (reg8 *) Tx_2__DM1)
#define Tx_2_DM2                    (* (reg8 *) Tx_2__DM2) 
/* Input Buffer Disable Override */
#define Tx_2_INP_DIS                (* (reg8 *) Tx_2__INP_DIS)
/* LCD Common or Segment Drive */
#define Tx_2_LCD_COM_SEG            (* (reg8 *) Tx_2__LCD_COM_SEG)
/* Enable Segment LCD */
#define Tx_2_LCD_EN                 (* (reg8 *) Tx_2__LCD_EN)
/* Slew Rate Control */
#define Tx_2_SLW                    (* (reg8 *) Tx_2__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Tx_2_PRTDSI__CAPS_SEL       (* (reg8 *) Tx_2__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Tx_2_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Tx_2__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Tx_2_PRTDSI__OE_SEL0        (* (reg8 *) Tx_2__PRTDSI__OE_SEL0) 
#define Tx_2_PRTDSI__OE_SEL1        (* (reg8 *) Tx_2__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Tx_2_PRTDSI__OUT_SEL0       (* (reg8 *) Tx_2__PRTDSI__OUT_SEL0) 
#define Tx_2_PRTDSI__OUT_SEL1       (* (reg8 *) Tx_2__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Tx_2_PRTDSI__SYNC_OUT       (* (reg8 *) Tx_2__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Tx_2__SIO_CFG)
    #define Tx_2_SIO_HYST_EN        (* (reg8 *) Tx_2__SIO_HYST_EN)
    #define Tx_2_SIO_REG_HIFREQ     (* (reg8 *) Tx_2__SIO_REG_HIFREQ)
    #define Tx_2_SIO_CFG            (* (reg8 *) Tx_2__SIO_CFG)
    #define Tx_2_SIO_DIFF           (* (reg8 *) Tx_2__SIO_DIFF)
#endif /* (Tx_2__SIO_CFG) */

/* Interrupt Registers */
#if defined(Tx_2__INTSTAT)
    #define Tx_2_INTSTAT            (* (reg8 *) Tx_2__INTSTAT)
    #define Tx_2_SNAP               (* (reg8 *) Tx_2__SNAP)
    
	#define Tx_2_0_INTTYPE_REG 		(* (reg8 *) Tx_2__0__INTTYPE)
#endif /* (Tx_2__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Tx_2_H */


/* [] END OF FILE */
