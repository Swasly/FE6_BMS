/*******************************************************************************
* File Name: current_supply.h  
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

#if !defined(CY_PINS_current_supply_H) /* Pins current_supply_H */
#define CY_PINS_current_supply_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "current_supply_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 current_supply__PORT == 15 && ((current_supply__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    current_supply_Write(uint8 value);
void    current_supply_SetDriveMode(uint8 mode);
uint8   current_supply_ReadDataReg(void);
uint8   current_supply_Read(void);
void    current_supply_SetInterruptMode(uint16 position, uint16 mode);
uint8   current_supply_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the current_supply_SetDriveMode() function.
     *  @{
     */
        #define current_supply_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define current_supply_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define current_supply_DM_RES_UP          PIN_DM_RES_UP
        #define current_supply_DM_RES_DWN         PIN_DM_RES_DWN
        #define current_supply_DM_OD_LO           PIN_DM_OD_LO
        #define current_supply_DM_OD_HI           PIN_DM_OD_HI
        #define current_supply_DM_STRONG          PIN_DM_STRONG
        #define current_supply_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define current_supply_MASK               current_supply__MASK
#define current_supply_SHIFT              current_supply__SHIFT
#define current_supply_WIDTH              1u

/* Interrupt constants */
#if defined(current_supply__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in current_supply_SetInterruptMode() function.
     *  @{
     */
        #define current_supply_INTR_NONE      (uint16)(0x0000u)
        #define current_supply_INTR_RISING    (uint16)(0x0001u)
        #define current_supply_INTR_FALLING   (uint16)(0x0002u)
        #define current_supply_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define current_supply_INTR_MASK      (0x01u) 
#endif /* (current_supply__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define current_supply_PS                     (* (reg8 *) current_supply__PS)
/* Data Register */
#define current_supply_DR                     (* (reg8 *) current_supply__DR)
/* Port Number */
#define current_supply_PRT_NUM                (* (reg8 *) current_supply__PRT) 
/* Connect to Analog Globals */                                                  
#define current_supply_AG                     (* (reg8 *) current_supply__AG)                       
/* Analog MUX bux enable */
#define current_supply_AMUX                   (* (reg8 *) current_supply__AMUX) 
/* Bidirectional Enable */                                                        
#define current_supply_BIE                    (* (reg8 *) current_supply__BIE)
/* Bit-mask for Aliased Register Access */
#define current_supply_BIT_MASK               (* (reg8 *) current_supply__BIT_MASK)
/* Bypass Enable */
#define current_supply_BYP                    (* (reg8 *) current_supply__BYP)
/* Port wide control signals */                                                   
#define current_supply_CTL                    (* (reg8 *) current_supply__CTL)
/* Drive Modes */
#define current_supply_DM0                    (* (reg8 *) current_supply__DM0) 
#define current_supply_DM1                    (* (reg8 *) current_supply__DM1)
#define current_supply_DM2                    (* (reg8 *) current_supply__DM2) 
/* Input Buffer Disable Override */
#define current_supply_INP_DIS                (* (reg8 *) current_supply__INP_DIS)
/* LCD Common or Segment Drive */
#define current_supply_LCD_COM_SEG            (* (reg8 *) current_supply__LCD_COM_SEG)
/* Enable Segment LCD */
#define current_supply_LCD_EN                 (* (reg8 *) current_supply__LCD_EN)
/* Slew Rate Control */
#define current_supply_SLW                    (* (reg8 *) current_supply__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define current_supply_PRTDSI__CAPS_SEL       (* (reg8 *) current_supply__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define current_supply_PRTDSI__DBL_SYNC_IN    (* (reg8 *) current_supply__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define current_supply_PRTDSI__OE_SEL0        (* (reg8 *) current_supply__PRTDSI__OE_SEL0) 
#define current_supply_PRTDSI__OE_SEL1        (* (reg8 *) current_supply__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define current_supply_PRTDSI__OUT_SEL0       (* (reg8 *) current_supply__PRTDSI__OUT_SEL0) 
#define current_supply_PRTDSI__OUT_SEL1       (* (reg8 *) current_supply__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define current_supply_PRTDSI__SYNC_OUT       (* (reg8 *) current_supply__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(current_supply__SIO_CFG)
    #define current_supply_SIO_HYST_EN        (* (reg8 *) current_supply__SIO_HYST_EN)
    #define current_supply_SIO_REG_HIFREQ     (* (reg8 *) current_supply__SIO_REG_HIFREQ)
    #define current_supply_SIO_CFG            (* (reg8 *) current_supply__SIO_CFG)
    #define current_supply_SIO_DIFF           (* (reg8 *) current_supply__SIO_DIFF)
#endif /* (current_supply__SIO_CFG) */

/* Interrupt Registers */
#if defined(current_supply__INTSTAT)
    #define current_supply_INTSTAT            (* (reg8 *) current_supply__INTSTAT)
    #define current_supply_SNAP               (* (reg8 *) current_supply__SNAP)
    
	#define current_supply_0_INTTYPE_REG 		(* (reg8 *) current_supply__0__INTTYPE)
#endif /* (current_supply__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_current_supply_H */


/* [] END OF FILE */
