/*******************************************************************************
* File Name: FanController_1.h
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

#if !defined(FanController_1_CY_FAN_CONTROLLER_H)
#define FanController_1_CY_FAN_CONTROLLER_H

#include "cytypes.h"
#include "cyfitter.h"
#if (!CY_PSOC4)
    #include <CYDMAC.H>
#endif /* (!CY_PSOC4) */


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define FanController_1_FAN_CTL_MODE               (1u)
#define FanController_1_NUMBER_OF_FANS             (1u)
#define FanController_1_NUMBER_OF_BANKS            (0u)
#define FanController_1_PWMRES                     (0u)

#define FanController_1_MANUAL_MODE                (0u)

#define FanController_1_MANUAL_MODE_TRUE           (1u)
#define FanController_1_MANUAL_MODE_FALSE          (0u)

#define FanController_1_FANCTLMODE_FIRMWARE        (0u)
#define FanController_1_FANCTLMODE_HARDWARE        (1u)

#define FanController_1_NUMBER_OF_FAN_OUTPUTS      (1u)

/* Resolution constants */
#define FanController_1_PWMRES_EIGHTBIT            (0u)
#define FanController_1_PWMRES_TENBIT              (1u)

/* PWM Configuration Constants */
#if(FanController_1_PWMRES == FanController_1_PWMRES_EIGHTBIT)
    #define FanController_1_PWM_PERIOD                 (240u)
#else
    #define FanController_1_PWM_PERIOD                 (960u)
#endif /*FanController_1_PWMRES == FanController_1_PWMRES_EIGHTBIT */

/* Macros to identify if Automatic or Manual Control mode is used */
#define FanController_1_IS_AUTO_HARDWARE_MODE      \
                (FanController_1_FAN_CTL_MODE == FanController_1_FANCTLMODE_HARDWARE)
#define FanController_1_IS_MANUAL_MODE             \
                ((FanController_1_MANUAL_MODE == FanController_1_MANUAL_MODE_TRUE) && \
                    ((FanController_1_FAN_CTL_MODE != FanController_1_FANCTLMODE_HARDWARE)))
#define FanController_1_IS_AUTO_FIRMWARE_MODE      \
                ((FanController_1_FAN_CTL_MODE == FanController_1_FANCTLMODE_FIRMWARE) && \
                    (!(FanController_1_IS_MANUAL_MODE)))

/* PID Constants */
#if(FanController_1_IS_AUTO_FIRMWARE_MODE)
    #define FanController_1_PID_A1             (716)
    #define FanController_1_PID_A2             (-614)
    #define FanController_1_PID_A3             (0)

    #define FanController_1_PID_POST_GAIN      (20u)

    #define FanController_1_PID_OUTPUT_SAT_L   (0u)
    #define FanController_1_PID_OUTPUT_SAT_H   (251658240u)

    /* PID error low and high saturation. These are constants defined
    * by EROS to be (-4096) and (4096) respectively.
    */
    #define FanController_1_PID_ERROR_SAT_L    (-4096)
    #define FanController_1_PID_ERROR_SAT_H    (4096)

#endif /* (FanController_1_IS_AUTO_FIRMWARE_MODE) */


/***************************************
*        Function Prototypes
***************************************/

void    FanController_1_Start(void) ;
void    FanController_1_Stop(void) ;
void    FanController_1_Init(void) ;
void    FanController_1_Enable(void) ;
void    FanController_1_EnableAlert(void) ;
void    FanController_1_DisableAlert(void) ;
void    FanController_1_SetAlertMode(uint8 alertMode) ;
uint8   FanController_1_GetAlertMode(void) ;
void    FanController_1_SetAlertMask(uint16 alertMask) ;
uint16  FanController_1_GetAlertMask(void) ;
uint8   FanController_1_GetAlertSource(void) ;
uint16  FanController_1_GetFanStallStatus(void) ;
void    FanController_1_SetDutyCycle(uint8 fanOrBankNumber, uint16 dutyCycle)
                                                        ;
uint16  FanController_1_GetDutyCycle(uint8 fanOrBankNumber) ;
void    FanController_1_SetDesiredSpeed(uint8 fanNumber, uint16 rpm)
                                                        ;
