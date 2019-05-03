/*******************************************************************************
* File Name: FanController.h
* Version 4.10
*
* Description:
*  API function prototypes, customizer parameters and other constants for the
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

#if !defined(FanController_CY_FAN_CONTROLLER_H)
#define FanController_CY_FAN_CONTROLLER_H

#include "cytypes.h"
#include "cyfitter.h"
#if (!CY_PSOC4)
    #include <CYDMAC.H>
#endif /* (!CY_PSOC4) */


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define FanController_FAN_CTL_MODE               (0u)
#define FanController_NUMBER_OF_FANS             (4u)
#define FanController_NUMBER_OF_BANKS            (0u)
#define FanController_PWMRES                     (0u)

#define FanController_MANUAL_MODE                (0u)

#define FanController_MANUAL_MODE_TRUE           (1u)
#define FanController_MANUAL_MODE_FALSE          (0u)

#define FanController_FANCTLMODE_FIRMWARE        (0u)
#define FanController_FANCTLMODE_HARDWARE        (1u)

#define FanController_NUMBER_OF_FAN_OUTPUTS      (4u)

/* Resolution constants */
#define FanController_PWMRES_EIGHTBIT            (0u)
#define FanController_PWMRES_TENBIT              (1u)

/* PWM Configuration Constants */
#if(FanController_PWMRES == FanController_PWMRES_EIGHTBIT)
    #define FanController_PWM_PERIOD                 (240u)
#else
    #define FanController_PWM_PERIOD                 (960u)
#endif /*FanController_PWMRES == FanController_PWMRES_EIGHTBIT */

/* Macros to identify if Automatic or Manual Control mode is used */
#define FanController_IS_AUTO_HARDWARE_MODE      \
                (FanController_FAN_CTL_MODE == FanController_FANCTLMODE_HARDWARE)
#define FanController_IS_MANUAL_MODE             \
                ((FanController_MANUAL_MODE == FanController_MANUAL_MODE_TRUE) && \
                    ((FanController_FAN_CTL_MODE != FanController_FANCTLMODE_HARDWARE)))
#define FanController_IS_AUTO_FIRMWARE_MODE      \
                ((FanController_FAN_CTL_MODE == FanController_FANCTLMODE_FIRMWARE) && \
                    (!(FanController_IS_MANUAL_MODE)))

/* PID Constants */
#if(FanController_IS_AUTO_FIRMWARE_MODE)
    #define FanController_PID_A1             (3072)
    #define FanController_PID_A2             (-5120)
    #define FanController_PID_A3             (2252)

    #define FanController_PID_POST_GAIN      (20u)

    #define FanController_PID_OUTPUT_SAT_L   (0u)
    #define FanController_PID_OUTPUT_SAT_H   (50331648u)

    /* PID error low and high saturation. These are constants defined
    * by EROS to be (-4096) and (4096) respectively.
    */
    #define FanController_PID_ERROR_SAT_L    (-4096)
    #define FanController_PID_ERROR_SAT_H    (4096)

#endif /* (FanController_IS_AUTO_FIRMWARE_MODE) */


/***************************************
*        Function Prototypes
***************************************/

void    FanController_Start(void) ;
void    FanController_Stop(void) ;
void    FanController_Init(void) ;
void    FanController_Enable(void) ;
void    FanController_EnableAlert(void) ;
void    FanController_DisableAlert(void) ;
void    FanController_SetAlertMode(uint8 alertMode) ;
uint8   FanController_GetAlertMode(void) ;
void    FanController_SetAlertMask(uint16 alertMask) ;
uint16  FanController_GetAlertMask(void) ;
uint8   FanController_GetAlertSource(void) ;
uint16  FanController_GetFanStallStatus(void) ;
void    FanController_SetDutyCycle(uint8 fanOrBankNumber, uint16 dutyCycle)
                                                        ;
uint16  FanController_GetDutyCycle(uint8 fanOrBankNumber) ;
void    FanController_SetDesiredSpeed(uint8 fanNumber, uint16 rpm)
                                                        ;
