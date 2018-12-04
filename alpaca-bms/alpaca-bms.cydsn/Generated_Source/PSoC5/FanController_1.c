/*******************************************************************************
* File Name: FanController_1.c
* Version 4.10
*
* Description:
*  This file contains the API files for the FanController component.
*  The FanController component supports up to 16 pulse width modulator (PWM)
*  controlled 4-wire fans and through an intuitive graphical interface,
*  enabling designers to quickly configure the number of fans, their electro-
*  mechanical properties, organization into banks (if desired) and the control
*  algorithm type: firmware or hardware based.
*
*  Refer to AN66627 "PSoC(R) 3 and PSoC 5 - Intelligent Fan Controller" for
*  more details and example projects.
*
* Note:
*
*******************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "FanController_1.h"
#include "CyLib.h"

#if(CY_PSOC4)
    #include "FanController_1_DataSend.h"

    #if(FanController_1_NUMBER_OF_FAN_OUTPUTS == 1u)
        #include "FanController_1_FW_PWM1.h"
    #elif(FanController_1_NUMBER_OF_FAN_OUTPUTS == 2u)
        #include "FanController_1_FW_PWM1.h"
        #include "FanController_1_FW_PWM2.h"
    #elif(FanController_1_NUMBER_OF_FAN_OUTPUTS == 3u)
        #include "FanController_1_FW_PWM1.h"
        #include "FanController_1_FW_PWM2.h"
        #include "FanController_1_FW_PWM3.h"
    #else
        #include "FanController_1_FW_PWM1.h"
        #include "FanController_1_FW_PWM2.h"
        #include "FanController_1_FW_PWM3.h"
        #include "FanController_1_FW_PWM4.h"
    #endif /* (FanController_1_NUMBER_OF_FAN_OUTPUTS == 1u) */

#endif /* (CY_PSOC4) */

#if(FanController_1_IS_AUTO_FIRMWARE_MODE)
    #include "FanController_1_PID_ISR.h"
#endif /* (FanController_1_IS_AUTO_FIRMWARE_MODE) */


/*******************************************************************************
* Variables
*
* Fan Control Data Structure declaration and initialization can be found at the
* end of this file due to its large size.
********************************************************************************/
uint8   FanController_1_initVar = 0u;

#if (CY_PSOC4)
    uint16 FanController_1_alertMask = 0u;
    uint16 FanController_1_stallStatus = 0u;
#else
    
    /* Stores channel number of Tachometer out DMA */
    static uint8 FanController_1_TachOutDMA_channel = 0u;

    #if(FanController_1_IS_AUTO_HARDWARE_MODE)
        
        /* Stores channel number of Tachometer in DMA */
        static uint8 FanController_1_TachInDMA_channel = 0u;

    #endif /* FanController_1_IS_AUTO_HARDWARE_MODE */

#endif /* (CY_PSOC4) */

/* Arrays for storing PID parameters and saturation ranges */
#if(FanController_1_IS_AUTO_FIRMWARE_MODE)
    int16 FanController_1_pidA1[FanController_1_NUMBER_OF_FANS];
    int16 FanController_1_pidA2[FanController_1_NUMBER_OF_FANS];
    int16 FanController_1_pidA3[FanController_1_NUMBER_OF_FANS];

    uint32 FanController_1_outputSatL[FanController_1_NUMBER_OF_FANS];
    uint32 FanController_1_outputSatH[FanController_1_NUMBER_OF_FANS];
#endif /* (FanController_1_IS_AUTO_FIRMWARE_MODE) */

#if(!FanController_1_IS_MANUAL_MODE)
    /* An array to store desired fans speeds */
    uint16 FanController_1_desiredFansSpeed[FanController_1_NUMBER_OF_FANS];
#endif /* (!FanController_1_IS_MANUAL_MODE) */


/*******************************************************************************
* Private API Prototypes
********************************************************************************/
#if (!CY_PSOC4)
    static void  FanController_1_SetupDMA(void) CYREENTRANT;
    static uint8 FanController_1_TachOutDMA_DmaInitialize(uint8 burstCount, uint8 requestPerBurst,
                                                    uint16 upperSrcAddress, uint16 upperDestAddress) CYREENTRANT;
    static void  FanController_1_TachOutDMA_DmaRelease(void) CYREENTRANT;

    #if(FanController_1_IS_AUTO_HARDWARE_MODE)

        static uint8 FanController_1_TachInDMA_DmaInitialize(uint8 burstCount, uint8 requestPerBurst,
                                                        uint16 upperSrcAddress, uint16 upperDestAddress) CYREENTRANT;
        static void  FanController_1_TachInDMA_DmaRelease(void) CYREENTRANT;

    #endif /* FanController_1_IS_AUTO_HARDWARE_MODE */

#endif /* (!CY_PSOC4) */


/******************************************************************************
* Function Name: FanController_1_Start
*******************************************************************************
*
* Summary:
*  Initializes component if not already initialized, then re-enables it.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  None
*
****************************************************************************/
void FanController_1_Start(void) 
{
    /* If not already initialized, then initialize hardware and variables */
    if(FanController_1_initVar == 0u)
    {
        FanController_1_Init();
        FanController_1_initVar = 1u;
    }
    FanController_1_Enable();
}


/******************************************************************************
* Function Name: FanController_1_Stop
*******************************************************************************
*
* Summary:
*  Stop the fan controller component.  Disables all hardware sub-components,
*  drives fan outputs high and de-asserts the alert pin.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  All PWM outputs to fans will be driven high (100% duty cycle).
*  Alert output de-asserted.
*
******************************************************************************/
void FanController_1_Stop(void) 
{
    /* Turn off the hardware sub-components */
    FanController_1_GLOBAL_CONTROL_REG = 0u;

    /* De-assert the alert pin */
    FanController_1_DisableAlert();

    #if (!CY_PSOC4)

        /* Release DMA resources */
        FanController_1_TachOutDMA_DmaRelease();

        #if(FanController_1_IS_AUTO_HARDWARE_MODE)
            FanController_1_TachInDMA_DmaRelease();
        #endif /* FanController_1_IS_AUTO_HARDWARE_MODE */

    #else

        /* Disable PWMs */
        #if(FanController_1_NUMBER_OF_FAN_OUTPUTS == 1u)
            FanController_1_FW_PWM1_Stop();
        #elif(FanController_1_NUMBER_OF_FAN_OUTPUTS == 2u)
            FanController_1_FW_PWM1_Stop();
            FanController_1_FW_PWM2_Stop();
        #elif(FanController_1_NUMBER_OF_FAN_OUTPUTS == 3u)
            FanController_1_FW_PWM1_Stop();
            FanController_1_FW_PWM2_Stop();
            FanController_1_FW_PWM3_Stop();
        #else
            FanController_1_FW_PWM1_Stop();
            FanController_1_FW_PWM2_Stop();
            FanController_1_FW_PWM3_Stop();
            FanController_1_FW_PWM4_Stop();
        #endif /* (FanController_1_NUMBER_OF_FAN_OUTPUTS == 1u) */

    #endif /* (!CY_PSOC4) */
}


/******************************************************************************
* Function Name: FanController_1_Init
*******************************************************************************
*
* Summary:
*  Sets up DMA channels and transaction descriptors. Configures PWMs and
*  Tachometer hardware blocks. Also it configures interrupts if they are used
*  in the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  Allocates DMA channels and transaction descriptors (TDs).
*
******************************************************************************/
void FanController_1_Init(void) 
{
    uint8 counti;
    uint8 interruptState;

    #if (!CY_PSOC4)
        
        /* Performs configuration of DMA used in the component */
        FanController_1_SetupDMA();

        /***********************************************************************
        * Initialize the PWMs
        ***********************************************************************/
        #if(FanController_1_IS_AUTO_HARDWARE_MODE)

            /* Configure hardware controlled PWMs */
            for (counti = 0u; counti < FanController_1_NUMBER_OF_FANS; counti++)
            {
                /* 8-bit hardware controlled PWM */
                #if(FanController_1_PWMRES == FanController_1_PWMRES_EIGHTBIT)
                    /* Set max PWM period */
                    CY_SET_REG8(FanController_1_fanDriverRegs[counti].pwmPeriodReg, FanController_1_PWM_PERIOD);
                    /* Set max PWM duty cycle (must be <= PWM period) */
                    CY_SET_REG8(FanController_1_fanDriverRegs[counti].pwmMaxDutyReg, FanController_1_PWM_PERIOD);
                /* 10-bit hardware controlled PWM */
                #else
                    /* Set max PWM period */
                    CY_SET_REG16(FanController_1_fanDriverRegs[counti].pwmPeriodReg, FanController_1_PWM_PERIOD);
                    /* Set max PWM duty cycle (must be <= PWM period) */
                    CY_SET_REG16(FanController_1_fanDriverRegs[counti].pwmMaxDutyReg, FanController_1_PWM_PERIOD);
                #endif /* FanController_1_PWMRES == FanController_1_PWMRES_EIGHTBIT */

                /* Set initial duty cycle */
                FanController_1_SetDutyCycle((counti + 1u), FanController_1_fanProperties[counti].initDuty);
                /* Set initial speed */
                FanController_1_SetDesiredSpeed((counti + 1u), FanController_1_fanProperties[counti].initRpm);

                /* Enable cnt7 to track speed regulation errors */
                CY_SET_REG8(FanController_1_fanDriverRegs[counti].errorCountReg,
                    CY_GET_REG8(FanController_1_fanDriverRegs[counti].errorCountReg) | FanController_1_COUNT7_ENABLE);
            }

        #else

            /* Configure firmware controlled PWMs */
            for (counti = 0u; counti < FanController_1_FANPWM_COUNT; counti++)
            {
                /* 8-bit firmware controlled PWM */
                #if (FanController_1_PWMRES == FanController_1_PWMRES_EIGHTBIT)
                    CY_SET_REG8(FanController_1_fanPwmInitRegs[counti].pwmPeriodReg, FanController_1_PWM_PERIOD);

                    interruptState = CyEnterCriticalSection();

                    CY_SET_REG8(FanController_1_fanPwmInitRegs[counti].pwmAuxControlReg,
                        CY_GET_REG8(FanController_1_fanPwmInitRegs[counti].pwmAuxControlReg) |
                            FanController_1_FANPWM_AUX_CTRL_FIFO0_CLR_8);

                    CyExitCriticalSection(interruptState);

                /* 10-bit firmware controlled PWM */
                #else
                    CY_SET_REG16(FanController_1_fanPwmInitRegs[counti].pwmPeriodReg, FanController_1_PWM_PERIOD);

                    interruptState = CyEnterCriticalSection();

                    CY_SET_REG16(FanController_1_fanPwmInitRegs[counti].pwmAuxControlReg,
                        CY_GET_REG16(FanController_1_fanPwmInitRegs[counti].pwmAuxControlReg) |
                            FanController_1_FANPWM_AUX_CTRL_FIFO0_CLR_10);

                    CyExitCriticalSection(interruptState);

                #endif /* FanController_1_PWMRES == FanController_1_PWMRES_EIGHTBIT */
            }

            for (counti = 0u; counti < FanController_1_NUMBER_OF_FAN_OUTPUTS; counti++)
            {
                /* Set Desired speed to initial RPM from the customizer */
                FanController_1_SetDesiredSpeed((counti + 1u), FanController_1_fanProperties[counti].initRpm);
            }

        #endif /* ((FanController_1_IS_AUTO_HARDWARE_MODE) || (FanController_1_IS_AUTO_FIRMWARE_MODE)) */

    #else

        /***********************************************************************
        * Configure the PWMs (TCPWMs)
        ***********************************************************************/

        /* Initialize PWMs */
        #if(FanController_1_NUMBER_OF_FAN_OUTPUTS == 1u)
            FanController_1_FW_PWM1_Init();
        #elif(FanController_1_NUMBER_OF_FAN_OUTPUTS == 2u)
            FanController_1_FW_PWM1_Init();
            FanController_1_FW_PWM2_Init();
        #elif(FanController_1_NUMBER_OF_FAN_OUTPUTS == 3u)
            FanController_1_FW_PWM1_Init();
            FanController_1_FW_PWM2_Init();
            FanController_1_FW_PWM3_Init();
        #else
            FanController_1_FW_PWM1_Init();
            FanController_1_FW_PWM2_Init();
            FanController_1_FW_PWM3_Init();
            FanController_1_FW_PWM4_Init();
        #endif /* (FanController_1_NUMBER_OF_FAN_OUTPUTS == 1u) */

        for (counti = 0u; counti < FanController_1_NUMBER_OF_FAN_OUTPUTS; counti++)
        {
            /* Set the hard-coded PWM period */
            CY_SET_REG32(FanController_1_fanPwmInitRegs[counti].pwmPeriodReg, FanController_1_PWM_PERIOD);

            #if(FanController_1_IS_AUTO_FIRMWARE_MODE)
                /* In Auto Firmware Mode it is required to set duty cycle directly on the
                * start.
                */
                FanController_1_SetDutyCycle((counti + 1u), FanController_1_fanProperties[counti].initDuty);
            #endif /* (FanController_1_IS_AUTO_FIRMWARE_MODE) */

            /* Set Desired speed to initial RPM from the customizer */
            FanController_1_SetDesiredSpeed((counti + 1u), FanController_1_fanProperties[counti].initRpm);
        }

    #endif /* (!CY_PSOC4) */

    /***********************************************************************
    * Initialize the Tachometer
    ***********************************************************************/

    interruptState = CyEnterCriticalSection();
    #if (CY_PSOC4)
        /* Clear the tachometer FIFO */
        FanController_1_TACH_CUNTER_AUX_CTL_REG |= FanController_1_TACH_COUNTER_FIFO_CLR;
        FanController_1_TACH_CUNTER_AUX_CTL_REG &= ((uint8) ~FanController_1_TACH_COUNTER_FIFO_CLR);
    #else
        /* Enable count7 hardware block to drive fan address */
        FanController_1_TACH_FAN_COUNTR_AUX_CTL_REG |= FanController_1_COUNT7_ENABLE;
    #endif /* (!CY_PSOC4) */
    (void)CyExitCriticalSection(interruptState);
    
    /* Initialize glitch filter counter */
    FanController_1_TACH_GLITCH_FILTER_PERIOD_REG = FanController_1_GLITCH_FILTER_VAL;

    #if((!FanController_1_IS_MANUAL_MODE) && (FanController_1_DAMPING_FACTOR > 0u))

        /* Set the divider by 10 */
        FanController_1_DAMPING_COUNTER_DIVIDER_REG = FanController_1_DAMPING_FACTOR_DIVIDER_VAL;

        /* Init lower 8 bits of 16-bit Damping Factor Counter */
         FanController_1_TACH_DAMPING_PERIOD_LOW_LSB_REG = FanController_1_DAMPING_FACTOR_PERIOD_LOW;

        /* Init high 8 bits of 16-bit Damping Factor Counter */
        FanController_1_TACH_DAMPING_PERIOD_HIGH_LSB_REG = FanController_1_DAMPING_FACTOR_PERIOD_HIGH;

    #endif /* ((!FanController_1_IS_MANUAL_MODE) && (FanController_1_DAMPING_FACTOR > 0u)) */

    /***********************************************************************
    * Initialize the Alert Mask Register based on customizer defaults
    ***********************************************************************/
    FanController_1_SetAlertMask(FanController_1_INIT_ALERT_MASK);

    #if(FanController_1_IS_AUTO_FIRMWARE_MODE)
        /***********************************************************************
        * Set PID coefficients for fans
        ***********************************************************************/
        for(counti = 0u; counti < FanController_1_NUMBER_OF_FANS; counti++)
        {
            /* Disable interrupts while updating PID constants */
            interruptState = CyEnterCriticalSection();

            /* Initialize both PID parameters and output saturation with
            * default values from the customizer.
            */
            FanController_1_pidA1[counti] = FanController_1_PID_A1;
            FanController_1_pidA2[counti] = FanController_1_PID_A2;
            FanController_1_pidA3[counti] = FanController_1_PID_A3;

            FanController_1_outputSatL[counti] = FanController_1_PID_OUTPUT_SAT_L;
            FanController_1_outputSatH[counti] = FanController_1_PID_OUTPUT_SAT_H;
            CyExitCriticalSection(interruptState);
        }
    #endif /* (FanController_1_IS_AUTO_FIRMWARE_MODE) */

    /***********************************************************************
    * Set up required interrupts
    ***********************************************************************/
    #if(CY_PSOC4)
        FanController_1_DataSend_StartEx(&FanController_1_DATA_SEND_ISR);
    #endif /* (CY_PSOC4) */

    #if(FanController_1_IS_AUTO_FIRMWARE_MODE)
        FanController_1_PID_ISR_StartEx(&FanController_1_PID_CONTROL_ISR);
    #endif /* (FanController_1_IS_AUTO_FIRMWARE_MODE) */
}


