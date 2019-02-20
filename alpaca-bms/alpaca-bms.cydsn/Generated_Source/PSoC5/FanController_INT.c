/*******************************************************************************
* File Name: FanController_INT.c
* Version 4.10
*
* Description:
*  This file contains the Interrupt Service Routines implementation for
*  Fan Controller component.
*
* Note:
*
*******************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "CyLib.h"
#include "FanController.h"



#if(FanController_IS_AUTO_FIRMWARE_MODE)
    volatile uint16 FanController_speedStatus = 0u;
#endif /* (FanController_IS_AUTO_FIRMWARE_MODE) */


#if(CY_PSOC4)
    /********************************************************************************
    *
    * Summary:
    *  Interrupt Service Routine for transferring actual speed from tachometer.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    * Reentrant:
    *  No
    *
    *******************************************************************************/
    CY_ISR(FanController_DATA_SEND_ISR)
    {
        uint8 fanNum;
        uint16 actSpeed;

        #ifdef FanController_DATA_SEND_ISR_ENTRY_CALLBACK
            FanController_DATA_SEND_ISR_EntryCallback();
        #endif /* FanController_DATA_SEND_ISR_ENTRY_CALLBACK */

        /* Get the number of active fan */
        fanNum = FanController_TACH_ADDR_CONTROL_REG & FanController_TACH_CNTL_NUM_FANS_MASK;
        
        /* Get measured actual speed period for a specific fan */
        actSpeed = CY_GET_REG16(FanController_TACH_STORED_ACT_PERIOD_PTR);
        
        /* Put down the measured speed to the proper location in RAM */
        FanController_fanControl.actualPeriod[fanNum] = actSpeed;

        /* Software stall detection */
        if(0u == actSpeed)
        {
            FanController_stallStatus |= (uint16) ((((uint16) 1u) << fanNum));
        }

        /* Calculate the new fan number value */
        fanNum = ((fanNum == 0u) ? ((uint8)(FanController_NUMBER_OF_FANS - 1u)) : (fanNum - 1u));
        
        /* Clear the previous fan number value in the control register */
        FanController_TACH_ADDR_CONTROL_REG &= ((uint8) ~FanController_TACH_CNTL_NUM_FANS_MASK);

        /* Set new fan number value and End-Of-Measurement bit if required */
        FanController_TACH_ADDR_CONTROL_REG |= 
                fanNum | ((fanNum == ((uint8)(FanController_NUMBER_OF_FANS - 1u))) ? FanController_TACH_EOM : 0u);

        /* In case if the last fan in the sequence was measured it is required
        * to generate the "eoc" pulse from Global Control Register.
        */
        if(((uint8)(FanController_NUMBER_OF_FANS - 1u)) == fanNum)
        {
            FanController_GLOBAL_CONTROL_REG |= FanController_SW_EOC;
            
            /* Also set stall bit in Global Control Register if any of the fans were stalled */
            if (0u != (FanController_stallStatus & FanController_alertMask))
            {
                FanController_GLOBAL_CONTROL_REG |= FanController_SW_STALL;
            }
        }
        
        #ifdef FanController_DATA_SEND_ISR_EXIT_CALLBACK
            FanController_DATA_SEND_ISR_ExitCallback();
        #endif /* FanController_DATA_SEND_ISR_EXIT_CALLBACK */
    }
#endif /* (CY_PSOC4) */