uint16  FanController_GetActualSpeed(uint8 fanNumber) ;

#if (FanController_IS_AUTO_FIRMWARE_MODE)
    void FanController_SetPID(uint8 fanNum, uint16 kp, uint16 ki, uint16 kd)
            ;
    void FanController_SetSaturation(uint8 fanNum, uint16 satH, uint16 satL)
            ;
#endif /* (FanController_IS_AUTO_FIRMWARE_MODE) */

#if (!FanController_IS_MANUAL_MODE)
    #if (FanController_IS_AUTO_HARDWARE_MODE)
        void    FanController_OverrideAutomaticControl(uint8 override)
                                                        ;
    #endif /* (FanController_IS_AUTO_HARDWARE_MODE)*/

    uint16  FanController_GetDesiredSpeed(uint8 fanNumber) ;
    uint16  FanController_GetFanSpeedStatus(void) ;
#endif /* (!FanController_IS_MANUAL_MODE) */

/* Interrupt handlers */
#if(CY_PSOC4)
    /* ISR for transferring data from Tachometer to RAM */
    CY_ISR_PROTO(FanController_DATA_SEND_ISR);
#endif /* (CY_PSOC4) */

#if(FanController_IS_AUTO_FIRMWARE_MODE)
    /* PID Control algorithm ISR */
    CY_ISR_PROTO(FanController_PID_CONTROL_ISR);
#endif /* (FanController_IS_AUTO_FIRMWARE_MODE) */


/***************************************
*           API Constants
***************************************/

/* Constants for Acoustic Noise Reduction */
#define FanController_NOISE_REDUCTION_OFF        (0u)
#define FanController_NOISE_REDUCTION_ON         (1u)

/* Bit definitions for the Alert Source Status Register (FanController_GetAlertSource()) */
#define FanController_STALL_ALERT                (0x01u)
#define FanController_SPEED_ALERT                (0x02u)

/* End of Conversion constants */
#define FanController_EOC_HIGH                   (0x01u)
#define FanController_EOC_LOW                    (0x00u)

/* Maximum possible Fans number */
#define FanController_MAX_FANS                   (16u)

/* Threshold for speed regulation error occurrence */
#define FanController_SPEED_ERRORS_TRESHOLD      (16u)
#define FanController_MAX_SPEED_ERRORS           (255u)

#if (FanController_IS_AUTO_FIRMWARE_MODE)
    #define FanController_MAX_GAIN                   (65535u)
    #define FanController_GAIN_MULTIPLIER            (4096u)
#endif /* (FanController_IS_AUTO_HARDWARE_MODE) */

/* RPM to Duty Cycle Conversion Constants */
#define FanController_TACH_CLOCK_FREQ            (500000u)
#define FanController_RPM_PERIOD_CONV_FACTOR     (FanController_TACH_CLOCK_FREQ * 60u)

#define FanController_PWM_INIT_DUTY              (FanController_PWM_PERIOD)
#define FanController_PWM_DUTY_DIVIDER           (10000u)      /* API Duty Cycle is expressed in 100ths of % */

#if (CY_PSOC3)
    /* 8051 registers have 16-bit addresses */
    #define FanController_RegAddrType            uint16
    /* DMA TD endian swap flag - 8051 is big endian */
    #define FanController_TD_SWAP_ENDIAN_FLAG    (TD_SWAP_EN)
#else
    /* ARM registers have 32-bit addresses */
    #define FanController_RegAddrType            uint32
    /* DMA TD endian swap flag - ARM is little endian */
    #define FanController_TD_SWAP_ENDIAN_FLAG    (0u)
#endif /* CY_PSOC3 */

#define FanController_TachOutDMA__TD_TERMOUT_EN ((FanController_TachOutDMA__TERMOUT0_EN ? TD_TERMOUT0_EN : 0u) | \
    (FanController_TachOutDMA__TERMOUT1_EN ? TD_TERMOUT1_EN : 0u))