uint16  FanController_1_GetActualSpeed(uint8 fanNumber) ;

#if (FanController_1_IS_AUTO_FIRMWARE_MODE)
    void FanController_1_SetPID(uint8 fanNum, uint16 kp, uint16 ki, uint16 kd)
            ;
    void FanController_1_SetSaturation(uint8 fanNum, uint16 satH, uint16 satL)
            ;
#endif /* (FanController_1_IS_AUTO_FIRMWARE_MODE) */

#if (!FanController_1_IS_MANUAL_MODE)
    #if (FanController_1_IS_AUTO_HARDWARE_MODE)
        void    FanController_1_OverrideAutomaticControl(uint8 override)
                                                        ;
    #endif /* (FanController_1_IS_AUTO_HARDWARE_MODE)*/

    uint16  FanController_1_GetDesiredSpeed(uint8 fanNumber) ;
    uint16  FanController_1_GetFanSpeedStatus(void) ;
#endif /* (!FanController_1_IS_MANUAL_MODE) */

/* Interrupt handlers */
#if(CY_PSOC4)
    /* ISR for transferring data from Tachometer to RAM */
    CY_ISR_PROTO(FanController_1_DATA_SEND_ISR);
#endif /* (CY_PSOC4) */

#if(FanController_1_IS_AUTO_FIRMWARE_MODE)
    /* PID Control algorithm ISR */
    CY_ISR_PROTO(FanController_1_PID_CONTROL_ISR);
#endif /* (FanController_1_IS_AUTO_FIRMWARE_MODE) */


/***************************************
*           API Constants
***************************************/

/* Constants for Acoustic Noise Reduction */
#define FanController_1_NOISE_REDUCTION_OFF        (0u)
#define FanController_1_NOISE_REDUCTION_ON         (1u)

/* Bit definitions for the Alert Source Status Register (FanController_1_GetAlertSource()) */
#define FanController_1_STALL_ALERT                (0x01u)
#define FanController_1_SPEED_ALERT                (0x02u)

/* End of Conversion constants */
#define FanController_1_EOC_HIGH                   (0x01u)
#define FanController_1_EOC_LOW                    (0x00u)

/* Maximum possible Fans number */
#define FanController_1_MAX_FANS                   (16u)

/* Threshold for speed regulation error occurrence */
#define FanController_1_SPEED_ERRORS_TRESHOLD      (16u)
#define FanController_1_MAX_SPEED_ERRORS           (255u)

#if (FanController_1_IS_AUTO_FIRMWARE_MODE)
    #define FanController_1_MAX_GAIN                   (65535u)
    #define FanController_1_GAIN_MULTIPLIER            (4096u)
#endif /* (FanController_1_IS_AUTO_HARDWARE_MODE) */

/* RPM to Duty Cycle Conversion Constants */
#define FanController_1_TACH_CLOCK_FREQ            (500000u)
#define FanController_1_RPM_PERIOD_CONV_FACTOR     (FanController_1_TACH_CLOCK_FREQ * 60u)

#define FanController_1_PWM_INIT_DUTY              (FanController_1_PWM_PERIOD)
#define FanController_1_PWM_DUTY_DIVIDER           (10000u)      /* API Duty Cycle is expressed in 100ths of % */

#if (CY_PSOC3)
    /* 8051 registers have 16-bit addresses */
    #define FanController_1_RegAddrType            uint16
    /* DMA TD endian swap flag - 8051 is big endian */
    #define FanController_1_TD_SWAP_ENDIAN_FLAG    (TD_SWAP_EN)
#else
    /* ARM registers have 32-bit addresses */
    #define FanController_1_RegAddrType            uint32
    /* DMA TD endian swap flag - ARM is little endian */
    #define FanController_1_TD_SWAP_ENDIAN_FLAG    (0u)
#endif /* CY_PSOC3 */

#define FanController_1_TachOutDMA__TD_TERMOUT_EN ((FanController_1_TachOutDMA__TERMOUT0_EN ? TD_TERMOUT0_EN : 0u) | \
    (FanController_1_TachOutDMA__TERMOUT1_EN ? TD_TERMOUT1_EN : 0u))

