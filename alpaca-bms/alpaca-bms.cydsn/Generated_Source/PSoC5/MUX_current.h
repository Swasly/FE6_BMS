/*******************************************************************************
* File Name: MUX_current.h
* Version 1.80
*
*  Description:
*    This file contains the constants and function prototypes for the Analog
*    Multiplexer User Module AMux.
*
*   Note:
*
********************************************************************************
* Copyright 2008-2010, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_AMUX_MUX_current_H)
#define CY_AMUX_MUX_current_H

#include "cyfitter.h"
#include "cyfitter_cfg.h"

#if ((CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC3) || \
         (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC4) || \
         (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC5))    
    #include "cytypes.h"
#else
    #include "syslib/cy_syslib.h"
#endif /* ((CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC3) */


/***************************************
*        Function Prototypes
***************************************/

void MUX_current_Start(void) ;
#define MUX_current_Init() MUX_current_Start()
void MUX_current_FastSelect(uint8 channel) ;
/* The Stop, Select, Connect, Disconnect and DisconnectAll functions are declared elsewhere */
/* void MUX_current_Stop(void); */
/* void MUX_current_Select(uint8 channel); */
/* void MUX_current_Connect(uint8 channel); */
/* void MUX_current_Disconnect(uint8 channel); */
/* void MUX_current_DisconnectAll(void) */


/***************************************
*         Parameter Constants
***************************************/

#define MUX_current_CHANNELS  2u
#define MUX_current_MUXTYPE   1
#define MUX_current_ATMOSTONE 0

/***************************************
*             API Constants
***************************************/

#define MUX_current_NULL_CHANNEL 0xFFu
#define MUX_current_MUX_SINGLE   1
#define MUX_current_MUX_DIFF     2


/***************************************
*        Conditional Functions
***************************************/

#if MUX_current_MUXTYPE == MUX_current_MUX_SINGLE
# if !MUX_current_ATMOSTONE
#  define MUX_current_Connect(channel) MUX_current_Set(channel)
# endif
# define MUX_current_Disconnect(channel) MUX_current_Unset(channel)
#else
# if !MUX_current_ATMOSTONE
void MUX_current_Connect(uint8 channel) ;
# endif
void MUX_current_Disconnect(uint8 channel) ;
#endif

#if MUX_current_ATMOSTONE
# define MUX_current_Stop() MUX_current_DisconnectAll()
# define MUX_current_Select(channel) MUX_current_FastSelect(channel)
void MUX_current_DisconnectAll(void) ;
#else
# define MUX_current_Stop() MUX_current_Start()
void MUX_current_Select(uint8 channel) ;
# define MUX_current_DisconnectAll() MUX_current_Start()
#endif

#endif /* CY_AMUX_MUX_current_H */


/* [] END OF FILE */