/******************************************************************************
* Function Name: FanController_1_Enable
*******************************************************************************
*
* Summary:
*  Enables hardware blocks in the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  None
*
****************************************************************************/
void FanController_1_Enable(void) 
{
    uint8 interruptState;

    #if(CY_PSOC4)
        /* Ehable PWMs */
        #if(FanController_1_NUMBER_OF_FAN_OUTPUTS == 1u)
            FanController_1_FW_PWM1_Enable();
        #elif(FanController_1_NUMBER_OF_FAN_OUTPUTS == 2u)
            FanController_1_FW_PWM1_Enable();
            FanController_1_FW_PWM2_Enable();
        #elif(FanController_1_NUMBER_OF_FAN_OUTPUTS == 3u)
            FanController_1_FW_PWM1_Enable();
            FanController_1_FW_PWM2_Enable();
            FanController_1_FW_PWM3_Enable();
        #else
            FanController_1_FW_PWM1_Enable();
            FanController_1_FW_PWM2_Enable();
            FanController_1_FW_PWM3_Enable();
            FanController_1_FW_PWM4_Enable();
        #endif /*(FanController_1_NUMBER_OF_FAN_OUTPUTS == 1u) */

    #endif /* (CY_PSOC4) */

    /* Write to control register based on customizer defaults */
    FanController_1_GLOBAL_CONTROL_REG = FanController_1_ENABLE | FanController_1_ALERT_PIN_ENABLE |
        ((uint8) (FanController_1_INIT_ALERT_ENABLE << FanController_1_ALERT_ENABLE_SHIFT));

    interruptState = CyEnterCriticalSection();

    /* Enable alerts from the Alert Status register */
    FanController_1_STATUS_ALERT_AUX_CTL_REG |= FanController_1_STATUS_ALERT_ENABLE;
    
    #if(!CY_PSOC4)
        
        /* Enable DMA channels for operation */
        (void) CyDmaChEnable(FanController_1_TachOutDMA_channel, 1u);
        
        #if(FanController_1_IS_AUTO_HARDWARE_MODE)
            (void) CyDmaChEnable(FanController_1_TachInDMA_channel, 1u);

        #endif /* FanController_1_IS_AUTO_HARDWARE_MODE */
    #endif /* !CY_PSOC4 */
        
    (void) CyExitCriticalSection(interruptState);
}


/******************************************************************************
* Function Name: FanController_1_EnableAlert
*******************************************************************************
*
* Summary:
*  Enables alerts from this component. Specifically which alert sources are
*  enabled is configured using SetAlertMode() API.  This API only
*  enables alert conditions to propagate to the Fan Controller component
*  Alert output pin.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  None
*
****************************************************************************/
void FanController_1_EnableAlert(void) 
{
    uint8 interruptState = CyEnterCriticalSection();

    FanController_1_GLOBAL_CONTROL_REG |= FanController_1_ALERT_PIN_ENABLE;

    (void) CyExitCriticalSection(interruptState);
}


/******************************************************************************
* Function Name: FanController_1_DisableAlert
*******************************************************************************
*
* Summary:
*  Disables alerts from this component. This API only disables alert
*  conditions from propagating to the fan controller component Alert output/pin
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  None
*
****************************************************************************/
void FanController_1_DisableAlert(void) 
{
    uint8 interruptState = CyEnterCriticalSection();

    FanController_1_GLOBAL_CONTROL_REG &= (uint8) ~FanController_1_ALERT_PIN_ENABLE;

    (void) CyExitCriticalSection(interruptState);
}


/******************************************************************************
* Function Name: FanController_1_SetAlertMode
*******************************************************************************
*
* Summary:
*  Enables alert sources from the component.
*
* Parameters:
*  alertMode:
*   b0:   STALL_ALERT_ENABLE (1==Enable, 0==Disable)
*   b1:   SPEED_ALERT_ENABLE (1==Enable, 0==Disable)
*   b7-2: RESERVED (write with zeroes)
*
* Return:
*  None
*
* Side Effects:
*  None
*
****************************************************************************/
void FanController_1_SetAlertMode(uint8 alertMode) 
{
    uint8 interruptState = CyEnterCriticalSection();

    FanController_1_GLOBAL_CONTROL_REG = (FanController_1_GLOBAL_CONTROL_REG &
            (uint8) ~FanController_1_ALERT_ENABLE_MASK) |
               ((uint8) (alertMode << FanController_1_ALERT_ENABLE_SHIFT) & FanController_1_ALERT_ENABLE_MASK);

    (void) CyExitCriticalSection(interruptState);
}


/******************************************************************************
* Function Name: FanController_1_GetAlertMode
*******************************************************************************
*
* Summary:
* Returns enabled alert sources from this component.
*
* Parameters:
*  None
*
* Return:
*  alertMode:
*   b0:   STALL_ALERT_ENABLE (1==Enable, 0==Disable)
*   b1:   SPEED_ALERT_ENABLE (1==Enable, 0==Disable)
*   b7-2: RESERVED (write with zeroes)
*
* Side Effects:
*  None
*
****************************************************************************/
uint8 FanController_1_GetAlertMode(void) 
{
    return((FanController_1_GLOBAL_CONTROL_REG & FanController_1_ALERT_ENABLE_MASK) >>
                FanController_1_ALERT_ENABLE_SHIFT);
}


/******************************************************************************
* Function Name: FanController_1_SetAlertMask
*******************************************************************************
*
* Summary:
*  Sets the per fan Alert Mask for both Stall and Speed errors.
*
* Parameters:
*  alertMask (1==Enable Alert, 0==Disable Alert)
*   b0:   FAN1 ALERT ENABLE/DISABLE
*   b1:   FAN2 ALERT ENABLE/DISABLE
*   ...
*   b15:  FAN16 ALERT ENABLE/DISABLE
*
* Return:
*  None
*
* Side Effects:
*  None
*
****************************************************************************/
void FanController_1_SetAlertMask(uint16 alertMask) 
{
    #if (CY_PSOC4)
        FanController_1_alertMask = alertMask;
    #else
        FanController_1_ALERT_MASK_LSB_CONTROL_REG = LO8(alertMask);

        #if(FanController_1_NUMBER_OF_FANS > 8u)
            FanController_1_ALERT_MASK_MSB_CONTROL_REG = HI8(alertMask);
        #endif /* FanController_1_NUMBER_OF_FANS > 8u */
    #endif /* (CY_PSOC4) */
}


/******************************************************************************
* Function Name: FanController_1_GetAlertMask
*******************************************************************************
*
* Summary:
*  Gets the current per fan Alert Mask for both Stall and Speed errors.
*
* Parameters:
*  None
*
* Return:
*  alertMask (1==Alert Enabled, 0==Alert Disabled)
*   b0:   FAN1 ALERT ENABLE/DISABLE
*   b1:   FAN2 ALERT ENABLE/DISABLE
*   ...
*   b15:  FAN16 ALERT ENABLE/DISABLE
*
* Side Effects:
*  None
*
****************************************************************************/
uint16 FanController_1_GetAlertMask(void) 
{
    uint16 alertMask;

    #if (CY_PSOC4)
        alertMask = FanController_1_alertMask;
    #else
        #if(FanController_1_NUMBER_OF_FANS > 8u)
            alertMask = (uint16)FanController_1_ALERT_MASK_LSB_CONTROL_REG |
                (uint16)((uint16)FanController_1_ALERT_MASK_MSB_CONTROL_REG << 8u);
        #else
            alertMask = (uint16)FanController_1_ALERT_MASK_LSB_CONTROL_REG;
        #endif /* FanController_1_NUMBER_OF_FANS > 8u */
    #endif /* (CY_PSOC4) */

    return(alertMask);
}


/******************************************************************************
* Function Name: FanController_1_GetAlertSource
*******************************************************************************
*
* Summary:
*  Returns pending alert sources. The alert pin is not de-asserted through
*  this API call. If this API call returns a non-zero value, call the
*  GetFanStallStatus() or GetFanSpeedStatus() APIs to determine the exact
*  source of the alert and simultaneously de-assert the pending alert and
*  alert pin.
*
* Parameters:
*  None
*
* Return:
*  alertSource:
*   b0:   STALL_ALERT (1==Present, 0==Not Present)
*   b1:   SPEED_ALERT (1==Present, 0==Not Present)
*   b7-2: RESERVED (returns all zeroes)
*
* Side Effects:
*  None
*
****************************************************************************/
uint8 FanController_1_GetAlertSource(void) 
{
    return ((FanController_1_ALERT_STATUS_REG & FanController_1_ALERT_STATUS_MASK));
}


/******************************************************************************
* Function Name: FanController_1_GetFanStallStatus
*******************************************************************************
*
* Summary:
*  Returns the stall status of all fans
*
* Parameters:
*  None
*
* Return:
*  stallStatus (1==Fan Stalled, 0=Fan OK)
*   b0:   FAN1 STALL
*   b1:   FAN2 STALL
*   ...
*   b15:  FAN16 STALL
*
* Side Effects:
*  Calling this API de-asserts the alert pin and clears all pending stall
*  status alerts.
*
******************************************************************************/
uint16 FanController_1_GetFanStallStatus(void) 
{
    uint16 stallStatus;

    #if (CY_PSOC4)
        stallStatus = FanController_1_stallStatus;

        /* Clear stall status for individual fans */
        FanController_1_stallStatus = 0u;

        /* Clear the common SW stall signal on the control register */
        FanController_1_GLOBAL_CONTROL_REG &= ((uint8) ~FanController_1_SW_STALL);
    #else
        
        #if(FanController_1_NUMBER_OF_FANS > 8u)
            stallStatus = (uint16)FanController_1_STALL_ERROR_LSB_STATUS_REG |
                (uint16)((uint16)FanController_1_STALL_ERROR_MSB_STATUS_REG << 8u);
        #else
            stallStatus = (uint16)FanController_1_STALL_ERROR_LSB_STATUS_REG;
        #endif /* FanController_1_NUMBER_OF_FANS > 8u */
    #endif /* (CY_PSOC4) */
        
    return(stallStatus);
}