#if(FanController_1_IS_AUTO_HARDWARE_MODE)
    #define FanController_1_TachInDMA__TD_TERMOUT_EN ((FanController_1_TachInDMA__TERMOUT0_EN ? \
                                TD_TERMOUT0_EN : 0u) | (FanController_1_TachInDMA__TERMOUT1_EN ? TD_TERMOUT1_EN : 0u))
#endif /* FanController_1_IS_AUTO_HARDWARE_MODE */

extern uint8 FanController_1_tachOutDMA_DmaHandle;
#if(FanController_1_IS_AUTO_HARDWARE_MODE)
    extern uint8 FanController_1_tachInDMA_DmaHandle;
#endif /* FanController_1_IS_AUTO_HARDWARE_MODE */


/***************************************
*     Data Struct Definitions
***************************************/

/* Hardware and Firmware Fan Control Mode DMA transaction descriptors to store actual fan speeds in SRAM */
typedef struct
{
   uint8     setActualPeriodTD;                           /* Actual tach period from Tach block to SRAM */
} FanController_1_fanTdOutStruct;

/* Hardware Fan Control Mode DMA transaction descriptors to read desired speeds and tolerance from SRAM */
#if(FanController_1_IS_AUTO_HARDWARE_MODE)
    typedef struct
    {
        uint8     getDesiredPeriodTD;                       /* Desired tach period from SRAM to Tach block */
        uint8     getToleranceTD;                           /* Desired period tolerance from SRAM to Tach block */
    } FanController_1_fanTdInStruct;
#endif /* FanController_1_IS_AUTO_HARDWARE_MODE */

/* DMA Controller SRAM Structure */
typedef struct
{
    #if(FanController_1_IS_AUTO_HARDWARE_MODE)
        uint16  desiredPeriod[FanController_1_NUMBER_OF_FANS];
        uint16  tolerance[FanController_1_NUMBER_OF_FANS];
    #endif /* FanController_1_IS_AUTO_HARDWARE_MODE */

    #if(CY_PSOC3)
        uint16  actualPeriodDma1[FanController_1_NUMBER_OF_FANS];
        uint16  actualPeriodDma2[FanController_1_NUMBER_OF_FANS];
    #else
        uint16  actualPeriod[FanController_1_NUMBER_OF_FANS];
    #endif /* (CY_PSoC3) */

} FanController_1_fanControlStruct;

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
} FanController_1_fanPropertiesStruct;

/* PWM configuration structure */
#if (!CY_PSOC4)
    #if(FanController_1_IS_AUTO_HARDWARE_MODE)

        /* An array that holds addresses of PWM configuration registers for Auto
        * Hardware mode.
        */
        typedef struct
        {
            FanController_1_RegAddrType  pwmPeriodReg;
            FanController_1_RegAddrType  pwmMaxDutyReg;
            FanController_1_RegAddrType  pwmSetDutyReg;
            FanController_1_RegAddrType  errorCountReg;
        } FanController_1_fanDriverRegsStruct;

    #else
        /* An array that holds addresses of PWM configuration registers for Auto
        * Firmware and Manual modes (PSoC 3/5LP).
        */
        typedef struct
        {
            FanController_1_RegAddrType  pwmSetDutyReg;
        } FanController_1_fanDriverRegsStruct;

        typedef struct
        {
            FanController_1_RegAddrType  pwmPeriodReg;
            FanController_1_RegAddrType  pwmAuxControlReg;
        } FanController_1_fanPwmInitRegsStruct;

        #define FanController_1_FANPWM_COUNT                   ((FanController_1_NUMBER_OF_FAN_OUTPUTS / 2u) + \
                    (FanController_1_NUMBER_OF_FAN_OUTPUTS % 2u))

        extern const  FanController_1_fanPwmInitRegsStruct CYCODE 
            FanController_1_fanPwmInitRegs[FanController_1_FANPWM_COUNT];

    #endif /* FanController_1_IS_AUTO_HARDWARE_MODE */

