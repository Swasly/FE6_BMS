/*******************************************************************************
* File Name: MUX_current.c
* Version 1.80
*
*  Description:
*    This file contains all functions required for the analog multiplexer
*    AMux User Module.
*
*   Note:
*
*******************************************************************************
* Copyright 2008-2010, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#include "MUX_current.h"

static uint8 MUX_current_lastChannel = MUX_current_NULL_CHANNEL;


/*******************************************************************************
* Function Name: MUX_current_Start
********************************************************************************
* Summary:
*  Disconnect all channels.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void MUX_current_Start(void) 
{
    uint8 chan;

    for(chan = 0u; chan < MUX_current_CHANNELS ; chan++)
    {
#if (MUX_current_MUXTYPE == MUX_current_MUX_SINGLE)
        MUX_current_Unset(chan);
#else
        MUX_current_CYAMUXSIDE_A_Unset(chan);
        MUX_current_CYAMUXSIDE_B_Unset(chan);
#endif
    }

    MUX_current_lastChannel = MUX_current_NULL_CHANNEL;
}


#if (!MUX_current_ATMOSTONE)
/*******************************************************************************
* Function Name: MUX_current_Select
********************************************************************************
* Summary:
*  This functions first disconnects all channels then connects the given
*  channel.
*
* Parameters:
*  channel:  The channel to connect to the common terminal.
*
* Return:
*  void
*
*******************************************************************************/
void MUX_current_Select(uint8 channel) 
{
    MUX_current_DisconnectAll();        /* Disconnect all previous connections */
    MUX_current_Connect(channel);       /* Make the given selection */
    MUX_current_lastChannel = channel;  /* Update last channel */
}
#endif


/*******************************************************************************
* Function Name: MUX_current_FastSelect
********************************************************************************
* Summary:
*  This function first disconnects the last connection made with FastSelect or
*  Select, then connects the given channel. The FastSelect function is similar
*  to the Select function, except it is faster since it only disconnects the
*  last channel selected rather than all channels.
*
* Parameters:
*  channel:  The channel to connect to the common terminal.
*
* Return:
*  void
*
*******************************************************************************/
void MUX_current_FastSelect(uint8 channel) 
{
    /* Disconnect the last valid channel */
    if( MUX_current_lastChannel != MUX_current_NULL_CHANNEL)
    {
        MUX_current_Disconnect(MUX_current_lastChannel);
    }

    /* Make the new channel connection */
#if (MUX_current_MUXTYPE == MUX_current_MUX_SINGLE)
    MUX_current_Set(channel);
#else
    MUX_current_CYAMUXSIDE_A_Set(channel);
    MUX_current_CYAMUXSIDE_B_Set(channel);
#endif


    MUX_current_lastChannel = channel;   /* Update last channel */
}


#if (MUX_current_MUXTYPE == MUX_current_MUX_DIFF)
#if (!MUX_current_ATMOSTONE)
/*******************************************************************************
* Function Name: MUX_current_Connect
********************************************************************************
* Summary:
*  This function connects the given channel without affecting other connections.
*
* Parameters:
*  channel:  The channel to connect to the common terminal.
*
* Return:
*  void
*
*******************************************************************************/
void MUX_current_Connect(uint8 channel) 
{
    MUX_current_CYAMUXSIDE_A_Set(channel);
    MUX_current_CYAMUXSIDE_B_Set(channel);
}
#endif

/*******************************************************************************
* Function Name: MUX_current_Disconnect
********************************************************************************
* Summary:
*  This function disconnects the given channel from the common or output
*  terminal without affecting other connections.
*
* Parameters:
*  channel:  The channel to disconnect from the common terminal.
*
* Return:
*  void
*
*******************************************************************************/
void MUX_current_Disconnect(uint8 channel) 
{
    MUX_current_CYAMUXSIDE_A_Unset(channel);
    MUX_current_CYAMUXSIDE_B_Unset(channel);
}
#endif

#if (MUX_current_ATMOSTONE)
/*******************************************************************************
* Function Name: MUX_current_DisconnectAll
********************************************************************************
* Summary:
*  This function disconnects all channels.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void MUX_current_DisconnectAll(void) 
{
    if(MUX_current_lastChannel != MUX_current_NULL_CHANNEL) 
    {
        MUX_current_Disconnect(MUX_current_lastChannel);
        MUX_current_lastChannel = MUX_current_NULL_CHANNEL;
    }
}
#endif

/* [] END OF FILE */
