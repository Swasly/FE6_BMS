/*******************************************************************************
* File Name: Thermistor1.h
* Version 1.20
*
* Description:
*  This header file provides registers and constants associated with the
*  ThermistorCalc component.
*
* Note:
*  None.
*
********************************************************************************
* Copyright 2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_THERMISTOR_CALC_Thermistor1_H)
#define CY_THERMISTOR_CALC_Thermistor1_H

#include "cyfitter.h"
#include "CyLib.h"

#define Thermistor1_IMPLEMENTATION         (1u)
#define Thermistor1_EQUATION_METHOD        (0u)
#define Thermistor1_LUT_METHOD             (1u)

#if (Thermistor1_IMPLEMENTATION == Thermistor1_EQUATION_METHOD)
    #include <math.h>
#endif /* (Thermistor1_IMPLEMENTATION == Thermistor1_EQUATION_METHOD) */


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define Thermistor1_REF_RESISTOR           (10000)
#define Thermistor1_REF_RES_SHIFT          (0u)
#define Thermistor1_ACCURACY               (5u)
#define Thermistor1_MIN_TEMP               (10 * Thermistor1_SCALE)


/***************************************
*        Function Prototypes
***************************************/

uint32 Thermistor1_GetResistance(int16 vReference, int16 vThermistor)
                                      ;
int16 Thermistor1_GetTemperature(uint32 resT) ;


/***************************************
*           API Constants
***************************************/

#define Thermistor1_K2C                    (273.15)
#define Thermistor1_SCALE                  (100)
#define Thermistor1_LUT_SIZE               (1401u)


#endif /* CY_THERMISTOR_CALC_Thermistor1_H */


/* [] END OF FILE */