#else /* PSoC 4 */
    /* An array that holds addresses of PWM configuration registers for Auto
    * Firmware and Manual modes (PSoC 4).
    */
    typedef struct
    {
        FanController_1_RegAddrType  pwmSetDutyReg;
    } FanController_1_fanDriverRegsStruct;

    typedef struct
    {
        FanController_1_RegAddrType  pwmPeriodReg;
    } FanController_1_fanPwmInitRegsStruct;

    extern const FanController_1_fanPwmInitRegsStruct CYCODE
        FanController_1_fanPwmInitRegs[FanController_1_NUMBER_OF_FAN_OUTPUTS];

#endif /* (!CY_PSOC4) */

extern uint8 FanController_1_initVar;

extern FanController_1_fanControlStruct FanController_1_fanControl;

#if(!FanController_1_IS_MANUAL_MODE)
    /* An array to store desired fans speeds */
    extern uint16 FanController_1_desiredFansSpeed[FanController_1_NUMBER_OF_FANS];
#endif /* (!FanController_1_IS_MANUAL_MODE) */

/* Array which store PID parameters for each fan */
#if(FanController_1_IS_AUTO_FIRMWARE_MODE)
    extern int16 FanController_1_pidA1[FanController_1_NUMBER_OF_FANS];
    extern int16 FanController_1_pidA2[FanController_1_NUMBER_OF_FANS];
    extern int16 FanController_1_pidA3[FanController_1_NUMBER_OF_FANS];

    extern uint32 FanController_1_outputSatL[FanController_1_NUMBER_OF_FANS];
    extern uint32 FanController_1_outputSatH[FanController_1_NUMBER_OF_FANS];

    extern volatile uint16 FanController_1_speedStatus;
#endif /* (FanController_1_IS_AUTO_FIRMWARE_MODE) */

extern const FanController_1_fanDriverRegsStruct CYCODE
    FanController_1_fanDriverRegs[FanController_1_NUMBER_OF_FAN_OUTPUTS];

extern FanController_1_fanPropertiesStruct FanController_1_fanProperties[FanController_1_NUMBER_OF_FANS];

#if (CY_PSOC4)
    extern uint16 FanController_1_alertMask;
    extern uint16 FanController_1_stallStatus;
#endif /* (CY_PSOC4) */

/***************************************
* Initial Parameter Constants
***************************************/

#define FanController_1_MAX_FAN_NUM                 (FanController_1_NUMBER_OF_FANS - 1u)
#define FanController_1_DAMPING_FACTOR              (50u)
#define FanController_1_INIT_ALERT_ENABLE           (0u)
#define FanController_1_NOISE_REDUCTION_MODE        (0u)
#define FanController_1_INIT_ALERT_MASK             ((uint16) (((uint32) 0x01u << 1u) - 1u))
#define FanController_1_DAMPING_FACTOR_PERIOD_LOW   (250u)
#define FanController_1_DAMPING_FACTOR_PERIOD_HIGH  (FanController_1_DAMPING_FACTOR)

/* Tolerance parameter and divider to convert it to % */
#define FanController_1_TOLERANCE                   (1u)
#define FanController_1_TOLERANCE_DIVIDER           (100u)

#if (CY_PSOC3)
    #define FanController_1_NUM_OUT_TDS                 (FanController_1_NUMBER_OF_FANS * 2u)
#else
    #define FanController_1_NUM_OUT_TDS                 (FanController_1_NUMBER_OF_FANS)
#endif /* (CY_PSOC3) */

/***************************************
*             Registers
***************************************/

/* Buried control/status registers */
#define    FanController_1_GLOBAL_CONTROL_REG                 \
                                    (* (reg8 *) FanController_1_B_FanCtrl_GlobalControlReg__CONTROL_REG)
#define    FanController_1_GLOBAL_CONTROL_PTR                 \
                                    (  (reg8 *) FanController_1_B_FanCtrl_GlobalControlReg__CONTROL_REG)

#define    FanController_1_EOC_DMA_CONTROL_PTR                \
                                    (  (reg8 *) FanController_1_B_FanCtrl_DmaControlReg__CONTROL_REG)