#if(FanController_IS_AUTO_HARDWARE_MODE)
    #define FanController_TachInDMA__TD_TERMOUT_EN ((FanController_TachInDMA__TERMOUT0_EN ? \
                                TD_TERMOUT0_EN : 0u) | (FanController_TachInDMA__TERMOUT1_EN ? TD_TERMOUT1_EN : 0u))
#endif /* FanController_IS_AUTO_HARDWARE_MODE */

extern uint8 FanController_tachOutDMA_DmaHandle;
#if(FanController_IS_AUTO_HARDWARE_MODE)
    extern uint8 FanController_tachInDMA_DmaHandle;
#endif /* FanController_IS_AUTO_HARDWARE_MODE */


/***************************************
*     Data Struct Definitions
***************************************/

/* Hardware and Firmware Fan Control Mode DMA transaction descriptors to store actual fan speeds in SRAM */
typedef struct
{
   uint8     setActualPeriodTD;                           /* Actual tach period from Tach block to SRAM */
} FanController_fanTdOutStruct;

/* Hardware Fan Control Mode DMA transaction descriptors to read desired speeds and tolerance from SRAM */
#if(FanController_IS_AUTO_HARDWARE_MODE)
    typedef struct
    {
        uint8     getDesiredPeriodTD;                       /* Desired tach period from SRAM to Tach block */
        uint8     getToleranceTD;                           /* Desired period tolerance from SRAM to Tach block */
    } FanController_fanTdInStruct;
#endif /* FanController_IS_AUTO_HARDWARE_MODE */

/* DMA Controller SRAM Structure */
typedef struct
{
    #if(FanController_IS_AUTO_HARDWARE_MODE)
        uint16  desiredPeriod[FanController_NUMBER_OF_FANS];
        uint16  tolerance[FanController_NUMBER_OF_FANS];
    #endif /* FanController_IS_AUTO_HARDWARE_MODE */

    #if(CY_PSOC3)
        uint16  actualPeriodDma1[FanController_NUMBER_OF_FANS];
        uint16  actualPeriodDma2[FanController_NUMBER_OF_FANS];
    #else
        uint16  actualPeriod[FanController_NUMBER_OF_FANS];
    #endif /* (CY_PSoC3) */

} FanController_fanControlStruct;

/* Fan Properties Structure. From parameters entered into the customizer */
typedef struct
{
    uint16 rpmA;
    uint16 rpmB;
    uint16 dutyA;
    uint16 dutyB;
    uint16 dutyRpmSlope;
    uint16 initRpm;
    uint16 initDuty;
} FanController_fanPropertiesStruct;

/* PWM configuration structure */
#if (!CY_PSOC4)
    #if(FanController_IS_AUTO_HARDWARE_MODE)

        /* An array that holds addresses of PWM configuration registers for Auto
        * Hardware mode.
        */
        typedef struct
        {
            FanController_RegAddrType  pwmPeriodReg;
            FanController_RegAddrType  pwmMaxDutyReg;
            FanController_RegAddrType  pwmSetDutyReg;
            FanController_RegAddrType  errorCountReg;
        } FanController_fanDriverRegsStruct;

    #else
        /* An array that holds addresses of PWM configuration registers for Auto
        * Firmware and Manual modes (PSoC 3/5LP).
        */
        typedef struct
        {
            FanController_RegAddrType  pwmSetDutyReg;
        } FanController_fanDriverRegsStruct;

        typedef struct
        {
            FanController_RegAddrType  pwmPeriodReg;
            FanController_RegAddrType  pwmAuxControlReg;
        } FanController_fanPwmInitRegsStruct;

        #define FanController_FANPWM_COUNT                   ((FanController_NUMBER_OF_FAN_OUTPUTS / 2u) + \
                    (FanController_NUMBER_OF_FAN_OUTPUTS % 2u))

        extern const  FanController_fanPwmInitRegsStruct CYCODE 
            FanController_fanPwmInitRegs[FanController_FANPWM_COUNT];

    #endif /* FanController_IS_AUTO_HARDWARE_MODE */