#if (!FanController_1_IS_MANUAL_MODE)
    /******************************************************************************
    * Function Name: FanController_1_GetFanSpeedStatus
    *******************************************************************************
    *
    * Summary:
    *  Returns the speed regulation status of all fans.
    *  This API is not available if Firmware Controller fan control method is
    *  selected.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  speedStatus (1==Fan Speed Failure, 0=Fan OK)
    *   b0:   FAN1 SPEED REGULATION FAIL
    *   b1:   FAN2 SPEED REGULATION FAIL
    *   ...
    *   b15:  FAN16 SPEED REGULATION FAIL
    *
    * Side Effects:
    *  Calling this API de-asserts the alert pin and clears all pending speed
    *  regulation failure status alerts.
    *
    ******************************************************************************/
    uint16 FanController_1_GetFanSpeedStatus(void) 
    {
        uint16 speedStatus;

        /* In case of Auto Hardware mode speed status is received from hardware
        * status register...
        */
        #if (FanController_1_IS_AUTO_HARDWARE_MODE)
            #if (FanController_1_NUMBER_OF_FANS > 8u)
                speedStatus = (uint16)FanController_1_SPEED_ERROR_LSB_STATUS_REG |
                    (uint16)((uint16)FanController_1_SPEED_ERROR_MSB_STATUS_REG << 8u);
            #else
                speedStatus = (uint16)FanController_1_SPEED_ERROR_LSB_STATUS_REG;
            #endif /* FanController_1_NUMBER_OF_FANS > 8u */
        #endif /* FanController_1_IS_AUTO_HARDWARE_MODE */

        /* ... in case of Auto Firmware mode the software status is used. */
        #if (FanController_1_IS_AUTO_FIRMWARE_MODE)

            speedStatus = FanController_1_speedStatus;

            /* Simulate clear-on-read status */
            FanController_1_speedStatus = 0u;
        #endif /* (FanController_1_IS_AUTO_FIRMWARE_MODE) */

        return(speedStatus);
    }

#endif /* (!FanController_1_IS_MANUAL_MODE) */


/******************************************************************************
* Function Name: FanController_1_SetDutyCycle
*******************************************************************************
*
* Summary:
*  Sets the duty cycle of selected fan or bank in hundredths of a percent,
*  i.e. 5000=50% duty cycle. In hardware controlled fan mode, if manual duty
*  cycle control is desirable, call the OverrideAutomaticControl(true) API 
*  prior to calling this API.
*
* Parameters:
*  uint8 fanOrBankNumber
*   Valid range is 1-16 and should not exceed the number of fans or banks in the
*   system.
*
*  uint16 dutyCycle
*   dutyCycle is entered in hundredths of a percent, i.e. 5000=50% duty cycle
*   Valid range is 0 to 10000.
*
* Return:
*  None
*
* Side Effects:
*  None
*
****************************************************************************/
void FanController_1_SetDutyCycle(uint8 fanOrBankNumber, uint16 dutyCycle)
        
{
    uint32 newCompare;      /* Needs to be 32-bit to allow for overflow during the math calculations */

    /* Make sure the Fan/Bank number is valid */
    if(fanOrBankNumber <= FanController_1_NUMBER_OF_FAN_OUTPUTS)
    {
        newCompare = (FanController_1_PWM_PERIOD * (uint32) dutyCycle) / FanController_1_PWM_DUTY_DIVIDER;

        /* Make sure the Compare value is in range (user entered a valid duty
        * cycle value).
        */
        if(newCompare <= FanController_1_PWM_PERIOD)
        {
            #if (!CY_PSOC4)
                #if(FanController_1_PWMRES == FanController_1_PWMRES_EIGHTBIT)
                    CY_SET_REG8(FanController_1_fanDriverRegs[fanOrBankNumber - 1u].pwmSetDutyReg, (uint8)newCompare);
                #else
                    CY_SET_REG16(FanController_1_fanDriverRegs[fanOrBankNumber - 1u].pwmSetDutyReg, (uint16)newCompare);
                #endif /* FanController_1_PWMRES == FanController_1_PWMRES_EIGHTBIT */
            #else
                CY_SET_REG32(FanController_1_fanDriverRegs[fanOrBankNumber - 1u].pwmSetDutyReg, newCompare);
            #endif /* (!CY_PSOC4) */
        }
    }
}


/******************************************************************************
* Function Name: FanController_1_GetDutyCycle
*******************************************************************************
*
* Summary:
*  Returns the current duty cycle of the selected fan or bank in hundredths
*  of a percent, i.e. 5000=50% duty cycle.
*
* Parameters:
*  uint8 fanOrBankNumber
*  Valid range is 1-16 and should not exceed the number of fans or banks in
*  the system.
*
* Return:
*  uint16 duyCycle
*  Current duty cycle in hundredths of a percent.
*
* Side Effects:
*  None
*
****************************************************************************/
uint16 FanController_1_GetDutyCycle(uint8 fanOrBankNumber) 
{
    uint16 duyCycle = 0u;

    if(fanOrBankNumber <= FanController_1_NUMBER_OF_FAN_OUTPUTS)
    {
        #if (!CY_PSOC4)
            #if(FanController_1_PWMRES == FanController_1_PWMRES_EIGHTBIT)
                duyCycle = (uint16)
                    ((((uint32) CY_GET_REG8(FanController_1_fanDriverRegs[fanOrBankNumber - 1u].pwmSetDutyReg))
                            * FanController_1_PWM_DUTY_DIVIDER) / FanController_1_PWM_PERIOD);
            #else
                duyCycle = (uint16)
                    (((uint32) CY_GET_REG16(FanController_1_fanDriverRegs[fanOrBankNumber - 1u].pwmSetDutyReg)
                                * FanController_1_PWM_DUTY_DIVIDER) / FanController_1_PWM_PERIOD);
            #endif /* FanController_1_PWMRES == FanController_1_PWMRES_EIGHTBIT */
        #else
            duyCycle = (uint16)
                (((uint32) CY_GET_REG32(FanController_1_fanDriverRegs[fanOrBankNumber - 1u].pwmSetDutyReg)
                        * FanController_1_PWM_DUTY_DIVIDER) / FanController_1_PWM_PERIOD);
        #endif /* (!CY_PSOC4) */
    }

    return(duyCycle);
}


/******************************************************************************
* Function Name: FanController_1_SetDesiredSpeed
*******************************************************************************
*
* Summary:
*  Sets the desired RPM of specified fan. If the AcousticNoiseReduction
*  parameter is true (enabled via the component customizer) in hardware
*  controlled fan mode, the desired speed is converted to a fan rotation period
*  and written to SRAM for subsequent DMA into the tachometer block. If the
*  AcousticNoiseReduction parameter is false, the desired speed is converted to
*  a duty cycle and written directly into the PWM register for the selected fan.
*
* Parameters:
*  uint8 fanNumber
*  Valid range is 1-16 and should not exceed the number of fans in the system.
*
* uint16 rpm
*  Valid range is 500 to 25,000, but should not exceed the max RPM of the
*  selected fan.
*
* Return:
*  None
*
* Side Effects:
*  None
*
****************************************************************************/
void FanController_1_SetDesiredSpeed(uint8 fanNumber, uint16 rpm) 
{
    uint32 overrideDuty;        /* Needs to be 32-bit to allow for overflow during the math calculations */
    uint16 currentSpeed;

    /* Check for valid Fan number */
    if(fanNumber <= FanController_1_NUMBER_OF_FANS)
    {
        /* Get the actual speed */
        currentSpeed = FanController_1_GetActualSpeed(fanNumber);

        /* 3 cases where we will directly write a new duty cycle in firmware: */
            /* Case 1) Manual Control fan mode */
        if((FanController_1_IS_MANUAL_MODE)   ||
            /* Case 2) Auto controlled fan mode and noise reduction is off */
            ((FanController_1_IS_AUTO_HARDWARE_MODE) &&
                (FanController_1_NOISE_REDUCTION_MODE == FanController_1_NOISE_REDUCTION_OFF)) ||
            /* Case 3) hardware controlled fan mode and new rpm is slower than current */
            ((FanController_1_IS_AUTO_HARDWARE_MODE) && (rpm < currentSpeed)))
        {
            /* Override hardware controlled fan mode temporarily to write new duty cycle */
            #if(FanController_1_IS_AUTO_HARDWARE_MODE)
                FanController_1_OverrideAutomaticControl(1u);
            #endif /* FanController_1_IS_AUTO_HARDWARE_MODE */

            /* Desired speed is slower than rpmA */
            if(rpm < FanController_1_fanProperties[fanNumber - 1u].rpmA)
            {
                overrideDuty =  (uint32)FanController_1_fanProperties[fanNumber - 1u].dutyA -
                                (((uint32)FanController_1_fanProperties[fanNumber - 1u].dutyRpmSlope *
                                   ((uint32)FanController_1_fanProperties[fanNumber - 1u].rpmA - (uint32)rpm)) / 100u);

                /* Check for math underflow */
                if(overrideDuty > 10000u)
                {
                    overrideDuty = 0u;
                }
            }

            /* Desired speed is faster than rpmA */
            else
            {
                overrideDuty =  (uint32)FanController_1_fanProperties[fanNumber - 1u].dutyA +
                                (((uint32)FanController_1_fanProperties[fanNumber - 1u].dutyRpmSlope *
                                   ((uint32)rpm - (uint32)FanController_1_fanProperties[fanNumber - 1u].rpmA)) / 100u);

                /* Check for math overflow */
                if(overrideDuty > 10000u)
                {
                    overrideDuty = 10000u;
                }
            }

            /* Set the newly calculated duty cycle */
            FanController_1_SetDutyCycle(fanNumber, (uint16) overrideDuty);
        }

        /* Set newly requested desired rpm including tolerance calculation and store
        * in SRAM for the tachometer block DMA controller.
        */
        #if(FanController_1_IS_AUTO_HARDWARE_MODE)

            FanController_1_fanControl.desiredPeriod[fanNumber - 1u] =
                (uint16) (FanController_1_RPM_PERIOD_CONV_FACTOR /
                    (uint32) ((uint32) rpm + (((uint32) rpm * FanController_1_TOLERANCE) /
                        FanController_1_TOLERANCE_DIVIDER)));

            if(rpm > 500u)
            {
                FanController_1_fanControl.tolerance[fanNumber - 1u] =
                    (uint16) (((FanController_1_RPM_PERIOD_CONV_FACTOR * FanController_1_TOLERANCE) /
                        FanController_1_TOLERANCE_DIVIDER) / (uint32) rpm);
            }
            else
            {
                FanController_1_fanControl.tolerance[fanNumber - 1u] = 100u;
            }

            /* Go back to hardware controlled fan mode */
            FanController_1_OverrideAutomaticControl(0u);

        #endif /* FanController_1_IS_AUTO_HARDWARE_MODE */
        
        /* Store the desired speed to RAM if we are in auto mode. It will be used by 
        * FanController_1_GetDesiredSpeed() and PID algorithm.
        */
        #if((FanController_1_IS_AUTO_FIRMWARE_MODE) || (FanController_1_IS_AUTO_HARDWARE_MODE))
            FanController_1_desiredFansSpeed[fanNumber - 1u] = rpm;
        #endif /* (FanController_1_IS_AUTO_HARDWARE_MODE) */
    }
}


#if (!FanController_1_IS_MANUAL_MODE)
    /******************************************************************************
    * Function Name: FanController_1_GetDesiredSpeed
    *******************************************************************************
    *
    * Summary:
    *  Returns the currently desired speed of the selected fan in RPMs in hardware
    *  controlled fan mode. This API is not available if firmware controlled fan
    *  mode is selected.
    *
    * Parameters:
    *  uint8 fanNumber
    *  Valid range is 1-16 and should not exceed the number of fans in the system.
    *
    * Return:
    *  uint16 desiredSpeed: Current desired speed for selected fan.
    *
    * Side Effects:
    *  None
    *
    ******************************************************************************/
    uint16 FanController_1_GetDesiredSpeed(uint8 fanNumber) 
    {
        return(FanController_1_desiredFansSpeed[fanNumber - 1u]);
    }

#endif /* (!FanController_1_IS_MANUAL_MODE) */


