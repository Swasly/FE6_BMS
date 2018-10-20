/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>

extern uint8_t can_buffer[];

typedef enum{
    NO_ERROR=0x0000,
    CHARGEMODE=0x0001,
    PACK_TEMP_OVER=0x0002,
    PACK_TEMP_CLOSE=0x0004,
    PACK_TEMP_UNDER=0x0008,
    LOW_SOC = 0x0010,
    CRITICAL_SOC = 0x0020,
    IMBALANCE = 0x0040,
    COM_FAILURE = 0x0080,
    NEG_CONT_CLOSED = 0x0100,
    POS_CONT_CLOSED = 0x0200,
    ISO_FAULT = 0x0400,
    CELL_VOLT_OVER = 0x0800,
    CELL_VOLT_UNDER = 0x1000,
    CHARGE_HAULT = 0x2000,
    FULL = 0x4000,
    PRECHARGE_CLOSED = 0x8000
}BMS_STATUS;


void CAN_Pack_status(uint8_t SOC_P,
                    uint8_t AH,
                    BMS_STATUS status,
                    uint16_t charge_cy,
                    uint16_t delta){
//8 SOC Percent
//8 AH used since full charge
//16 BMS Status bits (error flags)
//16 Number of charge cycles
//16 Pack balance (delta) mV
    can_buffer[0] = SOC_P;
    can_buffer[1] = AH;
    can_buffer[2] = (status>>8) & 0xFF;
    can_buffer[3] = (status) & 0xFF;
    can_buffer[4] = (charge_cy>>8) & 0xFF;
    can_buffer[5] = (charge_cy) & 0xFF;
    can_buffer[6] = (delta>>8) & 0xFF;
    can_buffer[7] = (delta) & 0xFF;
    CAN_1_SendMsgvolt();
}
                    
                    

/* [] END OF FILE */