#else /* PSoC 4 */
    /* An array that holds addresses of PWM configuration registers for Auto
    * Firmware and Manual modes (PSoC 4).
    */
    typedef struct
    {
        FanController_RegAddrType  pwmSetDutyReg;
    } FanController_fanDriverRegsStruct;

    typedef struct
    {
        FanController_RegAddrType  pwmPeriodReg;
    } FanController_fanPwmInitRegsStruct;

    extern const FanController_fanPwmInitRegsStruct CYCODE
        FanController_fanPwmInitRegs[FanController_NUMBER_OF_FAN_OUTPUTS];

#endif /* (!CY_PSOC4) */

extern uint8 FanController_initVar;

extern FanController_fanControlStruct FanController_fanControl;

#if(!FanController_IS_MANUAL_MODE)
    /* An array to store desired fans speeds */
    extern uint16 FanController_desiredFansSpeed[FanController_NUMBER_OF_FANS];
#endif /* (!FanController_IS_MANUAL_MODE) */

/* Array which store PID parameters for each fan */
#if(FanController_IS_AUTO_FIRMWARE_MODE)
    extern int16 FanController_pidA1[FanController_NUMBER_OF_FANS];
    extern int16 FanController_pidA2[FanController_NUMBER_OF_FANS];
    extern int16 FanController_pidA3[FanController_NUMBER_OF_FANS];

    extern uint32 FanController_outputSatL[FanController_NUMBER_OF_FANS];
    extern uint32 FanController_outputSatH[FanController_NUMBER_OF_FANS];

    extern volatile uint16 FanController_speedStatus;
#endif /* (FanController_IS_AUTO_FIRMWARE_MODE) */

extern const FanController_fanDriverRegsStruct CYCODE
    FanController_fanDriverRegs[FanController_NUMBER_OF_FAN_OUTPUTS];

extern FanController_fanPropertiesStruct FanController_fanProperties[FanController_NUMBER_OF_FANS];

#if (CY_PSOC4)
    extern uint16 FanController_alertMask;
    extern uint16 FanController_stallStatus;
#endif /* (CY_PSOC4) */

/***************************************
* Initial Parameter Constants
***************************************/

#define FanController_MAX_FAN_NUM                 (FanController_NUMBER_OF_FANS - 1u)
#define FanController_DAMPING_FACTOR              (2u)
#define FanController_INIT_ALERT_ENABLE           (0u)
#define FanController_NOISE_REDUCTION_MODE        (0u)
#define FanController_INIT_ALERT_MASK             ((uint16) (((uint32) 0x01u << 4u) - 1u))
#define FanController_DAMPING_FACTOR_PERIOD_LOW   (250u)
#define FanController_DAMPING_FACTOR_PERIOD_HIGH  (FanController_DAMPING_FACTOR)

/* Tolerance parameter and divider to convert it to % */
#define FanController_TOLERANCE                   (1u)
#define FanController_TOLERANCE_DIVIDER           (100u)

#if (CY_PSOC3)
    #define FanController_NUM_OUT_TDS                 (FanController_NUMBER_OF_FANS * 2u)
#else
    #define FanController_NUM_OUT_TDS                 (FanController_NUMBER_OF_FANS)
#endif /* (CY_PSOC3) */

/***************************************
*             Registers
***************************************/

/* Buried control/status registers */
#define    FanController_GLOBAL_CONTROL_REG                 \
                                    (* (reg8 *) FanController_B_FanCtrl_GlobalControlReg__CONTROL_REG)
#define    FanController_GLOBAL_CONTROL_PTR                 \
                                    (  (reg8 *) FanController_B_FanCtrl_GlobalControlReg__CONTROL_REG)

#define    FanController_EOC_DMA_CONTROL_PTR                \
                                    (  (reg8 *) FanController_B_FanCtrl_DmaControlReg__CONTROL_REG)

