/*******************************************************************************
* File Name: current.h  
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

#if !defined(CY_PINS_current_H) /* Pins current_H */
#define CY_PINS_current_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "current_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 current__PORT == 15 && ((current__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    current_Write(uint8 value);
void    current_SetDriveMode(uint8 mode);
uint8   current_ReadDataReg(void);
uint8   current_Read(void);
void    current_SetInterruptMode(uint16 position, uint16 mode);
uint8   current_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the current_SetDriveMode() function.
     *  @{
     */
        #define current_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define current_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define current_DM_RES_UP          PIN_DM_RES_UP
        #define current_DM_RES_DWN         PIN_DM_RES_DWN
        #define current_DM_OD_LO           PIN_DM_OD_LO
        #define current_DM_OD_HI           PIN_DM_OD_HI
        #define current_DM_STRONG          PIN_DM_STRONG
        #define current_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define current_MASK               current__MASK
#define current_SHIFT              current__SHIFT
#define current_WIDTH              1u

/* Interrupt constants */
#if defined(current__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in current_SetInterruptMode() function.
     *  @{
     */
        #define current_INTR_NONE      (uint16)(0x0000u)
        #define current_INTR_RISING    (uint16)(0x0001u)
        #define current_INTR_FALLING   (uint16)(0x0002u)
        #define current_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define current_INTR_MASK      (0x01u) 
#endif /* (current__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define current_PS                     (* (reg8 *) current__PS)
/* Data Register */
#define current_DR                     (* (reg8 *) current__DR)
/* Port Number */
#define current_PRT_NUM                (* (reg8 *) current__PRT) 
/* Connect to Analog Globals */                                                  
#define current_AG                     (* (reg8 *) current__AG)                       
/* Analog MUX bux enable */
#define current_AMUX                   (* (reg8 *) current__AMUX) 
/* Bidirectional Enable */                                                        
#define current_BIE                    (* (reg8 *) current__BIE)
/* Bit-mask for Aliased Register Access */
#define current_BIT_MASK               (* (reg8 *) current__BIT_MASK)
/* Bypass Enable */
#define current_BYP                    (* (reg8 *) current__BYP)
/* Port wide control signals */                                                   
#define current_CTL                    (* (reg8 *) current__CTL)
/* Drive Modes */
#define current_DM0                    (* (reg8 *) current__DM0) 
#define current_DM1                    (* (reg8 *) current__DM1)
#define current_DM2                    (* (reg8 *) current__DM2) 
/* Input Buffer Disable Override */
#define current_INP_DIS                (* (reg8 *) current__INP_DIS)
/* LCD Common or Segment Drive */
#define current_LCD_COM_SEG            (* (reg8 *) current__LCD_COM_SEG)
/* Enable Segment LCD */
#define current_LCD_EN                 (* (reg8 *) current__LCD_EN)
/* Slew Rate Control */
#define current_SLW                    (* (reg8 *) current__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define current_PRTDSI__CAPS_SEL       (* (reg8 *) current__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define current_PRTDSI__DBL_SYNC_IN    (* (reg8 *) current__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define current_PRTDSI__OE_SEL0        (* (reg8 *) current__PRTDSI__OE_SEL0) 
#define current_PRTDSI__OE_SEL1        (* (reg8 *) current__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define current_PRTDSI__OUT_SEL0       (* (reg8 *) current__PRTDSI__OUT_SEL0) 
#define current_PRTDSI__OUT_SEL1       (* (reg8 *) current__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define current_PRTDSI__SYNC_OUT       (* (reg8 *) current__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(current__SIO_CFG)
    #define current_SIO_HYST_EN        (* (reg8 *) current__SIO_HYST_EN)
    #define current_SIO_REG_HIFREQ     (* (reg8 *) current__SIO_REG_HIFREQ)
    #define current_SIO_CFG            (* (reg8 *) current__SIO_CFG)
    #define current_SIO_DIFF           (* (reg8 *) current__SIO_DIFF)
#endif /* (current__SIO_CFG) */

/* Interrupt Registers */
#if defined(current__INTSTAT)
    #define current_INTSTAT            (* (reg8 *) current__INTSTAT)
    #define current_SNAP               (* (reg8 *) current__SNAP)
    
	#define current_0_INTTYPE_REG 		(* (reg8 *) current__0__INTTYPE)
#endif /* (current__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_current_H */


/* [] END OF FILE */
