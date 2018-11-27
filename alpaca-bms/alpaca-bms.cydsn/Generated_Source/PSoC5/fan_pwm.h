/*******************************************************************************
* File Name: fan_pwm.h  
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

#if !defined(CY_PINS_fan_pwm_H) /* Pins fan_pwm_H */
#define CY_PINS_fan_pwm_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "fan_pwm_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 fan_pwm__PORT == 15 && ((fan_pwm__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    fan_pwm_Write(uint8 value);
void    fan_pwm_SetDriveMode(uint8 mode);
uint8   fan_pwm_ReadDataReg(void);
uint8   fan_pwm_Read(void);
void    fan_pwm_SetInterruptMode(uint16 position, uint16 mode);
uint8   fan_pwm_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the fan_pwm_SetDriveMode() function.
     *  @{
     */
        #define fan_pwm_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define fan_pwm_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define fan_pwm_DM_RES_UP          PIN_DM_RES_UP
        #define fan_pwm_DM_RES_DWN         PIN_DM_RES_DWN
        #define fan_pwm_DM_OD_LO           PIN_DM_OD_LO
        #define fan_pwm_DM_OD_HI           PIN_DM_OD_HI
        #define fan_pwm_DM_STRONG          PIN_DM_STRONG
        #define fan_pwm_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define fan_pwm_MASK               fan_pwm__MASK
#define fan_pwm_SHIFT              fan_pwm__SHIFT
#define fan_pwm_WIDTH              1u

/* Interrupt constants */
#if defined(fan_pwm__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in fan_pwm_SetInterruptMode() function.
     *  @{
     */
        #define fan_pwm_INTR_NONE      (uint16)(0x0000u)
        #define fan_pwm_INTR_RISING    (uint16)(0x0001u)
        #define fan_pwm_INTR_FALLING   (uint16)(0x0002u)
        #define fan_pwm_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define fan_pwm_INTR_MASK      (0x01u) 
#endif /* (fan_pwm__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define fan_pwm_PS                     (* (reg8 *) fan_pwm__PS)
/* Data Register */
#define fan_pwm_DR                     (* (reg8 *) fan_pwm__DR)
/* Port Number */
#define fan_pwm_PRT_NUM                (* (reg8 *) fan_pwm__PRT) 
/* Connect to Analog Globals */                                                  
#define fan_pwm_AG                     (* (reg8 *) fan_pwm__AG)                       
/* Analog MUX bux enable */
#define fan_pwm_AMUX                   (* (reg8 *) fan_pwm__AMUX) 
/* Bidirectional Enable */                                                        
#define fan_pwm_BIE                    (* (reg8 *) fan_pwm__BIE)
/* Bit-mask for Aliased Register Access */
#define fan_pwm_BIT_MASK               (* (reg8 *) fan_pwm__BIT_MASK)
/* Bypass Enable */
#define fan_pwm_BYP                    (* (reg8 *) fan_pwm__BYP)
/* Port wide control signals */                                                   
#define fan_pwm_CTL                    (* (reg8 *) fan_pwm__CTL)
/* Drive Modes */
#define fan_pwm_DM0                    (* (reg8 *) fan_pwm__DM0) 
#define fan_pwm_DM1                    (* (reg8 *) fan_pwm__DM1)
#define fan_pwm_DM2                    (* (reg8 *) fan_pwm__DM2) 
/* Input Buffer Disable Override */
#define fan_pwm_INP_DIS                (* (reg8 *) fan_pwm__INP_DIS)
/* LCD Common or Segment Drive */
#define fan_pwm_LCD_COM_SEG            (* (reg8 *) fan_pwm__LCD_COM_SEG)
/* Enable Segment LCD */
#define fan_pwm_LCD_EN                 (* (reg8 *) fan_pwm__LCD_EN)
/* Slew Rate Control */
#define fan_pwm_SLW                    (* (reg8 *) fan_pwm__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define fan_pwm_PRTDSI__CAPS_SEL       (* (reg8 *) fan_pwm__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define fan_pwm_PRTDSI__DBL_SYNC_IN    (* (reg8 *) fan_pwm__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define fan_pwm_PRTDSI__OE_SEL0        (* (reg8 *) fan_pwm__PRTDSI__OE_SEL0) 
#define fan_pwm_PRTDSI__OE_SEL1        (* (reg8 *) fan_pwm__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define fan_pwm_PRTDSI__OUT_SEL0       (* (reg8 *) fan_pwm__PRTDSI__OUT_SEL0) 
#define fan_pwm_PRTDSI__OUT_SEL1       (* (reg8 *) fan_pwm__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define fan_pwm_PRTDSI__SYNC_OUT       (* (reg8 *) fan_pwm__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(fan_pwm__SIO_CFG)
    #define fan_pwm_SIO_HYST_EN        (* (reg8 *) fan_pwm__SIO_HYST_EN)
    #define fan_pwm_SIO_REG_HIFREQ     (* (reg8 *) fan_pwm__SIO_REG_HIFREQ)
    #define fan_pwm_SIO_CFG            (* (reg8 *) fan_pwm__SIO_CFG)
    #define fan_pwm_SIO_DIFF           (* (reg8 *) fan_pwm__SIO_DIFF)
#endif /* (fan_pwm__SIO_CFG) */

/* Interrupt Registers */
#if defined(fan_pwm__INTSTAT)
    #define fan_pwm_INTSTAT            (* (reg8 *) fan_pwm__INTSTAT)
    #define fan_pwm_SNAP               (* (reg8 *) fan_pwm__SNAP)
    
	#define fan_pwm_0_INTTYPE_REG 		(* (reg8 *) fan_pwm__0__INTTYPE)
#endif /* (fan_pwm__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_fan_pwm_H */


/* [] END OF FILE */