/******************************************************************************
* Function Name: FanController_1_GetActualSpeed
*******************************************************************************
*
* Summary:
*  Returns the actual measured RPM of selected fan. This function should be 
*  called first time in the design only after the requested fan is made a full
*  rotation. This can be ensured by calling the function after End-of-Cycle 
*  (eoc) pulse generation.
*
* Parameters:
*  uint8 fanNumber
*  Valid range is 1-16 and should not exceed the number of fans in the system.
*
* Return:
*  uint16 actualSpeed: Actual measured RPM of target fan.
*
* Side Effects:
*  None
*
****************************************************************************/
uint16 FanController_1_GetActualSpeed(uint8 fanNumber) 
{
    uint16 actualSpeed = 0u;

    if(fanNumber <= FanController_1_NUMBER_OF_FANS)
    {
        #if(CY_PSOC3)

            if(0u != (FanController_1_DMA_STATUS_REG & FanController_1_DMA1))
            {
                /* If DMA writes to actualPeriodDma1 (DMA1 state) then read
                * actualPeriodDma2.
                */
                if(FanController_1_fanControl.actualPeriodDma2[fanNumber - 1u] != 0u)
                {
                    /* Calculate RPM from value measured by tachometer counter */
                    actualSpeed = (uint16) (FanController_1_RPM_PERIOD_CONV_FACTOR /
                        (uint32) FanController_1_fanControl.actualPeriodDma2[fanNumber - 1u]);
                }
            }
            else
            {
                /* If DMA writes to actualPeriodDma2 (DMA2 state) then read
                * actualPeriodDma1.
                */
                if(FanController_1_fanControl.actualPeriodDma1[fanNumber - 1u] != 0u)
                {
                    /* Calculate RPM from value measured bu tachometer counter */
                    actualSpeed = (uint16) (FanController_1_RPM_PERIOD_CONV_FACTOR /
                        (uint32) FanController_1_fanControl.actualPeriodDma1[fanNumber - 1u]);
                }
            }

        #else

            if(FanController_1_fanControl.actualPeriod[fanNumber - 1u] != 0u)
            {
                /* Calculate RPM from value measured by tachometer counter */
                actualSpeed = (uint16) (FanController_1_RPM_PERIOD_CONV_FACTOR /
                    (uint32) FanController_1_fanControl.actualPeriod[fanNumber - 1u]);
            }

        #endif /* (CY_PSOC3) */
    }

    return(actualSpeed);
}


#if (FanController_1_IS_AUTO_HARDWARE_MODE)
    /******************************************************************************
    * Function Name: FanController_1_OverrideAutomaticControl
    *******************************************************************************
    *
    * Summary:
    *  Allows firmware to take over fan control when hardware controlled fan mode
    *  is enabled. That is, directly control fan speed using the SetDutyCycle() API.
    *  This API is not available if firmware controlled fan mode is selected.
    *
    * Parameters:
    *  uint8 override
    *   non-zero = firmware assumes control of fans
    *   zero     = hardware assumes control of fans
    *
    * Return:
    *  None
    *
    * Side Effects:
    *  None
    ******************************************************************************/

    void FanController_1_OverrideAutomaticControl(uint8 override)
                                
    {
        uint8 interruptState = CyEnterCriticalSection();

        if(override != 0u)
        {
            FanController_1_GLOBAL_CONTROL_REG |= FanController_1_OVERRIDE;
        }
        else
        {
            FanController_1_GLOBAL_CONTROL_REG &= ((uint8) ~FanController_1_OVERRIDE);
        }

        CyExitCriticalSection(interruptState);
    }

#endif /* (FanController_1_IS_AUTO_HARDWARE_MODE) */


#if(FanController_1_IS_AUTO_FIRMWARE_MODE)
    /******************************************************************************
    * Function Name: FanController_1_SetSaturation
    *******************************************************************************
    *
    * Summary:
    *  Changes the PID controller output saturation. This bounds the output PWM to
    *  the fan and prevents what is known as integrator windup.
    *
    * Parameters:
    *  uint8 fanNum : Valid range is 1..16 but should not exceed the number of fans
    *                 in the system.
    *  uint16 satH  : The upper threshold for saturation. Valid range is 0 to
    *                 65535. A value of 0 represents 0% of the duty cycle. A value
    *                 of 65535 represents 100% duty cycle.
    *
    *  uint16 satL  : The lower threshold for saturation. Valid range is 0 to
    *                 65535. A value of 0 represents 0% of the duty cycle. A value
    *                 of 65535 represents 100% duty cycle.
    *
    * Return:
    *  None
    *
    * Side Effects:
    *  None
    *
    ****************************************************************************/
    void FanController_1_SetSaturation(uint8 fanNum, uint16 satH, uint16 satL)
                                                    
    {
        uint8 interruptState;
        uint32 tmpOutL;
        uint32 tmpOutH;

        if((fanNum - 1u) < FanController_1_NUMBER_OF_FANS)
        {
            /* Calculate PID Constants for fan and store them into temporary variables to
            * be later written to PID vars. This is done so to not disable interrupt for a
            * long time.
            */
            tmpOutH = ((FanController_1_PWM_PERIOD * ((uint32) (((uint32) 1u) << FanController_1_PID_POST_GAIN))) /
                                           FanController_1_MAX_GAIN) * satH;
            tmpOutL = ((FanController_1_PWM_PERIOD * ((uint32) (((uint32) 1u) << FanController_1_PID_POST_GAIN))) /
                                           FanController_1_MAX_GAIN) * satL;

            /* Disable interrupts while updating PID constants */
            interruptState = CyEnterCriticalSection();

            FanController_1_outputSatH[fanNum - 1u] = tmpOutH;
            FanController_1_outputSatL[fanNum - 1u] = tmpOutL;

            CyExitCriticalSection(interruptState);
        }
    }


    /******************************************************************************
    * Function Name: FanController_1_SetPID
    *******************************************************************************
    *
    * Summary:
    *  Changes the PID controller coefficients for the controlled fan. The
    *  coefficients are integers that are proportional to the gain.
    *
    * Parameters:
    *  uint8 fanNum : Valid range is 1..16 but should not exceed the number of fans
    *                 in the system.
    *  uint16 kp    : Proportional gain. Valid range is 0 to 65535. A value of 0
    *                 represents 0% gain. A value of 65535 represents 100% gain.
    *
    *  uint16 ki    : Integral gain. Valid range is 0 to 65535. A value of 0
    *                 represents 0% gain. A value of 65535 represents 100% gain.
    *
    *  uint16 kd    : Derivative gain. Valid range is 0 to 65535. A value of 0
    *                 represents 0% gain. A value of 65535 represents 100% gain.
    *
    * Return:
    *  None
    *
    * Side Effects:
    *  None
    *
    ****************************************************************************/
    void FanController_1_SetPID(uint8 fanNum, uint16 kp, uint16 ki, uint16 kd)
                                                                        
    {
        uint8 interruptState;
        /* Variables for temporary storing of a calculated PID constants */
        int16 tmpA1;
        int16 tmpA2;
        int16 tmpA3;

        if((fanNum - 1u) < FanController_1_NUMBER_OF_FANS)
        {
            /* Calculate PID Constants for fan and store them into temporary variables to
            * be later written to PID vars. This is done so to not disable interrupt for a
            * long time.
            */
            tmpA1 = (int16) ((((int32) kp + (int32) ki + (int32) kd) * ((int32) FanController_1_GAIN_MULTIPLIER)) /
                    (int32) FanController_1_MAX_GAIN);
            tmpA2 = (int16) (((-((int32) kp + (2 * (int32) kd))) * ((int32) FanController_1_GAIN_MULTIPLIER )) /
                    (int32) FanController_1_MAX_GAIN);
            tmpA3 = (int16) ((((int32) kd) * ((int32) FanController_1_GAIN_MULTIPLIER )) /
                    (int32) FanController_1_MAX_GAIN);

            /* Disable interrupts while updating PID constants */
            interruptState = CyEnterCriticalSection();

            FanController_1_pidA1[fanNum - 1u] = tmpA1;
            FanController_1_pidA2[fanNum - 1u] = tmpA2;
            FanController_1_pidA3[fanNum - 1u] = tmpA3;

            CyExitCriticalSection(interruptState);
        }
    }


#endif /* (FanController_1_IS_AUTO_FIRMWARE_MODE) */