#define FanController_1_ALERT_STATUS_REG                      \
                                    (* (reg8 *) FanController_1_B_FanCtrl_AlertStatusReg__STATUS_REG)
#define FanController_1_ALERT_STATUS_PTR                      \
                                    (  (reg8 *) FanController_1_B_FanCtrl_AlertStatusReg__STATUS_REG)
#define FanController_1_STATUS_ALERT_AUX_CTL_REG              \
                                    (* (reg8 *) FanController_1_B_FanCtrl_AlertStatusReg__STATUS_AUX_CTL_REG)
#define FanController_1_STATUS_ALERT_AUX_CTL_PTR              \
                                    (  (reg8 *) FanController_1_B_FanCtrl_AlertStatusReg__STATUS_AUX_CTL_REG)

#define FanController_1_TACH_GLITCH_FILTER_PERIOD_REG        \
                                    (* (reg8 *)  FanController_1_FanTach_GlitchFilterTimer_u0__D0_REG)

#if((!FanController_1_IS_MANUAL_MODE) && (FanController_1_DAMPING_FACTOR > 0u))
    #define FanController_1_DAMPING_COUNTER_DIVIDER_REG        \
                                        (* (reg8 *)  FanController_1_FanTach_GlitchFilterTimer_u0__D1_REG)
#endif /* ((!FanController_1_IS_MANUAL_MODE) && (FanController_1_DAMPING_FACTOR > 0u)) */

#if(CY_PSOC4)
    #define FanController_1_TACH_ADDR_CONTROL_REG             \
                                (* (reg8 *) FanController_1_FanTach_P4_TachAddrControl__CONTROL_REG)
    #define FanController_1_TACH_ADDR_CONTROL_PTR             \
                                (  (reg8 *) FanController_1_FanTach_P4_TachAddrControl__CONTROL_REG)
    #define FanController_1_TACH_CUNTER_AUX_CTL_REG           \
                                (* (reg16 *) FanController_1_FanTach_FanTachCounter_u0__16BIT_DP_AUX_CTL_REG)
    #define FanController_1_TACH_CUNTER_AUX_CTL_PTR           \
                                (  (reg16 *) FanController_1_FanTach_FanTachCounter_u0__16BIT_DP_AUX_CTL_REG)
    
    #define FanController_1_TACH_STORED_ACT_PERIOD_REG         \
                                ( *(reg16 *) FanController_1_FanTach_FanTachCounter_u0__16BIT_F0_REG)
    #define FanController_1_TACH_STORED_ACT_PERIOD_PTR         \
                                (  (reg16 *) FanController_1_FanTach_FanTachCounter_u0__16BIT_F0_REG)
#else
    
    /* Alert Mask LSB Control register */
    #define FanController_1_ALERT_MASK_LSB_CONTROL_REG            \
                                        (* (reg8 *) FanController_1_B_FanCtrl_HwCtl_AlertMaskLSB__CONTROL_REG)
    #define FanController_1_ALERT_MASK_LSB_CONTROL_PTR            \
                                        (  (reg8 *) FanController_1_B_FanCtrl_HwCtl_AlertMaskLSB__CONTROL_REG)

    #if(FanController_1_NUMBER_OF_FANS > 8u)
        /* Alert Mask MSB Control register */
        #define FanController_1_ALERT_MASK_MSB_CONTROL_REG        \
                               (* (reg8 *) FanController_1_B_FanCtrl_HwCtl_MSB_AlertMaskMSB__CONTROL_REG)
        #define FanController_1_ALERT_MASK_MSB_CONTROL_PTR        \
                               (  (reg8 *) FanController_1_B_FanCtrl_HwCrl_MSB_AlertMaskMSB__CONTROL_REG)
    #endif /* (FanController_1_NUMBER_OF_FANS > 8u) */

    /* Stall Status register */
    #define FanController_1_STALL_ERROR_LSB_STATUS_REG            \
                                        (* (reg8 *) FanController_1_B_FanCtrl_StallStatus_StallError_LSB__STATUS_REG)
    #define FanController_1_STALL_ERROR_LSB_STATUS_PTR            \
                                        (  (reg8 *) FanController_1_B_FanCtrl_StallStatus_StallError_LSB__STATUS_REG)

    #if(FanController_1_NUMBER_OF_FANS > 8u)
        /* Stall Status MSB register */
        #define FanController_1_STALL_ERROR_MSB_STATUS_REG        \
                                    (* (reg8 *) FanController_1_B_FanCtrl_StallStatus_MSB_StallError_MSB__STATUS_REG)
        #define FanController_1_STALL_ERROR_MSB_STATUS_PTR        \
                                    (  (reg8 *) FanController_1_B_FanCtrl_StallStatus_MSB_StallError_MSB__STATUS_REG)
    #endif /* (FanController_1_NUMBER_OF_FANS > 8u) */

    #define FanController_1_TACH_FAN_COUNTER_REG               \
                                (* (reg8 *) FanController_1_FanTach_P3_P5_FanCounter__COUNT_REG)
    #define FanController_1_TACH_FAN_COUNTER_PTR               \
                                (  (reg8 *) FanController_1_FanTach_P3_P5_FanCounter__COUNT_REG)
    #define FanController_1_TACH_FAN_COUNTR_AUX_CTL_REG        \
                                (* (reg8 *) FanController_1_FanTach_P3_P5_FanCounter__CONTROL_AUX_CTL_REG)
    #define FanController_1_TACH_FAN_COUNTR_AUX_CTL_PTR        \
                                (  (reg8 *) FanController_1_P3_P5_FanTach_FanCounter__CONTROL_AUX_CTL_REG)
