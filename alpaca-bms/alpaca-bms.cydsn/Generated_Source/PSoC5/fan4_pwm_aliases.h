/*******************************************************************************
* File Name: fan4_pwm.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_fan4_pwm_ALIASES_H) /* Pins fan4_pwm_ALIASES_H */
#define CY_PINS_fan4_pwm_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define fan4_pwm_0			(fan4_pwm__0__PC)
#define fan4_pwm_0_INTR	((uint16)((uint16)0x0001u << fan4_pwm__0__SHIFT))

#define fan4_pwm_INTR_ALL	 ((uint16)(fan4_pwm_0_INTR))

#endif /* End Pins fan4_pwm_ALIASES_H */


/* [] END OF FILE */
