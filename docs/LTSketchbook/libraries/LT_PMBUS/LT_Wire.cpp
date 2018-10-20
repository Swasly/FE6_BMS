/*
  TwoWire.cpp - TWI/I2C library for Wiring & Arduino
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 
  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
  
  Modified 2014 to conserve memory for master mode and PMBus compliance
*/

/*!	@file
	@ingroup LT_I2CBus
	Library File for LT_Wire: Routines to communicate with ATmega328P's hardware I2C port.
*/
extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
  #include "LT_twi.h"
}

#include "LT_Wire.h"
#include "Arduino.h" //for serial debug

// Initialize Class Variables //////////////////////////////////////////////////

uint8_t* TwoWire::commBuffer = 0;
uint16_t TwoWire::commBufferIndex = 0;
uint16_t TwoWire::commBufferLength = 0;
uint16_t TwoWire::commBufferTxMax = 0;

uint8_t TwoWire::txAddress = 0;

uint8_t TwoWire::transmitting = 0;

// Constructors ////////////////////////////////////////////////////////////////

TwoWire::TwoWire()
{
}

// Public Methods //////////////////////////////////////////////////////////////

void TwoWire::begin(void)
{
  commBufferIndex = 0;
  commBufferLength = 0;


  twi_init();
}


uint8_t TwoWire::requestFrom(uint8_t address, uint8_t* acceptBuffer, uint16_t quantity, uint8_t sendStop)
{
	
  // perform blocking read into buffer
  uint16_t read = twi_readFrom(address, acceptBuffer, quantity, sendStop);
  // set rx buffer iterator vars
  commBufferIndex = 0;
  commBufferLength = read;

  return read;
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t* acceptBuffer, uint16_t quantity)
{
  return requestFrom((uint8_t)address, (uint8_t*) acceptBuffer, (uint16_t)quantity, (uint8_t)true);
}

uint8_t TwoWire::requestFrom(int address, uint8_t* acceptBuffer, int quantity)
{
  return requestFrom((uint8_t)address, (uint8_t*) acceptBuffer, (uint16_t)quantity, (uint8_t)true);
}

uint8_t TwoWire::requestFrom(int address, uint8_t* acceptBuffer, int quantity, int sendStop)
{
  return requestFrom((uint8_t)address, (uint8_t*) acceptBuffer, (uint16_t)quantity, (uint8_t)sendStop);
}

void TwoWire::beginTransmission(uint8_t address)
{
  // indicate that we are transmitting
  transmitting = 1;
  // set address of targeted slave
  txAddress = address;
  // reset tx buffer iterator vars
  commBufferIndex = 0;
  commBufferLength = 0;
}

void TwoWire::beginTransmission(int address)
{
  beginTransmission((uint8_t)address);
}


void TwoWire::expectToWrite(uint16_t expectedTxLength)
{
  commBuffer = (uint8_t *) malloc(sizeof(uint8_t) * expectedTxLength);
  commBufferTxMax = expectedTxLength;
}


//
//	Originally, 'endTransmission' was an f(void) function.
//	It has been modified to take one parameter indicating
//	whether or not a STOP should be performed on the bus.
//	Calling endTransmission(false) allows a sketch to 
//	perform a repeated start. 
//
//	WARNING: Nothing in the library keeps track of whether
//	the bus tenure has been properly ended with a STOP. It
//	is very possible to leave the bus in a hung state if
//	no call to endTransmission(true) is made. Some I2C
//	devices will behave oddly if they do not see a STOP.
//
uint8_t TwoWire::endTransmission(uint8_t sendStop)
{
  // transmit buffer (blocking)
  int8_t ret = twi_writeTo(txAddress, commBuffer, commBufferLength, 1, sendStop);
  
  free(commBuffer);
  commBuffer = 0;
  commBufferTxMax = 0;
  // reset tx buffer iterator vars
  commBufferIndex = 0;
  commBufferLength = 0;
  // indicate that we are done transmitting
  transmitting = 0;
  return ret;
}

//	This provides backwards compatibility with the original
//	definition, and expected behaviour, of endTransmission
//
uint8_t TwoWire::endTransmission(void)
{
  return endTransmission(true);
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t TwoWire::write(uint8_t data)
{
  if(!commBuffer)
  {
	expectToWrite((uint16_t) 10);
  }

  if(transmitting){
  // in master transmitter mode
    // don't bother if buffer is full
    if(commBufferLength >= commBufferTxMax){
      return 0;
    }
    // put byte in tx buffer
    commBuffer[commBufferIndex] = data;
    ++commBufferIndex;
    // update amount in buffer   
    commBufferLength = commBufferIndex;
  }else{
  // in slave send mode
    // reply to master
    //twi_transmit(&data, 1);
  }
  return 1;
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t TwoWire::write(const uint8_t *data, size_t quantity)
{
  if(transmitting){
  // in master transmitter mode
    for(size_t i = 0; i < quantity; ++i){
      write(data[i]);
    }
  }else{
  // in slave send mode
    // reply to master
    //twi_transmit(data, quantity);
  }
  return quantity;
}



// Preinstantiate Objects //////////////////////////////////////////////////////

TwoWire LT_Wire = TwoWire();