#if(FanController_IS_AUTO_FIRMWARE_MODE)
    /*******************************************************************************
    *
    * Summary:
    *  Interrupt Service Routine for implementation of PID Firmware Controlling
    *  algorithm.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    * Reentrant:
    *  No
    *
    * Theory:
    *  PID interrupt occurs in constant periods of time that is configured through
    *  the component customizer.
    *
    *******************************************************************************/
    CY_ISR(FanController_PID_CONTROL_ISR)
    {
        uint8 i;
        int16 pidErrorZ0;
        uint32 newCompare;
        int16 periodError;
        uint32 currActSpeed;
        uint32 tmpCompare;
        uint16 mask;

        /***************************************
        *   PID Parameters
        ***************************************/
        /* The error history */
        static int16 FanController_pidErrorZ1[FanController_NUMBER_OF_FANS] = {0};
        static int16 FanController_pidErrorZ2[FanController_NUMBER_OF_FANS] = {0};

        /* The compensator integrator */
        static uint32 FanController_pidArea[FanController_NUMBER_OF_FANS] = {0u};

        /* Holds result of PID algorithm execution for currently active fan */
        static uint16 FanController_pidOut;

        /* Fans speed regulation counters */
        static uint8 FanController_spdErrCount[FanController_NUMBER_OF_FANS] = {0u};

        #ifdef FanController_PID_CONTROL_ISR_ENTRY_CALLBACK
            FanController_PID_CONTROL_ISR_EntryCallback();
        #endif /* FanController_PID_CONTROL_ISR_ENTRY_CALLBACK */
        
        for(i = 0u; i < FanController_NUMBER_OF_FANS; i++)
        {
            /* Calculate the error in number of cycles of tachometer clock */
            #if(CY_PSOC3)
                if(0u != (FanController_DMA_STATUS_REG & FanController_DMA1))
                {
                    /* If DMA writes to actualPeriodDma1 (DMA1 state) then read actualPeriodDma2. */
                    currActSpeed =
                        (FanController_RPM_PERIOD_CONV_FACTOR / FanController_fanControl.actualPeriodDma2[i]);
                }
                else
                {
                    /* If DMA writes to actualPeriodDma2 (DMA2 state) then read actualPeriodDma1. */
                    currActSpeed =
                        (FanController_RPM_PERIOD_CONV_FACTOR / FanController_fanControl.actualPeriodDma1[i]);
                }
            #else
               currActSpeed =
                    (FanController_RPM_PERIOD_CONV_FACTOR / (uint32)FanController_fanControl.actualPeriod[i]);
            #endif /* (CY_PSOC3) */

            periodError = (int16) ((int32) FanController_desiredFansSpeed[i] - (int32) currActSpeed);

            /* The error in RPM */
            pidErrorZ0 = periodError;

            /* Saturate... Limit the error within the design range of the compensator */
            if(FanController_PID_ERROR_SAT_L > pidErrorZ0)
            {
                pidErrorZ0 = FanController_PID_ERROR_SAT_L;
            }
            else if(FanController_PID_ERROR_SAT_H < pidErrorZ0)
            {
                pidErrorZ0 = FanController_PID_ERROR_SAT_H;
            }
            else
            {
                /* Do nothing, the error is in range */
            }

            /* Process PID compensation */
            FanController_pidArea[i] = (uint32) ((int32) FanController_pidArea[i] +
                                            ((int32) pidErrorZ0 * (int32) FanController_pidA1[i]));
            FanController_pidArea[i] = (uint32) ((int32) FanController_pidArea[i] +
                                            ((int32) FanController_pidErrorZ1[i] *
                                            (int32) FanController_pidA2[i]));
            FanController_pidArea[i] = (uint32) ((int32) FanController_pidArea[i] +
                                            ((int32) FanController_pidErrorZ2[i] *
                                            (int32) FanController_pidA3[i]));

            /* Save error history */
            FanController_pidErrorZ2[i] = FanController_pidErrorZ1[i];
            FanController_pidErrorZ1[i] = pidErrorZ0;

            /* Saturate... Limit the error within the design range of the compensator */
            if(((int32) FanController_outputSatL[i]) > ((int32) FanController_pidArea[i]))
            {
                FanController_pidArea[i] = FanController_outputSatL[i];
            }
            else if(((int32) FanController_outputSatH[i]) < ((int32) FanController_pidArea[i]))
            {
                FanController_pidArea[i] = FanController_outputSatH[i];
            }
            else
            {
                /* Do nothing, the error is in range */
            }

            /* Get the algorithm result */
            FanController_pidOut = (uint16) (FanController_pidArea[i] >> FanController_PID_POST_GAIN);

            #if (!CY_PSOC4)
                /* Get compare from UDB PWMs */
                #if(FanController_PWMRES == FanController_PWMRES_EIGHTBIT)
                    tmpCompare = CY_GET_REG8(FanController_fanDriverRegs[i].pwmSetDutyReg);
                #else
                    tmpCompare = CY_GET_REG16(FanController_fanDriverRegs[i].pwmSetDutyReg);
                #endif /* FanController_PWMRES == FanController_PWMRES_EIGHTBIT */
            #else
                /* Get compare from TCPWMs */
                tmpCompare = CY_GET_REG32(FanController_fanDriverRegs[i].pwmSetDutyReg);
            #endif /* (!CY_PSOC4) */

            /* Assign new compare value for fas' PWM with PID out */
            newCompare = FanController_pidOut;

            if (0u == (FanController_GLOBAL_CONTROL_REG & FanController_OVERRIDE))
            {
                /* If override bit is cleared then set the newly tuned compare value */
                #if (!CY_PSOC4)
                    #if(FanController_PWMRES == FanController_PWMRES_EIGHTBIT)
                        CY_SET_REG8(FanController_fanDriverRegs[i].pwmSetDutyReg, (uint8) newCompare);
                    #else
                        CY_SET_REG16(FanController_fanDriverRegs[i].pwmSetDutyReg, (uint16) newCompare);
                    #endif /* FanController_PWMRES == FanController_PWMRES_EIGHTBIT */
                #else
                    CY_SET_REG32(FanController_fanDriverRegs[i].pwmSetDutyReg, newCompare);
                #endif /* (!CY_PSOC4) */
            }

            /*************************************
            * Speed regulation status
            **************************************/
            /* Analyze compare and result from PID */
            if ((FanController_PWM_PERIOD == tmpCompare) &&
                (currActSpeed < FanController_desiredFansSpeed[i]))
            {
                /* Increase speed failure counter */
                FanController_spdErrCount[i]++;
            }
            else if ((0u == tmpCompare) &&
                (currActSpeed > FanController_desiredFansSpeed[i]))
            {
                /* Avoid overflow */
                if (FanController_spdErrCount[i] < FanController_MAX_SPEED_ERRORS)
                {
                    /* Increase speed failure counter */
                    FanController_spdErrCount[i]++;
                }
            }
            else
            {
                /* Clear speed failure counter */
                FanController_spdErrCount[i] = 0u;
            }

            if (FanController_SPEED_ERRORS_TRESHOLD < FanController_spdErrCount[i])
            {
                /* The number of speed errors is above threshold so set the
                * respective speed error bit in the software speed status but
                * only if respective bit is set in the alert mask register.
                */
                mask = FanController_GetAlertMask();

                FanController_speedStatus = (uint16) ((((uint16) 1u) << i)) & mask;
            }
        }

        /* If software speed status register is not zero then simulate
        * hardware failure event. This will generate a pulse that goes to
        * Alert Status register.
        */
        if (0u !=FanController_speedStatus)
        {
            FanController_GLOBAL_CONTROL_REG |= FanController_SW_SPEED_ERROR;
        }

        #ifdef FanController_PID_CONTROL_ISR_EXIT_CALLBACK
            FanController_PID_CONTROL_ISR_ExitCallback();
        #endif /* FanController_PID_CONTROL_ISR_EXIT_CALLBACK */
    }
#endif /* (FanController_IS_AUTO_FIRMWARE_MODE) */


/* [] END OF FILE */