#if (!CY_PSOC4)

    /******************************************************************************
    * Function Name: FanController_1_SetupDMA
    *******************************************************************************
    *
    * Summary:
    *  This is a private API not exposed to users.
    *
    *  Sets up the DMA controllers depending on firmware or hardware controlled fan
    *  mode. The number and the sequence of the transaction descriptors depends on
    *  the number of fans in the system.
    *
    *  The NRQ output of the DMA controllers is used in different ways depending
    *  on the fan control mode:
    *
    *  1) In firmware controlled fan mode, the NRQ of the TachOutDMA is asserted
    *     only once at the end of the TD chain. This is used to generate the
    *     end-of-cylce (eoc) pulse for the component.
    *
    *  2) In hardware controlled fan mode, the NRQ of the TachOutDMA is asserted
    *     after every TD and connects to the DRQ of the TachInDMA. This ensures
    *     that reading desired speeds and hysteresis into the tachometer block
    *     occurs automatically when the fan input is changed. In this
    *     configuration, the NRQ output of the TachInDMA is used to generate the
    *     end-of-cylce (eoc) pulse for the component.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    * Side Effects:
    *  None
    *
    ****************************************************************************/
    static void FanController_1_SetupDMA(void) CYREENTRANT
    {
        uint8 counti;
        uint8 fanNum;
        
        /* DMA transfer descriptors used by the tachometer block for fan speed 
        * control/reporting.
        */
        static FanController_1_fanTdOutStruct FanController_1_fanOutTds[FanController_1_NUM_OUT_TDS];
    
        #if(FanController_1_IS_AUTO_HARDWARE_MODE)

            /* DMA transfer descriptors used by the tachometer block for fan speed 
            * tunning. 
            */
            static FanController_1_fanTdInStruct FanController_1_fanInTds[FanController_1_NUMBER_OF_FANS];

        #endif /* FanController_1_IS_AUTO_HARDWARE_MODE */

        /* Get DMA controller channels allocated */
        /* PSoC 3 family memory spaces */
        #if defined(__C51__)

            FanController_1_TachOutDMA_channel =   FanController_1_TachOutDMA_DmaInitialize(
                                                    2u,
                                                    1u,
                                                    HI16(CYDEV_PERIPH_BASE),
                                                    HI16(CYDEV_SRAM_BASE));

            #if (FanController_1_IS_AUTO_HARDWARE_MODE)

                FanController_1_TachInDMA_channel =    FanController_1_TachInDMA_DmaInitialize(
                                                        2u,
                                                        1u,
                                                        HI16(CYDEV_SRAM_BASE),
                                                        HI16(CYDEV_PERIPH_BASE));
            #endif /* FanController_1_IS_AUTO_HARDWARE_MODE */

        /* PSoC 5 family memory spaces */
        #else

            FanController_1_TachOutDMA_channel =   FanController_1_TachOutDMA_DmaInitialize(
                                                    2u,
                                                    1u,
                                                    HI16(FanController_1_TACH_ACTUAL_PERIOD_PTR),
                                                    HI16(((uint32)&FanController_1_fanControl.actualPeriod[0])));

            #if (FanController_1_IS_AUTO_HARDWARE_MODE)

                FanController_1_TachInDMA_channel =    FanController_1_TachInDMA_DmaInitialize(
                                                        2u,
                                                        1u,
                                                        HI16(((uint32)&FanController_1_fanControl.desiredPeriod[0])),
                                                        HI16(FanController_1_TACH_DESIRED_PERIOD_PTR));
            #endif /* FanController_1_IS_AUTO_HARDWARE_MODE */
        #endif /* __C51__ */

        /* Get transaction descriptors allocated */
        for (counti = 0u; counti < FanController_1_NUMBER_OF_FANS; counti++)
        {
            #if (CY_PSOC3)
                /* For PSoC 3 it is required to allocate two times more TDs */
                FanController_1_fanOutTds[counti].setActualPeriodTD = CyDmaTdAllocate();
                FanController_1_fanOutTds[counti + FanController_1_NUMBER_OF_FANS].setActualPeriodTD =
                        CyDmaTdAllocate();
            #else
                FanController_1_fanOutTds[counti].setActualPeriodTD = CyDmaTdAllocate();
            #endif /* (CY_PSOC3) */

            #if(FanController_1_IS_AUTO_HARDWARE_MODE)
                FanController_1_fanInTds[counti].getDesiredPeriodTD = CyDmaTdAllocate();
                FanController_1_fanInTds[counti].getToleranceTD = CyDmaTdAllocate();
            #endif /* FanController_1_IS_AUTO_HARDWARE_MODE */
        }

        /* Configure the transaction descriptors and sequence depending on fan control mode */
        for (counti = 0u; counti < FanController_1_NUMBER_OF_FANS; counti++)
        {
            fanNum = FanController_1_NUMBER_OF_FANS - counti - 1u;

            /* Put current actual tachometer periods (hardware and firmware
            * controlled fan modes) to SRAM for the GetActualSpeed API.
            */
            if(fanNum == 0u)
            {
                /* TDs need to be sequenced in reverse to match the Tachometer hardware
                * block address bus sequence. Next TD after fan[0] is final fan:
                fan[NUMBER_OF_FANS-1].
                */
                #if (CY_PSOC3)

                    /* For PSoC 3 the DMA can write to two RAM locations and therefore it
                    * uses two times more TDs. The configuration is a bit complex.
                    * Short explanation is for a set of 16 fans the TD[0] will be chained to
                    * TD[31] and TD[16] will be chained to TD[15]. It's because TD[0] to TD[15]
                    * are used by DMA2 and TD[16] to TD[31] are used by DMA1. DMA1 will write
                    * measured data to .actualPeriodDma1[] and DMA2 will write to
                    * .actualPeriodDma2[].
                    */
                    (void) CyDmaTdSetConfiguration(FanController_1_fanOutTds[fanNum].setActualPeriodTD,
                                            2u,
                                            FanController_1_fanOutTds[
                            (FanController_1_NUMBER_OF_FANS +FanController_1_NUMBER_OF_FANS) - 1u].setActualPeriodTD,
                                            TD_INC_DST_ADR | FanController_1_TD_SWAP_ENDIAN_FLAG  |
                                            /* Assert NRQ at final TD in hardware and firmware controlled fan modes */
                                            FanController_1_TachOutDMA__TD_TERMOUT_EN);

                    (void) CyDmaTdSetConfiguration(
                                FanController_1_fanOutTds[FanController_1_NUMBER_OF_FANS].setActualPeriodTD,
                                2u,
                                FanController_1_fanOutTds[FanController_1_NUMBER_OF_FANS - 1u].setActualPeriodTD,
                                TD_INC_DST_ADR | FanController_1_TD_SWAP_ENDIAN_FLAG  |
                                /* Assert NRQ at final TD in hardware and firmware controlled fan modes */
                                FanController_1_TachOutDMA__TD_TERMOUT_EN);

                #else

                    /* For PSoC 5LP or PSoC 4 there is no need of two TD sets */
                    (void) CyDmaTdSetConfiguration(FanController_1_fanOutTds[fanNum].setActualPeriodTD,
                                    2u,
                                    FanController_1_fanOutTds[FanController_1_NUMBER_OF_FANS - 1u].setActualPeriodTD,
                                    TD_INC_DST_ADR | FanController_1_TD_SWAP_ENDIAN_FLAG  |
                                    /* Assert NRQ at final TD in hardware and firmware controlled fan modes */
                                    FanController_1_TachOutDMA__TD_TERMOUT_EN);

                #endif /* (CY_PSOC3) */
            }
            else
            {
                /* TDs need to be sequenced in reverse to match the Tachometer hardware
                * block address bus sequence. Next TD after fan[n] is fan[n-1].
                */
                #if (FanController_1_IS_AUTO_HARDWARE_MODE)
                    #if (CY_PSOC3)

                        (void) CyDmaTdSetConfiguration(FanController_1_fanOutTds[fanNum].setActualPeriodTD,
                                    2u,
                                    FanController_1_fanOutTds[fanNum - 1u].setActualPeriodTD,
                                    TD_INC_DST_ADR | FanController_1_TD_SWAP_ENDIAN_FLAG  |
                                    /* Also assert NRQ at end of each TD in hardware controlled fan mode */
                                    FanController_1_TachOutDMA__TD_TERMOUT_EN);

                        (void) CyDmaTdSetConfiguration(
                            FanController_1_fanOutTds[FanController_1_NUMBER_OF_FANS + fanNum].setActualPeriodTD,
                            2u,
                            FanController_1_fanOutTds[
                                (FanController_1_NUMBER_OF_FANS + fanNum) - 1u].setActualPeriodTD,
                            TD_INC_DST_ADR | FanController_1_TD_SWAP_ENDIAN_FLAG  |
                            /* Also assert NRQ at end of each TD in hardware controlled fan mode */
                            FanController_1_TachOutDMA__TD_TERMOUT_EN);

                    #else

                        (void) CyDmaTdSetConfiguration(FanController_1_fanOutTds[fanNum].setActualPeriodTD,
                                    2u,
                                    FanController_1_fanOutTds[fanNum - 1u].setActualPeriodTD,
                                    TD_INC_DST_ADR | FanController_1_TD_SWAP_ENDIAN_FLAG  |
                                    /* Also assert NRQ at end of each TD in hardware controlled fan mode */
                                    FanController_1_TachOutDMA__TD_TERMOUT_EN);

                    #endif /* (CY_PSOC3) */

                #else
                    #if (CY_PSOC3)

                        (void) CyDmaTdSetConfiguration(FanController_1_fanOutTds[fanNum].setActualPeriodTD,
                                    2u,
                                    FanController_1_fanOutTds[fanNum - 1u].setActualPeriodTD,
                                    TD_INC_DST_ADR |
                                    /* Don't assert NRQ at end of each TD in firmware controlled fan mode */
                                    FanController_1_TD_SWAP_ENDIAN_FLAG);

                        (void) CyDmaTdSetConfiguration(
                            FanController_1_fanOutTds[FanController_1_NUMBER_OF_FANS + fanNum].setActualPeriodTD,
                            2u,
                            FanController_1_fanOutTds[
                                (FanController_1_NUMBER_OF_FANS + fanNum) - 1u].setActualPeriodTD,
                            TD_INC_DST_ADR |
                            /* Don't assert NRQ at end of each TD in firmware controlled fan mode */
                            FanController_1_TD_SWAP_ENDIAN_FLAG);

                    #else

                        (void) CyDmaTdSetConfiguration(FanController_1_fanOutTds[fanNum].setActualPeriodTD,
                                    2u,
                                    FanController_1_fanOutTds[fanNum - 1u].setActualPeriodTD,
                                    TD_INC_DST_ADR |
                                    /* Don't assert NRQ at end of each TD in firmware controlled fan mode */
                                    FanController_1_TD_SWAP_ENDIAN_FLAG);

                    #endif /* (CY_PSOC3) */
                #endif /* FanController_1_IS_AUTO_HARDWARE_MODE */
            }

            /* Get desired tachometer periods and hysteresis (hardware controlled fan mode
            * only) from SRAM from the SetActualSpeed API.
            */
            #if (FanController_1_IS_AUTO_HARDWARE_MODE)

                /* Get desired tachometer periods */
                (void) CyDmaTdSetConfiguration(FanController_1_fanInTds[fanNum].getDesiredPeriodTD,
                                    2u,
                                    FanController_1_fanInTds[fanNum].getToleranceTD,
                                    TD_INC_SRC_ADR | FanController_1_TD_SWAP_ENDIAN_FLAG | TD_AUTO_EXEC_NEXT);

                /* And get tachometer tolerances - either chain to the next fan OR wrap around to the last fan */
                if(fanNum == 0u)
                {
                    /* Next TD after fan[0] is final fan: fan[NUMBER_OF_FANS-1] */
                    (void) CyDmaTdSetConfiguration(FanController_1_fanInTds[fanNum].getToleranceTD,
                                    2u,
                                    FanController_1_fanInTds[FanController_1_NUMBER_OF_FANS - 1u].getDesiredPeriodTD,
                                    TD_INC_SRC_ADR | FanController_1_TD_SWAP_ENDIAN_FLAG |
                                    FanController_1_TachInDMA__TD_TERMOUT_EN);  /* Assert NRQ at end of all TDs */
                }
                else
                {
                    /* Next TD after fan[n] is fan[n-1] */
                    (void) CyDmaTdSetConfiguration(FanController_1_fanInTds[fanNum].getToleranceTD,
                                    2u,
                                    FanController_1_fanInTds[fanNum - 1u].getDesiredPeriodTD,
                                    TD_INC_SRC_ADR |
                                    /* Don't assert NRQ on intermediate TDs */
                                    FanController_1_TD_SWAP_ENDIAN_FLAG);
                }
            #endif /* FanController_1_IS_AUTO_HARDWARE_MODE */
        }

        /* Setup SRAM source/destination addresses and datapath register addresses */
        for (counti = 0u; counti < FanController_1_NUMBER_OF_FANS; counti++)
        {
            /* TD for actual period to RAM */
            #if (CY_PSOC3)

                (void) CyDmaTdSetAddress(FanController_1_fanOutTds[counti].setActualPeriodTD,
                                  LO16(FanController_1_TACH_ACTUAL_PERIOD_PTR),
                                  LO16(((uint32) &FanController_1_fanControl.actualPeriodDma2[counti])));

                (void) CyDmaTdSetAddress(
                                FanController_1_fanOutTds[FanController_1_NUMBER_OF_FANS + counti].setActualPeriodTD,
                                LO16(FanController_1_TACH_ACTUAL_PERIOD_PTR),
                                LO16(((uint32) &FanController_1_fanControl.actualPeriodDma1[counti])));

            #else
                (void) CyDmaTdSetAddress(FanController_1_fanOutTds[counti].setActualPeriodTD,
                                  LO16(FanController_1_TACH_ACTUAL_PERIOD_PTR),
                                  LO16(((uint32) &FanController_1_fanControl.actualPeriod[counti])));
            #endif /* (CY_PSOC3) */

            #if(FanController_1_IS_AUTO_HARDWARE_MODE)

                /* TD for desired period to tachometer */
                (void) CyDmaTdSetAddress(FanController_1_fanInTds[counti].getDesiredPeriodTD,
                                LO16(((uint32) &FanController_1_fanControl.desiredPeriod[counti])),
                                LO16(FanController_1_TACH_DESIRED_PERIOD_PTR));

                /* TD for tolerance to tachometer */
                (void) CyDmaTdSetAddress(FanController_1_fanInTds[counti].getToleranceTD,
                                LO16(((uint32) &FanController_1_fanControl.tolerance[counti])),
                                LO16(FanController_1_TACH_TOLERANCE_PTR));

            #endif /* FanController_1_IS_AUTO_HARDWARE_MODE */
        }

        /* Set the initial transaction descriptor to kick things off */
        (void) CyDmaChSetInitialTd(FanController_1_TachOutDMA_channel,
                FanController_1_fanOutTds[0u].setActualPeriodTD);
                
        #if(FanController_1_IS_AUTO_HARDWARE_MODE)

            (void) CyDmaChSetInitialTd(FanController_1_TachInDMA_channel,
                    FanController_1_fanInTds[0u].getDesiredPeriodTD);

        #endif /* FanController_1_IS_AUTO_HARDWARE_MODE */
    }


    /*********************************************************************
    * Function Name: FanController_1_TachOutDMA_DmaInitalize
    **********************************************************************
    *
    * Summary:
    *  This is a private API not exposed to users.
    *
    *  Allocates and initializes a channel of the DMAC to be used by the
    *  caller.
    *
    * Parameters:
    *  uint8  burstCount
    *  uint8  requestPerBurst
    *  uint16 upperSrcAddress
    *  uint16 upperDestAddress
    *
    * Return:
    *  The channel that can be used by the caller for DMA activity.
    *  DMA_INVALID_CHANNEL (0xFF) if there are no channels left.
    *
    * Side Effects:
    *  None
    *
    *****************************************************************/
    static uint8 FanController_1_TachOutDMA_DmaInitialize(uint8 burstCount, uint8 requestPerBurst,
                                                        uint16 upperSrcAddress, uint16 upperDestAddress) CYREENTRANT
    {
        /* Allocate a DMA channel */
        uint8 tachOutDMA_dmaHandle = FanController_1_TachOutDMA__DRQ_NUMBER;

        if(tachOutDMA_dmaHandle != DMA_INVALID_CHANNEL)
        {
            /* Configure the channel */
            (void) CyDmaChSetConfiguration(tachOutDMA_dmaHandle,
                                    burstCount,
                                    requestPerBurst,
                                    FanController_1_TachOutDMA__TERMOUT0_SEL,
                                    FanController_1_TachOutDMA__TERMOUT1_SEL,
                                    FanController_1_TachOutDMA__TERMIN_SEL);

            /* Set the extended address for the transfers */
            (void) CyDmaChSetExtendedAddress(tachOutDMA_dmaHandle, upperSrcAddress, upperDestAddress);

            /* Set the priority for this channel */
            (void) CyDmaChPriority(tachOutDMA_dmaHandle, FanController_1_TachOutDMA__PRIORITY);
        }

        return(tachOutDMA_dmaHandle);
    }


    /*********************************************************************
    * Function Name: void FanController_1_TachOutDMA_DmaRelease
    **********************************************************************
    *
    * Summary:
    *   Frees the channel associated with FanController_1_TachOutDMA and
    *   also frees the TD descriptors.
    *
    * Parameters:
    *   None
    *
    * Return:
    *   None
    *
    * Side Effects:
    *  None
    *
    *****************************************************************/
    static void FanController_1_TachOutDMA_DmaRelease(void) CYREENTRANT
    {
        /* Disable the channel, even if someone just did! */
        (void) CyDmaChDisable(FanController_1_TachOutDMA_channel);
    }


    #if(FanController_1_IS_AUTO_HARDWARE_MODE)
        
        /*********************************************************************
        * Function Name: uint8 FanController_1_TachInDMA_DmaInitalize
        **********************************************************************
        *
        * Summary:
        *   Allocates and initializes a channel of the DMAC to be used by the
        *   caller.
        *
        * Parameters:
        *  uint8  burstCount
        *  uint8  requestPerBurst
        *  uint16 upperSrcAddress
        *  uint16 upperDestAddress
        *
        * Return:
        *  The channel that can be used by the caller for DMA activity.
        *  DMA_INVALID_CHANNEL (0xFF) if there are no channels left.
        *
        * Side Effects:
        *  None
        *
        *******************************************************************/
        static uint8 FanController_1_TachInDMA_DmaInitialize(uint8 burstCount, uint8 requestPerBurst,
                                        uint16 upperSrcAddress,   uint16 upperDestAddress) CYREENTRANT
        {
            /* Allocate a DMA channel */
            uint8 tachInDMA_dmaHandle = FanController_1_TachInDMA__DRQ_NUMBER;

            if(tachInDMA_dmaHandle != DMA_INVALID_CHANNEL)
            {
                /* Configure the channel */
                (void) CyDmaChSetConfiguration(tachInDMA_dmaHandle,
                                        burstCount,
                                        requestPerBurst,
                                        FanController_1_TachInDMA__TERMOUT0_SEL,
                                        FanController_1_TachInDMA__TERMOUT1_SEL,
                                        FanController_1_TachInDMA__TERMIN_SEL);

                /* Set the extended address for the transfers */
                (void) CyDmaChSetExtendedAddress(tachInDMA_dmaHandle, upperSrcAddress, upperDestAddress);

                /* Set the priority for this channel */
                (void) CyDmaChPriority(tachInDMA_dmaHandle, FanController_1_TachInDMA__PRIORITY);
            }

            return(tachInDMA_dmaHandle);
        }

        /*********************************************************************
        * Function Name: void FanController_1_TachInDMA_DmaRelease
        **********************************************************************
        *
        * Summary:
        *   Frees the channel associated with FanController_1_TachInDMA and
        *   also frees the TD descriptors
        *
        * Parameters:
        *   None
        *
        * Return:
        *   None
        *
        * Side Effects:
        *  None
        *
        *******************************************************************/
        static void FanController_1_TachInDMA_DmaRelease(void) CYREENTRANT
        {
            /* Disable the channel, even if someone just did! */
            (void) CyDmaChDisable(FanController_1_TachInDMA_channel);
        }

    #endif /* FanController_1_IS_AUTO_HARDWARE_MODE */

