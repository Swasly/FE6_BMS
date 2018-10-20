/*******************************************************************************
* File Name: poll.h
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
#if !defined(CY_ISR_poll_H)
#define CY_ISR_poll_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void poll_Start(void);
void poll_StartEx(cyisraddress address);
void poll_Stop(void);

CY_ISR_PROTO(poll_Interrupt);

void poll_SetVector(cyisraddress address);
cyisraddress poll_GetVector(void);

void poll_SetPriority(uint8 priority);
uint8 poll_GetPriority(void);

void poll_Enable(void);
uint8 poll_GetState(void);
void poll_Disable(void);

void poll_SetPending(void);
void poll_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the poll ISR. */
#define poll_INTC_VECTOR            ((reg32 *) poll__INTC_VECT)

/* Address of the poll ISR priority. */
#define poll_INTC_PRIOR             ((reg8 *) poll__INTC_PRIOR_REG)

/* Priority of the poll interrupt. */
#define poll_INTC_PRIOR_NUMBER      poll__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable poll interrupt. */
#define poll_INTC_SET_EN            ((reg32 *) poll__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the poll interrupt. */
#define poll_INTC_CLR_EN            ((reg32 *) poll__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the poll interrupt state to pending. */
#define poll_INTC_SET_PD            ((reg32 *) poll__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the poll interrupt. */
#define poll_INTC_CLR_PD            ((reg32 *) poll__INTC_CLR_PD_REG)


#endif /* CY_ISR_poll_H */


/* [] END OF FILE */