#define FanController_ALERT_STATUS_REG                      \
                                    (* (reg8 *) FanController_B_FanCtrl_AlertStatusReg__STATUS_REG)
#define FanController_ALERT_STATUS_PTR                      \
                                    (  (reg8 *) FanController_B_FanCtrl_AlertStatusReg__STATUS_REG)
#define FanController_STATUS_ALERT_AUX_CTL_REG              \
                                    (* (reg8 *) FanController_B_FanCtrl_AlertStatusReg__STATUS_AUX_CTL_REG)
#define FanController_STATUS_ALERT_AUX_CTL_PTR              \
                                    (  (reg8 *) FanController_B_FanCtrl_AlertStatusReg__STATUS_AUX_CTL_REG)

#define FanController_TACH_GLITCH_FILTER_PERIOD_REG        \
                                    (* (reg8 *)  FanController_FanTach_GlitchFilterTimer_u0__D0_REG)

#if((!FanController_IS_MANUAL_MODE) && (FanController_DAMPING_FACTOR > 0u))
    #define FanController_DAMPING_COUNTER_DIVIDER_REG        \
                                        (* (reg8 *)  FanController_FanTach_GlitchFilterTimer_u0__D1_REG)
#endif /* ((!FanController_IS_MANUAL_MODE) && (FanController_DAMPING_FACTOR > 0u)) */

#if(CY_PSOC4)
    #define FanController_TACH_ADDR_CONTROL_REG             \
                                (* (reg8 *) FanController_FanTach_P4_TachAddrControl__CONTROL_REG)
    #define FanController_TACH_ADDR_CONTROL_PTR             \
                                (  (reg8 *) FanController_FanTach_P4_TachAddrControl__CONTROL_REG)
    #define FanController_TACH_CUNTER_AUX_CTL_REG           \
                                (* (reg16 *) FanController_FanTach_FanTachCounter_u0__16BIT_DP_AUX_CTL_REG)
    #define FanController_TACH_CUNTER_AUX_CTL_PTR           \
                                (  (reg16 *) FanController_FanTach_FanTachCounter_u0__16BIT_DP_AUX_CTL_REG)
    
    #define FanController_TACH_STORED_ACT_PERIOD_REG         \
                                ( *(reg16 *) FanController_FanTach_FanTachCounter_u0__16BIT_F0_REG)
    #define FanController_TACH_STORED_ACT_PERIOD_PTR         \
                                (  (reg16 *) FanController_FanTach_FanTachCounter_u0__16BIT_F0_REG)