#endif /* (!CY_PSOC4) */

/*****************************************************************************
* PWM Data Structure Initialization
*****************************************************************************/
#if (!CY_PSOC4)
    #if(FanController_1_IS_AUTO_HARDWARE_MODE)

        /***********************************************************************
        * Hardware Controlled PWM Control Registers (8-bit)
        ***********************************************************************/
        #if(FanController_1_PWMRES == FanController_1_PWMRES_EIGHTBIT)

            const FanController_1_fanDriverRegsStruct CYCODE
                FanController_1_fanDriverRegs[FanController_1_NUMBER_OF_FANS] =
            {
                #if (FanController_1_NUMBER_OF_FANS >= 1u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_Fan_1_PWM8_ClosedLoopFan8_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_Fan_1_PWM8_ClosedLoopFan8_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_Fan_1_PWM8_ClosedLoopFan8_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_Fan_1_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 1u */

                #if(FanController_1_NUMBER_OF_FANS >= 2u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN2_Fan_2_PWM8_ClosedLoopFan8_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN2_Fan_2_PWM8_ClosedLoopFan8_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN2_Fan_2_PWM8_ClosedLoopFan8_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN2_Fan_2_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 2u */

                #if(FanController_1_NUMBER_OF_FANS >= 3u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN3_Fan_3_PWM8_ClosedLoopFan8_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN3_Fan_3_PWM8_ClosedLoopFan8_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN3_Fan_3_PWM8_ClosedLoopFan8_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN3_Fan_3_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 3u */

                #if(FanController_1_NUMBER_OF_FANS >= 4u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN4_Fan_4_PWM8_ClosedLoopFan8_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN4_Fan_4_PWM8_ClosedLoopFan8_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN4_Fan_4_PWM8_ClosedLoopFan8_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN4_Fan_4_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 4u */

                #if(FanController_1_NUMBER_OF_FANS >= 5u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN5_Fan_5_PWM8_ClosedLoopFan8_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN5_Fan_5_PWM8_ClosedLoopFan8_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN5_Fan_5_PWM8_ClosedLoopFan8_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN5_Fan_5_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 5u */

                #if(FanController_1_NUMBER_OF_FANS >= 6u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN6_Fan_6_PWM8_ClosedLoopFan8_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN6_Fan_6_PWM8_ClosedLoopFan8_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN6_Fan_6_PWM8_ClosedLoopFan8_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN6_Fan_6_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif  /* FanController_1_NUMBER_OF_FANS >= 6u */

                #if(FanController_1_NUMBER_OF_FANS >= 7u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN7_Fan_7_PWM8_ClosedLoopFan8_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN7_Fan_7_PWM8_ClosedLoopFan8_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN7_Fan_7_PWM8_ClosedLoopFan8_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN7_Fan_7_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 7u */

                #if(FanController_1_NUMBER_OF_FANS >= 8u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN8_Fan_8_PWM8_ClosedLoopFan8_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN8_Fan_8_PWM8_ClosedLoopFan8_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN8_Fan_8_PWM8_ClosedLoopFan8_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN8_Fan_8_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 8u */

                #if(FanController_1_NUMBER_OF_FANS >= 9u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN9_Fan_9_PWM8_ClosedLoopFan8_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN9_Fan_9_PWM8_ClosedLoopFan8_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN9_Fan_9_PWM8_ClosedLoopFan8_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN9_Fan_9_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 9u */

                #if(FanController_1_NUMBER_OF_FANS >= 10u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN10_Fan_10_PWM8_ClosedLoopFan8_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN10_Fan_10_PWM8_ClosedLoopFan8_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN10_Fan_10_PWM8_ClosedLoopFan8_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN10_Fan_10_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 10u */

                #if(FanController_1_NUMBER_OF_FANS >= 11u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN11_Fan_11_PWM8_ClosedLoopFan8_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN11_Fan_11_PWM8_ClosedLoopFan8_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN11_Fan_11_PWM8_ClosedLoopFan8_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN11_Fan_11_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 11u */

                #if(FanController_1_NUMBER_OF_FANS >= 12u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN12_Fan_12_PWM8_ClosedLoopFan8_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN12_Fan_12_PWM8_ClosedLoopFan8_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN12_Fan_12_PWM8_ClosedLoopFan8_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN12_Fan_12_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 12u */

                #if(FanController_1_NUMBER_OF_FANS >= 13u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN13_Fan_13_PWM8_ClosedLoopFan8_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN13_Fan_13_PWM8_ClosedLoopFan8_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN13_Fan_13_PWM8_ClosedLoopFan8_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN13_Fan_13_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 13u */

                #if(FanController_1_NUMBER_OF_FANS >= 14u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN14_Fan_14_PWM8_ClosedLoopFan8_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN14_Fan_14_PWM8_ClosedLoopFan8_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN14_Fan_14_PWM8_ClosedLoopFan8_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN14_Fan_14_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 14u */

                #if(FanController_1_NUMBER_OF_FANS >= 15u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN15_Fan_15_PWM8_ClosedLoopFan8_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN15_Fan_15_PWM8_ClosedLoopFan8_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN15_Fan_15_PWM8_ClosedLoopFan8_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN15_Fan_15_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 15u */

                #if(FanController_1_NUMBER_OF_FANS >= 16u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN16_Fan_16_PWM8_ClosedLoopFan8_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN16_Fan_16_PWM8_ClosedLoopFan8_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN16_Fan_16_PWM8_ClosedLoopFan8_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN16_Fan_16_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    }
                #endif /* FanController_1_NUMBER_OF_FANS >= 16u */
            };

        /***********************************************************************
        * Hardware Controlled PWM Control Registers (10-bit)
        ***********************************************************************/
        #else

            const FanController_1_fanDriverRegsStruct CYCODE 
                FanController_1_fanDriverRegs[FanController_1_NUMBER_OF_FANS] =
            {
                #if(FanController_1_NUMBER_OF_FANS >= 1u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_Fan_1_PWM10_ClosedLoopFan10_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_Fan_1_PWM10_ClosedLoopFan10_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_Fan_1_PWM10_ClosedLoopFan10_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_Fan_1_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 1u */

                #if(FanController_1_NUMBER_OF_FANS >= 2u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN2_Fan_2_PWM10_ClosedLoopFan10_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN2_Fan_2_PWM10_ClosedLoopFan10_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN2_Fan_2_PWM10_ClosedLoopFan10_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN2_Fan_2_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 2u */

                #if(FanController_1_NUMBER_OF_FANS >= 3u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN3_Fan_3_PWM10_ClosedLoopFan10_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN3_Fan_3_PWM10_ClosedLoopFan10_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN3_Fan_3_PWM10_ClosedLoopFan10_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN3_Fan_3_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 3u */

                #if(FanController_1_NUMBER_OF_FANS >= 4u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN4_Fan_4_PWM10_ClosedLoopFan10_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN4_Fan_4_PWM10_ClosedLoopFan10_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN4_Fan_4_PWM10_ClosedLoopFan10_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN4_Fan_4_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 4u */

                #if(FanController_1_NUMBER_OF_FANS >= 5u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN5_Fan_5_PWM10_ClosedLoopFan10_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN5_Fan_5_PWM10_ClosedLoopFan10_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN5_Fan_5_PWM10_ClosedLoopFan10_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN5_Fan_5_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 5u */

                #if(FanController_1_NUMBER_OF_FANS >= 6u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN6_Fan_6_PWM10_ClosedLoopFan10_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN6_Fan_6_PWM10_ClosedLoopFan10_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN6_Fan_6_PWM10_ClosedLoopFan10_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN6_Fan_6_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 6u */

                #if(FanController_1_NUMBER_OF_FANS >= 7u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN7_Fan_7_PWM10_ClosedLoopFan10_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN7_Fan_7_PWM10_ClosedLoopFan10_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN7_Fan_7_PWM10_ClosedLoopFan10_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN7_Fan_7_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 7u */

                #if(FanController_1_NUMBER_OF_FANS >= 8u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN8_Fan_8_PWM10_ClosedLoopFan10_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN8_Fan_8_PWM10_ClosedLoopFan10_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN8_Fan_8_PWM10_ClosedLoopFan10_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN8_Fan_8_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 8u */

                #if(FanController_1_NUMBER_OF_FANS >= 9u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN9_Fan_9_PWM10_ClosedLoopFan10_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN9_Fan_9_PWM10_ClosedLoopFan10_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN9_Fan_9_PWM10_ClosedLoopFan10_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN9_Fan_9_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 9u */

                #if(FanController_1_NUMBER_OF_FANS >= 10u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN10_Fan_10_PWM10_ClosedLoopFan10_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN10_Fan_10_PWM10_ClosedLoopFan10_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN10_Fan_10_PWM10_ClosedLoopFan10_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN10_Fan_10_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 10u */

                #if(FanController_1_NUMBER_OF_FANS >= 11u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN11_Fan_11_PWM10_ClosedLoopFan10_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN11_Fan_11_PWM10_ClosedLoopFan10_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN11_Fan_11_PWM10_ClosedLoopFan10_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN11_Fan_11_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 11u */

                #if(FanController_1_NUMBER_OF_FANS >= 12u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN12_Fan_12_PWM10_ClosedLoopFan10_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN12_Fan_12_PWM10_ClosedLoopFan10_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN12_Fan_12_PWM10_ClosedLoopFan10_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN12_Fan_12_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 12u */

                #if(FanController_1_NUMBER_OF_FANS >= 13u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN13_Fan_13_PWM10_ClosedLoopFan10_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN13_Fan_13_PWM10_ClosedLoopFan10_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN13_Fan_13_PWM10_ClosedLoopFan10_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN13_Fan_13_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 13u */

                #if(FanController_1_NUMBER_OF_FANS >= 14u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN14_Fan_14_PWM10_ClosedLoopFan10_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN14_Fan_14_PWM10_ClosedLoopFan10_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN14_Fan_14_PWM10_ClosedLoopFan10_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN14_Fan_14_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 14u */

                #if(FanController_1_NUMBER_OF_FANS >= 15u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN15_Fan_15_PWM10_ClosedLoopFan10_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN15_Fan_15_PWM10_ClosedLoopFan10_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN15_Fan_15_PWM10_ClosedLoopFan10_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN15_Fan_15_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FANS >= 15u */

                #if(FanController_1_NUMBER_OF_FANS >= 16u)
                    {
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN16_Fan_16_PWM10_ClosedLoopFan10_u0__D1_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN16_Fan_16_PWM10_ClosedLoopFan10_u0__D0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN16_Fan_16_PWM10_ClosedLoopFan10_u0__A0_REG,
                        FanController_1_B_FanCtrl_CLOSED_LOOP_FAN16_Fan_16_SpeedErrorCounter__CONTROL_AUX_CTL_REG
                    }
                #endif /* FanController_1_NUMBER_OF_FANS >= 1u */
            };

        #endif /* Hardware Controlled Fan Mode */

    #else

        /***********************************************************************
        * Firmware Controlled PWM Control Registers (8-bit)
        ***********************************************************************/
        #if(FanController_1_PWMRES == FanController_1_PWMRES_EIGHTBIT)

            const FanController_1_fanDriverRegsStruct CYCODE
            FanController_1_fanDriverRegs[FanController_1_NUMBER_OF_FAN_OUTPUTS] =
            {
                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 1u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FanPWM_1_2_PWM8_OpenLoopFan8_u0__F0_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 1u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 2u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FanPWM_1_2_PWM8_OpenLoopFan8_u0__F1_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 2u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 3u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN34_FanPWM_3_4_PWM8_OpenLoopFan8_u0__F0_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 3u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 4u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN34_FanPWM_3_4_PWM8_OpenLoopFan8_u0__F1_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 4u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 5u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN56_FanPWM_5_6_PWM8_OpenLoopFan8_u0__F0_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 5u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 6u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN56_FanPWM_5_6_PWM8_OpenLoopFan8_u0__F1_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 6u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 7u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN78_FanPWM_7_8_PWM8_OpenLoopFan8_u0__F0_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 7u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 8u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN78_FanPWM_7_8_PWM8_OpenLoopFan8_u0__F1_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 8u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 9u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN910_FanPWM_9_10_PWM8_OpenLoopFan8_u0__F0_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 9u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 10u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN910_FanPWM_9_10_PWM8_OpenLoopFan8_u0__F1_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 10u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 11u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1112_FanPWM_11_12_PWM8_OpenLoopFan8_u0__F0_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 11u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 12u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1112_FanPWM_11_12_PWM8_OpenLoopFan8_u0__F1_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 12u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 13u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1314_FanPWM_13_14_PWM8_OpenLoopFan8_u0__F0_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 13u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 14u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1314_FanPWM_13_14_PWM8_OpenLoopFan8_u0__F1_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 14u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 15u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1516_FanPWM_15_16_PWM8_OpenLoopFan8_u0__F0_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 15u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 16u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1516_FanPWM_15_16_PWM8_OpenLoopFan8_u0__F1_REG
                    }
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 16u */
            };

            const FanController_1_fanPwmInitRegsStruct CYCODE
            FanController_1_fanPwmInitRegs[FanController_1_FANPWM_COUNT] =
            {
                #if FanController_1_NUMBER_OF_FAN_OUTPUTS >= 1u
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FanPWM_1_2_PWM8_OpenLoopFan8_u0__A1_REG,
                        FanController_1_B_FanCtrl_OPEN_LOOP_FanPWM_1_2_PWM8_OpenLoopFan8_u0__DP_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 1u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 3u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN34_FanPWM_3_4_PWM8_OpenLoopFan8_u0__A1_REG,
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN34_FanPWM_3_4_PWM8_OpenLoopFan8_u0__DP_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 3u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 5u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN56_FanPWM_5_6_PWM8_OpenLoopFan8_u0__A1_REG,
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN56_FanPWM_5_6_PWM8_OpenLoopFan8_u0__DP_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 5u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 7u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN78_FanPWM_7_8_PWM8_OpenLoopFan8_u0__A1_REG,
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN78_FanPWM_7_8_PWM8_OpenLoopFan8_u0__DP_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 7 */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 9u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN910_FanPWM_9_10_PWM8_OpenLoopFan8_u0__A1_REG,
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN910_FanPWM_9_10_PWM8_OpenLoopFan8_u0__DP_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 9u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 11u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1112_FanPWM_11_12_PWM8_OpenLoopFan8_u0__A1_REG,
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1112_FanPWM_11_12_PWM8_OpenLoopFan8_u0__DP_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 11u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 13u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1314_FanPWM_13_14_PWM8_OpenLoopFan8_u0__A1_REG,
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1314_FanPWM_13_14_PWM8_OpenLoopFan8_u0__DP_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 13 */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 15u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1516_FanPWM_15_16_PWM8_OpenLoopFan8_u0__A1_REG,
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1516_FanPWM_15_16_PWM8_OpenLoopFan8_u0__DP_AUX_CTL_REG
                    }
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 15u */
            };

        /***********************************************************************
        * Firmware Controlled PWM Control Registers (10-bit)
        ***********************************************************************/
        #else

            const FanController_1_fanDriverRegsStruct CYCODE
                FanController_1_fanDriverRegs[FanController_1_NUMBER_OF_FAN_OUTPUTS] =
            {
                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 1u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FanPWM_1_2_PWM10_OpenLoopFan10_u0__F0_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 1u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 2u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FanPWM_1_2_PWM10_OpenLoopFan10_u0__F1_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 2 */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 3u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN34_FanPWM_3_4_PWM10_OpenLoopFan10_u0__F0_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 3u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 4u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN34_FanPWM_3_4_PWM10_OpenLoopFan10_u0__F1_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 4u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 5u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN56_FanPWM_5_6_PWM10_OpenLoopFan10_u0__F0_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 5u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 6u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN56_FanPWM_5_6_PWM10_OpenLoopFan10_u0__F1_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 6u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 7u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN78_FanPWM_7_8_PWM10_OpenLoopFan10_u0__F0_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 7u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 8u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN78_FanPWM_7_8_PWM10_OpenLoopFan10_u0__F1_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 8u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 9u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN910_FanPWM_9_10_PWM10_OpenLoopFan10_u0__F0_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 9u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 10u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN910_FanPWM_9_10_PWM10_OpenLoopFan10_u0__F1_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 10u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 11u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1112_FanPWM_11_12_PWM10_OpenLoopFan10_u0__F0_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 11u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 12u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1112_FanPWM_11_12_PWM10_OpenLoopFan10_u0__F1_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 12u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 13u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1314_FanPWM_13_14_PWM10_OpenLoopFan10_u0__F0_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 13u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 14u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1314_FanPWM_13_14_PWM10_OpenLoopFan10_u0__F1_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 14u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 15u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1516_FanPWM_15_16_PWM10_OpenLoopFan10_u0__F0_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 15u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 16u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1516_FanPWM_15_16_PWM10_OpenLoopFan10_u0__F1_REG
                    }
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 16u */
            };

            const FanController_1_fanPwmInitRegsStruct CYCODE
                FanController_1_fanPwmInitRegs[FanController_1_FANPWM_COUNT] =
            {
                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 1u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FanPWM_1_2_PWM10_OpenLoopFan10_u0__A1_REG,
                        FanController_1_B_FanCtrl_OPEN_LOOP_FanPWM_1_2_PWM10_OpenLoopFan10_u0__DP_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 1u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 3u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN34_FanPWM_3_4_PWM10_OpenLoopFan10_u0__A1_REG,
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN34_FanPWM_3_4_PWM10_OpenLoopFan10_u0__DP_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 3u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 5u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN56_FanPWM_5_6_PWM10_OpenLoopFan10_u0__A1_REG,
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN56_FanPWM_5_6_PWM10_OpenLoopFan10_u0__DP_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 5u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 7u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN78_FanPWM_7_8_PWM10_OpenLoopFan10_u0__A1_REG,
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN78_FanPWM_7_8_PWM10_OpenLoopFan10_u0__DP_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 7u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 9u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN910_FanPWM_9_10_PWM10_OpenLoopFan10_u0__A1_REG,
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN910_FanPWM_9_10_PWM10_OpenLoopFan10_u0__DP_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 9u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 11u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1112_FanPWM_11_12_PWM10_OpenLoopFan10_u0__A1_REG,
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1112_FanPWM_11_12_PWM10_OpenLoopFan10_u0__DP_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 11u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 13u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1314_FanPWM_13_14_PWM10_OpenLoopFan10_u0__A1_REG,
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1314_FanPWM_13_14_PWM10_OpenLoopFan10_u0__DP_AUX_CTL_REG
                    },
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 13u */

                #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 15u)
                    {
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1516_FanPWM_15_16_PWM10_OpenLoopFan10_u0__A1_REG,
                        FanController_1_B_FanCtrl_OPEN_LOOP_FAN1516_FanPWM_15_16_PWM10_OpenLoopFan10_u0__DP_AUX_CTL_REG
                    }
                #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 15u */
            };

        #endif    /* 10-bit Resolution */
    #endif /* FanController_1_IS_AUTO_HARDWARE_MODE */