#endif /* (CY_PSOC4) */

/* Register defines inside the embedded tach component - these are sources/destinations for DMA controller */
#define FanController_1_TACH_TOLERANCE_PTR                  (FanController_1_FanTach_FanTachCounter_u0__16BIT_D1_REG)
#define FanController_1_TACH_DESIRED_PERIOD_PTR             (FanController_1_FanTach_FanTachCounter_u0__16BIT_D0_REG)
#define FanController_1_TACH_ACTUAL_PERIOD_PTR              (FanController_1_FanTach_FanTachCounter_u0__16BIT_A0_REG)

#if((!FanController_1_IS_MANUAL_MODE) && (FanController_1_DAMPING_FACTOR > 0u))

    /* Damping Factor counter Registers */
    #define FanController_1_TACH_DAMPING_PERIOD_LOW_LSB_REG    \
                                    (*  (reg8 *) FanController_1_FanTach_DmpgFactor_DmpgTimeCntr_u0__D0_REG)
    #define FanController_1_TACH_DAMPING_PERIOD_LOW_LSB_PTR    \
                                    (   (reg8 *) FanController_1_FanTach_DmpgFactor_DmpgTimeCntr_u0__D0_REG)
    #define FanController_1_TACH_DAMPING_PERIOD_HIGH_LSB_REG   \
                                    (*  (reg8 *) FanController_1_FanTach_DmpgFactor_DmpgTimeCntr_u0__D1_REG)
    #define FanController_1_TACH_DAMPING_PERIOD_HIGH_LSB_PTR   \
                                    (   (reg8 *) FanController_1_FanTach_DmpgFactor_DmpgTimeCntr_u0__D1_REG)

#endif /* ((!FanController_1_IS_MANUAL_MODE) && (FanController_1_DAMPING_FACTOR > 0u)) */

#if(FanController_1_IS_AUTO_HARDWARE_MODE)
    #define FanController_1_SPEED_ERROR_LSB_STATUS_REG         \
                                          (* (reg8 *) FanController_1_B_FanCtrl_SpeedAlrt_SpeedError_LSB__STATUS_REG)

    #if(FanController_1_NUMBER_OF_FANS > 8u)
        #define FanController_1_SPEED_ERROR_MSB_STATUS_REG     \
                                      (* (reg8 *) FanController_1_B_FanCtrl_SpeedAlrt_MSB_SpeedError_MSB__STATUS_REG )
    #endif /* FanController_1_NUMBER_OF_FANS > 8u */
#endif /* (FanController_1_IS_AUTO_HARDWARE_MODE) */

