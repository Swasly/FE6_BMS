/*******************************************************************************
* File Name: Can_Update_ISR.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_Can_Update_ISR_H)
#define CY_ISR_Can_Update_ISR_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void Can_Update_ISR_Start(void);
void Can_Update_ISR_StartEx(cyisraddress address);
void Can_Update_ISR_Stop(void);

CY_ISR_PROTO(Can_Update_ISR_Interrupt);

void Can_Update_ISR_SetVector(cyisraddress address);
cyisraddress Can_Update_ISR_GetVector(void);

void Can_Update_ISR_SetPriority(uint8 priority);
uint8 Can_Update_ISR_GetPriority(void);

void Can_Update_ISR_Enable(void);
uint8 Can_Update_ISR_GetState(void);
void Can_Update_ISR_Disable(void);

void Can_Update_ISR_SetPending(void);
void Can_Update_ISR_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the Can_Update_ISR ISR. */
#define Can_Update_ISR_INTC_VECTOR            ((reg32 *) Can_Update_ISR__INTC_VECT)

/* Address of the Can_Update_ISR ISR priority. */
#define Can_Update_ISR_INTC_PRIOR             ((reg8 *) Can_Update_ISR__INTC_PRIOR_REG)

/* Priority of the Can_Update_ISR interrupt. */
#define Can_Update_ISR_INTC_PRIOR_NUMBER      Can_Update_ISR__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable Can_Update_ISR interrupt. */
#define Can_Update_ISR_INTC_SET_EN            ((reg32 *) Can_Update_ISR__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the Can_Update_ISR interrupt. */
#define Can_Update_ISR_INTC_CLR_EN            ((reg32 *) Can_Update_ISR__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the Can_Update_ISR interrupt state to pending. */
#define Can_Update_ISR_INTC_SET_PD            ((reg32 *) Can_Update_ISR__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the Can_Update_ISR interrupt. */
#define Can_Update_ISR_INTC_CLR_PD            ((reg32 *) Can_Update_ISR__INTC_CLR_PD_REG)


#endif /* CY_ISR_Can_Update_ISR_H */


/* [] END OF FILE */