#else  /* PSoC 4 */

    const FanController_1_fanDriverRegsStruct CYCODE
        FanController_1_fanDriverRegs[FanController_1_NUMBER_OF_FAN_OUTPUTS] =
    {
        #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 1u)
            {
                FanController_1_FW_PWM1_cy_m0s8_tcpwm_1__CC
            },
        #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 1u */

        #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 2u)
            {
                FanController_1_FW_PWM2_cy_m0s8_tcpwm_1__CC
            },
        #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 2u */

        #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 3u)
            {
                FanController_1_FW_PWM3_cy_m0s8_tcpwm_1__CC
            },
        #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 3u */

        #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 4u)
            {
                FanController_1_FW_PWM4_cy_m0s8_tcpwm_1__CC
            }
        #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 4u */
    };

    const  FanController_1_fanPwmInitRegsStruct CYCODE 
        FanController_1_fanPwmInitRegs[FanController_1_NUMBER_OF_FAN_OUTPUTS] =
    {
        #if FanController_1_NUMBER_OF_FAN_OUTPUTS >= 1u
            {
                FanController_1_FW_PWM1_cy_m0s8_tcpwm_1__PERIOD
            },
        #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 1u */

        #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 2u)
            {
                FanController_1_FW_PWM2_cy_m0s8_tcpwm_1__PERIOD
            },
        #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 2u */

        #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 3u)
            {
                FanController_1_FW_PWM3_cy_m0s8_tcpwm_1__PERIOD
            },
        #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 3u */

        #if(FanController_1_NUMBER_OF_FAN_OUTPUTS >= 4u)
            {
                FanController_1_FW_PWM4_cy_m0s8_tcpwm_1__PERIOD
            },
        #endif /* FanController_1_NUMBER_OF_FAN_OUTPUTS >= 4 */
    };

#endif /* (!CY_PSOC4) */