#if(CY_PSOC3)

    /* DMA status. Used to capture the status of DMA state machine */
    #define FanController_1_DMA_STATUS_REG     (* (reg8 *) FanController_1_B_FanCtrl_DmaEoc_Sm_Dma_Status__STATUS_REG)

#endif /* (CY_PSOC3) */


/***************************************
*       Register Constants
***************************************/

#if(CY_PSOC3)

    #define FanController_1_DMA1                       (0x01u)
    #define FanController_1_DMA2                       (0x02u)

#endif /* (CY_PSOC3) */

#define FanController_1_COUNT7_ENABLE              (0x20u)
#define FanController_1_STATUS_ALERT_ENABLE        (0x10u)

#define FanController_1_COUNT7_PERIOD_MASK         (0x7Fu)

#if (!CY_PSOC4)
    #if(!FanController_1_IS_AUTO_HARDWARE_MODE)
        #define FanController_1_FANPWM_AUX_CTRL_FIFO0_CLR_8    (0x03u)
        #define FanController_1_FANPWM_AUX_CTRL_FIFO0_CLR_10   (0x0303u)
    #endif /* (!FanController_1_IS_AUTO_HARDWARE_MODE) */
#else    
    #define FanController_1_TACH_COUNTER_FIFO_CLR      (0x0101u)
    #define FanController_1_TACH_CNTL_NUM_FANS_MASK    ((uint8)(0x0Fu))
    #define FanController_1_TACH_EOM                   ((uint8)(0x80u))
    #define FanController_1_SW_EOC                     ((uint8)(0x08u))
    #define FanController_1_SW_STALL                   ((uint8)(0x80u))
#endif /* (!CY_PSOC4) */

/* Control Register bit defines */
#define FanController_1_ALERT_PIN_ENABLE_SHIFT     (0x00u)
#define FanController_1_ALERT_ENABLE_SHIFT         (0x01u)
#define FanController_1_STALL_ALERT_ENABLE_SHIFT   (0x01u)
#define FanController_1_SPEED_ALERT_ENABLE_SHIFT   (0x02u)
#if (!FanController_1_IS_MANUAL_MODE)
    #define FanController_1_SW_SPEED_ERROR_SHIFT   (0x04u)    
#endif /* (!FanController_1_IS_MANUAL_MODE) */

#define FanController_1_ENABLE_SHIFT               (0x05u)
#define FanController_1_OVERRIDE_SHIFT             (0x06u)

#define FanController_1_ALERT_PIN_ENABLE           (uint8)(0x01u << FanController_1_ALERT_PIN_ENABLE_SHIFT)
#define FanController_1_STALL_ALERT_ENABLE         (uint8)(0x01u << FanController_1_STALL_ALERT_ENABLE_SHIFT)
#define FanController_1_SPEED_ALERT_ENABLE         (uint8)(0x01u << FanController_1_SPEED_ALERT_ENABLE_SHIFT)
#define FanController_1_ENABLE                     (uint8)(0x01u << FanController_1_ENABLE_SHIFT)
#define FanController_1_OVERRIDE                   (uint8)(0x01u << FanController_1_OVERRIDE_SHIFT)
#if (!FanController_1_IS_MANUAL_MODE)
    #define FanController_1_SW_SPEED_ERROR         (uint8)(0x01u << FanController_1_SW_SPEED_ERROR_SHIFT)
#endif /* (!FanController_1_IS_MANUAL_MODE) */


#define FanController_1_ALERT_ENABLE_MASK          (uint8)(0x03u << FanController_1_ALERT_ENABLE_SHIFT)

#define FanController_1_ALERT_STATUS_MASK          (0x03u)

#define FanController_1_GLITCH_FILTER_VAL          (60u)

#if ((!FanController_1_IS_MANUAL_MODE) && (FanController_1_DAMPING_FACTOR > 0u))
    #define FanController_1_DAMPING_FACTOR_DIVIDER_VAL          (10u)
#endif /* ((!FanController_1_IS_MANUAL_MODE) && (FanController_1_DAMPING_FACTOR > 0u)) */


#endif /* FanController_1_CY_FAN_CONTROLLER_H */


/* [] END OF FILE */