#else
    
    /* Alert Mask LSB Control register */
    #define FanController_ALERT_MASK_LSB_CONTROL_REG            \
                                        (* (reg8 *) FanController_B_FanCtrl_HwCtl_AlertMaskLSB__CONTROL_REG)
    #define FanController_ALERT_MASK_LSB_CONTROL_PTR            \
                                        (  (reg8 *) FanController_B_FanCtrl_HwCtl_AlertMaskLSB__CONTROL_REG)

    #if(FanController_NUMBER_OF_FANS > 8u)
        /* Alert Mask MSB Control register */
        #define FanController_ALERT_MASK_MSB_CONTROL_REG        \
                               (* (reg8 *) FanController_B_FanCtrl_HwCtl_MSB_AlertMaskMSB__CONTROL_REG)
        #define FanController_ALERT_MASK_MSB_CONTROL_PTR        \
                               (  (reg8 *) FanController_B_FanCtrl_HwCrl_MSB_AlertMaskMSB__CONTROL_REG)
    #endif /* (FanController_NUMBER_OF_FANS > 8u) */

    /* Stall Status register */
    #define FanController_STALL_ERROR_LSB_STATUS_REG            \
                                        (* (reg8 *) FanController_B_FanCtrl_StallStatus_StallError_LSB__STATUS_REG)
    #define FanController_STALL_ERROR_LSB_STATUS_PTR            \
                                        (  (reg8 *) FanController_B_FanCtrl_StallStatus_StallError_LSB__STATUS_REG)

    #if(FanController_NUMBER_OF_FANS > 8u)
        /* Stall Status MSB register */
        #define FanController_STALL_ERROR_MSB_STATUS_REG        \
                                    (* (reg8 *) FanController_B_FanCtrl_StallStatus_MSB_StallError_MSB__STATUS_REG)
        #define FanController_STALL_ERROR_MSB_STATUS_PTR        \
                                    (  (reg8 *) FanController_B_FanCtrl_StallStatus_MSB_StallError_MSB__STATUS_REG)
    #endif /* (FanController_NUMBER_OF_FANS > 8u) */

    #define FanController_TACH_FAN_COUNTER_REG               \
                                (* (reg8 *) FanController_FanTach_P3_P5_FanCounter__COUNT_REG)
    #define FanController_TACH_FAN_COUNTER_PTR               \
                                (  (reg8 *) FanController_FanTach_P3_P5_FanCounter__COUNT_REG)
    #define FanController_TACH_FAN_COUNTR_AUX_CTL_REG        \
                                (* (reg8 *) FanController_FanTach_P3_P5_FanCounter__CONTROL_AUX_CTL_REG)
    #define FanController_TACH_FAN_COUNTR_AUX_CTL_PTR        \
                                (  (reg8 *) FanController_P3_P5_FanTach_FanCounter__CONTROL_AUX_CTL_REG)
#endif /* (CY_PSOC4) */

/* Register defines inside the embedded tach component - these are sources/destinations for DMA controller */
#define FanController_TACH_TOLERANCE_PTR                  (FanController_FanTach_FanTachCounter_u0__16BIT_D1_REG)
#define FanController_TACH_DESIRED_PERIOD_PTR             (FanController_FanTach_FanTachCounter_u0__16BIT_D0_REG)
#define FanController_TACH_ACTUAL_PERIOD_PTR              (FanController_FanTach_FanTachCounter_u0__16BIT_A0_REG)

#if((!FanController_IS_MANUAL_MODE) && (FanController_DAMPING_FACTOR > 0u))

    /* Damping Factor counter Registers */
    #define FanController_TACH_DAMPING_PERIOD_LOW_LSB_REG    \
                                    (*  (reg8 *) FanController_FanTach_DmpgFactor_DmpgTimeCntr_u0__D0_REG)
    #define FanController_TACH_DAMPING_PERIOD_LOW_LSB_PTR    \
                                    (   (reg8 *) FanController_FanTach_DmpgFactor_DmpgTimeCntr_u0__D0_REG)
    #define FanController_TACH_DAMPING_PERIOD_HIGH_LSB_REG   \
                                    (*  (reg8 *) FanController_FanTach_DmpgFactor_DmpgTimeCntr_u0__D1_REG)
    #define FanController_TACH_DAMPING_PERIOD_HIGH_LSB_PTR   \
                                    (   (reg8 *) FanController_FanTach_DmpgFactor_DmpgTimeCntr_u0__D1_REG)

#endif /* ((!FanController_IS_MANUAL_MODE) && (FanController_DAMPING_FACTOR > 0u)) */

#if(FanController_IS_AUTO_HARDWARE_MODE)
    #define FanController_SPEED_ERROR_LSB_STATUS_REG         \
                                          (* (reg8 *) FanController_B_FanCtrl_SpeedAlrt_SpeedError_LSB__STATUS_REG)

    #if(FanController_NUMBER_OF_FANS > 8u)
        #define FanController_SPEED_ERROR_MSB_STATUS_REG     \
                                      (* (reg8 *) FanController_B_FanCtrl_SpeedAlrt_MSB_SpeedError_MSB__STATUS_REG )
    #endif /* FanController_NUMBER_OF_FANS > 8u */
#endif /* (FanController_IS_AUTO_HARDWARE_MODE) */