/*****************************************************************************
* Hardware Controlled Fan Mode Fan Control Data Structure Initialization
*****************************************************************************/
FanController_1_fanControlStruct FanController_1_fanControl
#if(FanController_1_IS_AUTO_HARDWARE_MODE)
= {
    {
        /* Initialize the Desired Period Field (RPM B scaled up by tolerance factor %) */
        #if(FanController_1_NUMBER_OF_FANS >= 1u)
            (uint16) (FanController_1_RPM_PERIOD_CONV_FACTOR /
            (16100u + ((FanController_1_TOLERANCE * 16100u)/ FanController_1_TOLERANCE_DIVIDER))),
        #endif /* FanController_1_NUMBER_OF_FANS >= 1u */

        #if(FanController_1_NUMBER_OF_FANS >= 2u)
            (uint16) (FanController_1_RPM_PERIOD_CONV_FACTOR /
            (10000u + ((FanController_1_TOLERANCE * 10000u)/ FanController_1_TOLERANCE_DIVIDER))),
        #endif /* FanController_1_NUMBER_OF_FANS >= 2u */

        #if(FanController_1_NUMBER_OF_FANS >= 3u)
            (uint16) (FanController_1_RPM_PERIOD_CONV_FACTOR /
            (10000u + ((FanController_1_TOLERANCE * 10000u)/ FanController_1_TOLERANCE_DIVIDER))),
        #endif /* FanController_1_NUMBER_OF_FANS >= 3u */

        #if(FanController_1_NUMBER_OF_FANS >= 4u)
            (uint16) (FanController_1_RPM_PERIOD_CONV_FACTOR /
            (10000u + ((FanController_1_TOLERANCE * 10000u)/ FanController_1_TOLERANCE_DIVIDER))),
        #endif /* FanController_1_NUMBER_OF_FANS >= 4 */

        #if(FanController_1_NUMBER_OF_FANS >= 5)
            (uint16) (FanController_1_RPM_PERIOD_CONV_FACTOR /
            (10000u + ((FanController_1_TOLERANCE * 10000u)/ FanController_1_TOLERANCE_DIVIDER))),
        #endif /* FanController_1_NUMBER_OF_FANS >= 5u */

        #if(FanController_1_NUMBER_OF_FANS >= 6u)
            (uint16) (FanController_1_RPM_PERIOD_CONV_FACTOR /
            (10000u + ((FanController_1_TOLERANCE * 10000u)/ FanController_1_TOLERANCE_DIVIDER))),
        #endif /* FanController_1_NUMBER_OF_FANS >= 6u */

        #if(FanController_1_NUMBER_OF_FANS >= 7u)
            (uint16) (FanController_1_RPM_PERIOD_CONV_FACTOR /
            (10000u + ((FanController_1_TOLERANCE * 10000u)/ FanController_1_TOLERANCE_DIVIDER))),
        #endif /* FanController_1_NUMBER_OF_FANS >= 7u */

        #if(FanController_1_NUMBER_OF_FANS >= 8u)
            (uint16) (FanController_1_RPM_PERIOD_CONV_FACTOR /
            (10000u + ((FanController_1_TOLERANCE * 10000u)/ FanController_1_TOLERANCE_DIVIDER))),
        #endif /* FanController_1_NUMBER_OF_FANS >= 8u */

        #if(FanController_1_NUMBER_OF_FANS >= 9u)
            (uint16) (FanController_1_RPM_PERIOD_CONV_FACTOR /
            (10000u + ((FanController_1_TOLERANCE * 10000u)/ FanController_1_TOLERANCE_DIVIDER))),
        #endif /* FanController_1_NUMBER_OF_FANS >= 9u */

        #if(FanController_1_NUMBER_OF_FANS >= 10u)
            (uint16) (FanController_1_RPM_PERIOD_CONV_FACTOR /
            (10000u + ((FanController_1_TOLERANCE * 10000u)/ FanController_1_TOLERANCE_DIVIDER))),
        #endif /* FanController_1_NUMBER_OF_FANS >= 10u */

        #if(FanController_1_NUMBER_OF_FANS >= 11u)
            (uint16) (FanController_1_RPM_PERIOD_CONV_FACTOR /
            (10000u + ((FanController_1_TOLERANCE * 10000u)/ FanController_1_TOLERANCE_DIVIDER))),
        #endif /* FanController_1_NUMBER_OF_FANS >= 11u */

        #if(FanController_1_NUMBER_OF_FANS >= 12u)
            (uint16) (FanController_1_RPM_PERIOD_CONV_FACTOR /
            (10000u + ((FanController_1_TOLERANCE * 10000u)/ FanController_1_TOLERANCE_DIVIDER))),
        #endif /* FanController_1_NUMBER_OF_FANS >= 12u */

        #if(FanController_1_NUMBER_OF_FANS >= 13u)
            (uint16) (FanController_1_RPM_PERIOD_CONV_FACTOR /
            (10000u + ((FanController_1_TOLERANCE * 10000u)/ FanController_1_TOLERANCE_DIVIDER))),
        #endif /* FanController_1_NUMBER_OF_FANS >= 13u */

        #if(FanController_1_NUMBER_OF_FANS >= 14u)
            (uint16) (FanController_1_RPM_PERIOD_CONV_FACTOR /
            (10000u + ((FanController_1_TOLERANCE * 10000u)/ FanController_1_TOLERANCE_DIVIDER))),
        #endif /* FanController_1_NUMBER_OF_FANS >= 14u */

        #if(FanController_1_NUMBER_OF_FANS >= 15u)
            (uint16) (FanController_1_RPM_PERIOD_CONV_FACTOR /
            (10000u + ((FanController_1_TOLERANCE * 10000u)/ FanController_1_TOLERANCE_DIVIDER))),
        #endif /* FanController_1_NUMBER_OF_FANS >= 15 */

        #if(FanController_1_NUMBER_OF_FANS >= 16u)
            (uint16) (FanController_1_RPM_PERIOD_CONV_FACTOR /
            (10000u + ((FanController_1_TOLERANCE * 10000u)/ FanController_1_TOLERANCE_DIVIDER))),
        #endif /* FanController_1_NUMBER_OF_FANS >= 16u */
    },
    {
        /* Initialize the Tolerance Field (% of RPM B) */
        #if(FanController_1_NUMBER_OF_FANS >= 1u)
            (uint16) (((FanController_1_RPM_PERIOD_CONV_FACTOR * FanController_1_TOLERANCE) /
              FanController_1_TOLERANCE_DIVIDER) / (16100u)),
        #endif /* FanController_1_NUMBER_OF_FANS >= 1u */

        #if(FanController_1_NUMBER_OF_FANS >= 2u)
            (uint16) (((FanController_1_RPM_PERIOD_CONV_FACTOR * FanController_1_TOLERANCE) /
              FanController_1_TOLERANCE_DIVIDER) / (10000u)),
        #endif /* FanController_1_NUMBER_OF_FANS >= 2u */

        #if(FanController_1_NUMBER_OF_FANS >= 3u)
            (uint16) (((FanController_1_RPM_PERIOD_CONV_FACTOR * FanController_1_TOLERANCE) /
              FanController_1_TOLERANCE_DIVIDER) / (10000u)),
        #endif /* FanController_1_NUMBER_OF_FANS >= 3u */

        #if(FanController_1_NUMBER_OF_FANS >= 4u)
            (uint16) (((FanController_1_RPM_PERIOD_CONV_FACTOR * FanController_1_TOLERANCE) /
              FanController_1_TOLERANCE_DIVIDER) / (10000u)),
        #endif /* FanController_1_NUMBER_OF_FANS >= 4u */

        #if(FanController_1_NUMBER_OF_FANS >= 5u)
            (uint16) (((FanController_1_RPM_PERIOD_CONV_FACTOR * FanController_1_TOLERANCE) /
              FanController_1_TOLERANCE_DIVIDER) / (10000u)),
        #endif /* FanController_1_NUMBER_OF_FANS >= 5u */

        #if(FanController_1_NUMBER_OF_FANS >= 6u)
            (uint16) (((FanController_1_RPM_PERIOD_CONV_FACTOR * FanController_1_TOLERANCE) /
              FanController_1_TOLERANCE_DIVIDER) / (10000u)),
        #endif /* FanController_1_NUMBER_OF_FANS >= 6u */

        #if(FanController_1_NUMBER_OF_FANS >= 7u)
            (uint16) (((FanController_1_RPM_PERIOD_CONV_FACTOR * FanController_1_TOLERANCE) /
              FanController_1_TOLERANCE_DIVIDER) / (10000u)),
        #endif /* FanController_1_NUMBER_OF_FANS >= 7u */

        #if(FanController_1_NUMBER_OF_FANS >= 8u)
            (uint16) (((FanController_1_RPM_PERIOD_CONV_FACTOR * FanController_1_TOLERANCE) /
              FanController_1_TOLERANCE_DIVIDER) / (10000u)),
        #endif /* FanController_1_NUMBER_OF_FANS >= 8u */

        #if(FanController_1_NUMBER_OF_FANS >= 9u)
            (uint16) (((FanController_1_RPM_PERIOD_CONV_FACTOR * FanController_1_TOLERANCE) /
              FanController_1_TOLERANCE_DIVIDER) / (10000u)),
        #endif /* FanController_1_NUMBER_OF_FANS >= 9u */

        #if(FanController_1_NUMBER_OF_FANS >= 10)
            (uint16) (((FanController_1_RPM_PERIOD_CONV_FACTOR * FanController_1_TOLERANCE) /
              FanController_1_TOLERANCE_DIVIDER) / (10000u)),
        #endif /* FanController_1_NUMBER_OF_FANS >= 10u */

        #if(FanController_1_NUMBER_OF_FANS >= 11u)
            (uint16) (((FanController_1_RPM_PERIOD_CONV_FACTOR * FanController_1_TOLERANCE) /
              FanController_1_TOLERANCE_DIVIDER) / (10000u)),
        #endif /* FanController_1_NUMBER_OF_FANS >= 11u */

        #if(FanController_1_NUMBER_OF_FANS >= 12u)
            (uint16) (((FanController_1_RPM_PERIOD_CONV_FACTOR * FanController_1_TOLERANCE) /
              FanController_1_TOLERANCE_DIVIDER) / (10000u)),
        #endif /* FanController_1_NUMBER_OF_FANS >= 12 */

        #if(FanController_1_NUMBER_OF_FANS >= 13u)
            (uint16) (((FanController_1_RPM_PERIOD_CONV_FACTOR * FanController_1_TOLERANCE) /
              FanController_1_TOLERANCE_DIVIDER) / (10000u)),
        #endif /* FanController_1_NUMBER_OF_FANS >= 13u */

        #if(FanController_1_NUMBER_OF_FANS >= 14u)
            (uint16) (((FanController_1_RPM_PERIOD_CONV_FACTOR * FanController_1_TOLERANCE) /
              FanController_1_TOLERANCE_DIVIDER) / (10000u)),
        #endif /* FanController_1_NUMBER_OF_FANS >= 14u */

        #if(FanController_1_NUMBER_OF_FANS >= 15u)
            (uint16) (((FanController_1_RPM_PERIOD_CONV_FACTOR * FanController_1_TOLERANCE) /
              FanController_1_TOLERANCE_DIVIDER) / (10000u)),
        #endif /* FanController_1_NUMBER_OF_FANS >= 15u */

        #if(FanController_1_NUMBER_OF_FANS >= 16u)
            (uint16) (((FanController_1_RPM_PERIOD_CONV_FACTOR * FanController_1_TOLERANCE) /
              FanController_1_TOLERANCE_DIVIDER) / (10000u))
        #endif /* FanController_1_NUMBER_OF_FANS >= 16u */
    }
}
#endif /* FanController_1_IS_AUTO_HARDWARE_MODE */
;

/*****************************************************************************
* Fan Electromechanical Properties Data Structure Initialization
*****************************************************************************/
 /* Properties come from customizer GUI */
FanController_1_fanPropertiesStruct FanController_1_fanProperties[FanController_1_NUMBER_OF_FANS] =
{
    #if(FanController_1_NUMBER_OF_FANS >= 1u)
        {
            4200u,
            16100u,
            (20u * 100u),
            (100u * 100u),
            (((100u - 20u) * 100u) / ((16100u - 4200u) / 100u)),
            4200u,
            (20u * 100u)
        },
    #endif /* FanController_1_NUMBER_OF_FANS >= 1u */

    #if(FanController_1_NUMBER_OF_FANS >= 2u)
        {
            1000u,
            10000u,
            (25u * 100u),
            (100u * 100u),
            (((100u - 25u) * 100u) / ((10000u - 1000u) / 100u)),
            5000u,
            (58u * 100u)
        },
    #endif /* FanController_1_NUMBER_OF_FANS >= 2 */

    #if(FanController_1_NUMBER_OF_FANS >= 3u)
        {
            1000u,
            10000u,
            (25u * 100u),
            (100u * 100u),
            (((100u - 25u) * 100u) / ((10000u - 1000u) / 100u)),
            5000u,
            (58u * 100u)
        },
    #endif /* FanController_1_NUMBER_OF_FANS >= 3u */

    #if(FanController_1_NUMBER_OF_FANS >= 4u)
        {
            1000u,
            10000u,
            (25u * 100u),
            (100u * 100u),
            (((100u - 25u) * 100u) / ((10000u - 1000u) / 100u)),
            5000u,
            (58u * 100u)
        },
    #endif /* FanController_1_NUMBER_OF_FANS >= 4u */

    #if(FanController_1_NUMBER_OF_FANS >= 5u)
        {
            1000u,
            10000u,
            (25u * 100u),
            (100u * 100u),
            (((100u - 25u) * 100u) / ((10000u - 1000u) / 100u)),
            5000u,
            (58u * 100u)
        },
    #endif /* FanController_1_NUMBER_OF_FANS >= 5u */

    #if(FanController_1_NUMBER_OF_FANS >= 6u)
        {
            1000u,
            10000u,
            (25u * 100u),
            (100u * 100u),
            (((100u - 25u) * 100u) / ((10000u - 1000u) / 100u)),
            5000u,
            (58u * 100u)
        },
    #endif /* FanController_1_NUMBER_OF_FANS >= 6u */

    #if(FanController_1_NUMBER_OF_FANS >= 7u)
        {
            1000u,
            10000u,
            (25u * 100u),
            (100u * 100u),
            (((100u - 25u) * 100u) / ((10000u - 1000u) / 100u)),
            5000u,
            (58u * 100u)
        },
    #endif /* FanController_1_NUMBER_OF_FANS >= 7u */

    #if(FanController_1_NUMBER_OF_FANS >= 8u)
        {
            1000u,
            10000u,
            (25u * 100u),
            (100u * 100u),
            (((100u - 25u) * 100u) / ((10000u - 1000u) / 100u)),
            5000u,
            (58u * 100u)
        },
    #endif /* FanController_1_NUMBER_OF_FANS >= 8u */

    #if(FanController_1_NUMBER_OF_FANS >= 9u)
        {
            1000u,
            10000u,
            (25u * 100u),
            (100u * 100u),
            (((100u - 25u) * 100u) / ((10000u - 1000u) / 100u)),
            5000u,
            (58u * 100u)
        },
    #endif /* FanController_1_NUMBER_OF_FANS >= 9u */

    #if(FanController_1_NUMBER_OF_FANS >= 10u)
        {
            1000u,
            10000u,
            (25u * 100u),
            (100u * 100u),
            (((100u - 25u) * 100u) / ((10000u - 1000u) / 100u)),
            5000u,
            (58u * 100u)
        },
    #endif /* FanController_1_NUMBER_OF_FANS >= 10u */

    #if(FanController_1_NUMBER_OF_FANS >= 11u)
        {
            1000u,
            10000u,
            (25u * 100u),
            (100u * 100u),
            (((100u - 25u) * 100u) / ((10000u - 1000u) / 100u)),
            5000u,
            (58u * 100u)
        },
    #endif /* FanController_1_NUMBER_OF_FANS >= 11u */

    #if(FanController_1_NUMBER_OF_FANS >= 12u)
        {
            100u,
            10000u,
            (25u * 100u),
            (100u * 100u),
            (((100u - 25u) * 100u) / ((10000u - 100u) / 100u)),
            5000u,
            (62u * 100u)
        },
    #endif /* FanController_1_NUMBER_OF_FANS >= 12u */

    #if(FanController_1_NUMBER_OF_FANS >= 13u)
        {
            100u,
            10000u,
            (25u * 100u),
            (100u * 100u),
            (((100u - 25u) * 100u) / ((10000u - 100u) / 100u)),
            5000u,
            (62u * 100u)
        },
    #endif /* FanController_1_NUMBER_OF_FANS >= 13u */

    #if(FanController_1_NUMBER_OF_FANS >= 14u)
        {
            1000u,
            10000u,
            (25u * 100u),
            (100u * 100u),
            (((100u - 25u) * 100u) / ((10000u - 1000u) / 100u)),
            5000u,
            (58u * 100u)
        },
    #endif /* FanController_1_NUMBER_OF_FANS >= 14u */

    #if(FanController_1_NUMBER_OF_FANS >= 15u)
        {
            1000u,
            10000u,
            (25u * 100u),
            (100u * 100u),
            (((100u - 25u) * 100u) / ((10000u - 1000u) / 100u)),
            5000u,
            (58u * 100u)
        },
    #endif /* FanController_1_NUMBER_OF_FANS >= 15u */

    #if(FanController_1_NUMBER_OF_FANS >= 16u)
        {
            1000u,
            10000u,
            (25u * 100u),
            (100u * 100u),
            (((100u - 25u) * 100u) / ((10000u - 1000u) / 100u)),
             5000u,
            (58u * 100u)
        }
    #endif /* FanController_1_NUMBER_OF_FANS >= 16u */
};


/* [] END OF FILE */
