/*******************************************************************************
* File Name: OK_SIG.h  
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

#if !defined(CY_PINS_OK_SIG_H) /* Pins OK_SIG_H */
#define CY_PINS_OK_SIG_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "OK_SIG_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 OK_SIG__PORT == 15 && ((OK_SIG__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    OK_SIG_Write(uint8 value);
void    OK_SIG_SetDriveMode(uint8 mode);
uint8   OK_SIG_ReadDataReg(void);
uint8   OK_SIG_Read(void);
void    OK_SIG_SetInterruptMode(uint16 position, uint16 mode);
uint8   OK_SIG_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the OK_SIG_SetDriveMode() function.
     *  @{
     */
        #define OK_SIG_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define OK_SIG_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define OK_SIG_DM_RES_UP          PIN_DM_RES_UP
        #define OK_SIG_DM_RES_DWN         PIN_DM_RES_DWN
        #define OK_SIG_DM_OD_LO           PIN_DM_OD_LO
        #define OK_SIG_DM_OD_HI           PIN_DM_OD_HI
        #define OK_SIG_DM_STRONG          PIN_DM_STRONG
        #define OK_SIG_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define OK_SIG_MASK               OK_SIG__MASK
#define OK_SIG_SHIFT              OK_SIG__SHIFT
#define OK_SIG_WIDTH              1u

/* Interrupt constants */
#if defined(OK_SIG__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in OK_SIG_SetInterruptMode() function.
     *  @{
     */
        #define OK_SIG_INTR_NONE      (uint16)(0x0000u)
        #define OK_SIG_INTR_RISING    (uint16)(0x0001u)
        #define OK_SIG_INTR_FALLING   (uint16)(0x0002u)
        #define OK_SIG_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define OK_SIG_INTR_MASK      (0x01u) 
#endif /* (OK_SIG__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define OK_SIG_PS                     (* (reg8 *) OK_SIG__PS)
/* Data Register */
#define OK_SIG_DR                     (* (reg8 *) OK_SIG__DR)
/* Port Number */
#define OK_SIG_PRT_NUM                (* (reg8 *) OK_SIG__PRT) 
/* Connect to Analog Globals */                                                  
#define OK_SIG_AG                     (* (reg8 *) OK_SIG__AG)                       
/* Analog MUX bux enable */
#define OK_SIG_AMUX                   (* (reg8 *) OK_SIG__AMUX) 
/* Bidirectional Enable */                                                        
#define OK_SIG_BIE                    (* (reg8 *) OK_SIG__BIE)
/* Bit-mask for Aliased Register Access */
#define OK_SIG_BIT_MASK               (* (reg8 *) OK_SIG__BIT_MASK)
/* Bypass Enable */
#define OK_SIG_BYP                    (* (reg8 *) OK_SIG__BYP)
/* Port wide control signals */                                                   
#define OK_SIG_CTL                    (* (reg8 *) OK_SIG__CTL)
/* Drive Modes */
#define OK_SIG_DM0                    (* (reg8 *) OK_SIG__DM0) 
#define OK_SIG_DM1                    (* (reg8 *) OK_SIG__DM1)
#define OK_SIG_DM2                    (* (reg8 *) OK_SIG__DM2) 
/* Input Buffer Disable Override */
#define OK_SIG_INP_DIS                (* (reg8 *) OK_SIG__INP_DIS)
/* LCD Common or Segment Drive */
#define OK_SIG_LCD_COM_SEG            (* (reg8 *) OK_SIG__LCD_COM_SEG)
/* Enable Segment LCD */
#define OK_SIG_LCD_EN                 (* (reg8 *) OK_SIG__LCD_EN)
/* Slew Rate Control */
#define OK_SIG_SLW                    (* (reg8 *) OK_SIG__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define OK_SIG_PRTDSI__CAPS_SEL       (* (reg8 *) OK_SIG__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define OK_SIG_PRTDSI__DBL_SYNC_IN    (* (reg8 *) OK_SIG__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define OK_SIG_PRTDSI__OE_SEL0        (* (reg8 *) OK_SIG__PRTDSI__OE_SEL0) 
#define OK_SIG_PRTDSI__OE_SEL1        (* (reg8 *) OK_SIG__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define OK_SIG_PRTDSI__OUT_SEL0       (* (reg8 *) OK_SIG__PRTDSI__OUT_SEL0) 
#define OK_SIG_PRTDSI__OUT_SEL1       (* (reg8 *) OK_SIG__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define OK_SIG_PRTDSI__SYNC_OUT       (* (reg8 *) OK_SIG__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(OK_SIG__SIO_CFG)
    #define OK_SIG_SIO_HYST_EN        (* (reg8 *) OK_SIG__SIO_HYST_EN)
    #define OK_SIG_SIO_REG_HIFREQ     (* (reg8 *) OK_SIG__SIO_REG_HIFREQ)
    #define OK_SIG_SIO_CFG            (* (reg8 *) OK_SIG__SIO_CFG)
    #define OK_SIG_SIO_DIFF           (* (reg8 *) OK_SIG__SIO_DIFF)
#endif /* (OK_SIG__SIO_CFG) */

/* Interrupt Registers */
#if defined(OK_SIG__INTSTAT)
    #define OK_SIG_INTSTAT            (* (reg8 *) OK_SIG__INTSTAT)
    #define OK_SIG_SNAP               (* (reg8 *) OK_SIG__SNAP)
    
	#define OK_SIG_0_INTTYPE_REG 		(* (reg8 *) OK_SIG__0__INTTYPE)
#endif /* (OK_SIG__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_OK_SIG_H */


/* [] END OF FILE */