#if(CY_PSOC3)

    /* DMA status. Used to capture the status of DMA state machine */
    #define FanController_DMA_STATUS_REG     (* (reg8 *) FanController_B_FanCtrl_DmaEoc_Sm_Dma_Status__STATUS_REG)

#endif /* (CY_PSOC3) */


/***************************************
*       Register Constants
***************************************/

#if(CY_PSOC3)

    #define FanController_DMA1                       (0x01u)
    #define FanController_DMA2                       (0x02u)

#endif /* (CY_PSOC3) */

#define FanController_COUNT7_ENABLE              (0x20u)
#define FanController_STATUS_ALERT_ENABLE        (0x10u)

#define FanController_COUNT7_PERIOD_MASK         (0x7Fu)

#if (!CY_PSOC4)
    #if(!FanController_IS_AUTO_HARDWARE_MODE)
        #define FanController_FANPWM_AUX_CTRL_FIFO0_CLR_8    (0x03u)
        #define FanController_FANPWM_AUX_CTRL_FIFO0_CLR_10   (0x0303u)
    #endif /* (!FanController_IS_AUTO_HARDWARE_MODE) */
#else    
    #define FanController_TACH_COUNTER_FIFO_CLR      (0x0101u)
    #define FanController_TACH_CNTL_NUM_FANS_MASK    ((uint8)(0x0Fu))
    #define FanController_TACH_EOM                   ((uint8)(0x80u))
    #define FanController_SW_EOC                     ((uint8)(0x08u))
    #define FanController_SW_STALL                   ((uint8)(0x80u))
#endif /* (!CY_PSOC4) */

/* Control Register bit defines */
#define FanController_ALERT_PIN_ENABLE_SHIFT     (0x00u)
#define FanController_ALERT_ENABLE_SHIFT         (0x01u)
#define FanController_STALL_ALERT_ENABLE_SHIFT   (0x01u)
#define FanController_SPEED_ALERT_ENABLE_SHIFT   (0x02u)
#if (!FanController_IS_MANUAL_MODE)
    #define FanController_SW_SPEED_ERROR_SHIFT   (0x04u)    
#endif /* (!FanController_IS_MANUAL_MODE) */

#define FanController_ENABLE_SHIFT               (0x05u)
#define FanController_OVERRIDE_SHIFT             (0x06u)

#define FanController_ALERT_PIN_ENABLE           (uint8)(0x01u << FanController_ALERT_PIN_ENABLE_SHIFT)
#define FanController_STALL_ALERT_ENABLE         (uint8)(0x01u << FanController_STALL_ALERT_ENABLE_SHIFT)
#define FanController_SPEED_ALERT_ENABLE         (uint8)(0x01u << FanController_SPEED_ALERT_ENABLE_SHIFT)
#define FanController_ENABLE                     (uint8)(0x01u << FanController_ENABLE_SHIFT)
#define FanController_OVERRIDE                   (uint8)(0x01u << FanController_OVERRIDE_SHIFT)
#if (!FanController_IS_MANUAL_MODE)
    #define FanController_SW_SPEED_ERROR         (uint8)(0x01u << FanController_SW_SPEED_ERROR_SHIFT)
#endif /* (!FanController_IS_MANUAL_MODE) */


#define FanController_ALERT_ENABLE_MASK          (uint8)(0x03u << FanController_ALERT_ENABLE_SHIFT)

#define FanController_ALERT_STATUS_MASK          (0x03u)

#define FanController_GLITCH_FILTER_VAL          (60u)

#if ((!FanController_IS_MANUAL_MODE) && (FanController_DAMPING_FACTOR > 0u))
    #define FanController_DAMPING_FACTOR_DIVIDER_VAL          (10u)
#endif /* ((!FanController_IS_MANUAL_MODE) && (FanController_DAMPING_FACTOR > 0u)) */


#endif /* FanController_CY_FAN_CONTROLLER_H */


/* [] END OF FILE */
