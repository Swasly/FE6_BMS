/*******************************************************************************
* File Name: SOC_Store.h
* Version 3.0
*
*  Description:
*   Provides the function definitions for the EEPROM APIs.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_EEPROM_SOC_Store_H)
#define CY_EEPROM_SOC_Store_H

#include "cydevice_trm.h"
#include "CyFlash.h"

#if !defined(CY_PSOC5LP)
    #error Component EEPROM_v3_0 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void SOC_Store_Enable(void) ;
void SOC_Store_Start(void) ;
void SOC_Store_Stop (void) ;
cystatus SOC_Store_WriteByte(uint8 dataByte, uint16 address) \
                                            ;
uint8 SOC_Store_ReadByte(uint16 address) ;
uint8 SOC_Store_UpdateTemperature(void) ;
cystatus SOC_Store_EraseSector(uint8 sectorNumber) ;
cystatus SOC_Store_Write(const uint8 * rowData, uint8 rowNumber) ;
cystatus SOC_Store_StartWrite(const uint8 * rowData, uint8 rowNumber) \
                                                ;
cystatus SOC_Store_StartErase(uint8 sectorNumber) ;
cystatus SOC_Store_Query(void) ;
cystatus SOC_Store_ByteWritePos(uint8 dataByte, uint8 rowNumber, uint8 byteNumber) \
                                                ;


/****************************************
*           API Constants
****************************************/

#define SOC_Store_EEPROM_SIZE            CYDEV_EE_SIZE
#define SOC_Store_SPC_BYTE_WRITE_SIZE    (0x01u)

#define SOC_Store_SECTORS_NUMBER         (CYDEV_EE_SIZE / CYDEV_EEPROM_SECTOR_SIZE)

#define SOC_Store_AHB_REQ_SHIFT          (0x00u)
#define SOC_Store_AHB_REQ                ((uint8)(0x01u << SOC_Store_AHB_REQ_SHIFT))
#define SOC_Store_AHB_ACK_SHIFT          (0x01u)
#define SOC_Store_AHB_ACK_MASK           ((uint8)(0x01u << SOC_Store_AHB_ACK_SHIFT))


/***************************************
* Registers
***************************************/
#define SOC_Store_SPC_EE_SCR_REG                 (*(reg8 *) CYREG_SPC_EE_SCR)
#define SOC_Store_SPC_EE_SCR_PTR                 ( (reg8 *) CYREG_SPC_EE_SCR)



/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/
#define SOC_Store_ByteWrite                  SOC_Store_ByteWritePos
#define SOC_Store_QueryWrite                 SOC_Store_Query

#endif /* CY_EEPROM_SOC_Store_H */

/* [] END OF FILE */
