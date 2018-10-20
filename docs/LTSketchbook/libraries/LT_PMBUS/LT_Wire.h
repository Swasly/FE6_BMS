/*!
  TwoWire.h - TWI/I2C library for Arduino & Wiring
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
	Library Header File for LT_Wire: Routines to communicate with ATmega328P's hardware I2C port.
*/

#ifndef TwoWire_h
#define TwoWire_h

#include <inttypes.h>


class TwoWire
{
  private:
	static uint8_t* commBuffer;	//!< local pointer to application RX/TX buffer
    static uint16_t commBufferIndex;	//!< current index for async ISR use
    static uint16_t commBufferLength;	//!< amount of data to RX/TX
	static uint16_t commBufferTxMax;	//!< expected max limit to transfer

    static uint8_t txAddress;	//!< address to transfer to


    static uint8_t transmitting;	//!< state marking whether still transmitting a message

  public:
    TwoWire();
	
	//! Initiate Prep
	//! @return void
    void begin();
	
	//! Initiate Prep
	//! @return void
    void begin(uint8_t);
	
	//! Initiate Prep
	//! @return void
    void begin(int);
	
	//! Prepare to send I2C communication
	//! @return void
    void beginTransmission(uint8_t address	//!< 7-bit address to communicate to
								);
	
	//! Prepare to send I2C communication
	//! @return void	
    void beginTransmission(int address	//!< 7-bit address to communicate to
							);
							
	//! Sets maximum size for transmission buffer
	//! @return void
	void expectToWrite(uint16_t expectedTxLength	//!< expected length of transmission
							);
	
	//! Performs the I2C communication.
	//! @return 0 if success, 2 if address nack, 3 if data nack, 4 if other error
    uint8_t endTransmission(void);
	
	//! Performs the I2C communication.
	//! @return 0 if success, 2 if address nack, 3 if data nack, 4 if other error
    uint8_t endTransmission(uint8_t sendStop	//!< whether to STOP or anticipate a repeated START
							);
							
	//! Read from a slave I2C device.
	//! @return number of bytes read.  If different from quantity, something bad happened.
    uint8_t requestFrom(uint8_t address, 	//!< 7-bit I2C address
							 uint8_t* acceptBuffer, 	//!< buffer pointer to fill
							 uint16_t quantity	//!< anticipated length of read
							);
							
	//! Read from a slave I2C device.
	//! @return number of bytes read.  If different from quantity, something bad happened.
    uint8_t requestFrom(uint8_t address, 	//!< 7-bit I2C address
							 uint8_t* acceptBuffer, 	//!< buffer pointer to fill
							 uint16_t quantity,	//!< anticipated length of read
							 uint8_t sendStop	//!< whether to STOP or anticipate a repeated START
							);
	
	//! Read from a slave I2C device.
	//! @return number of bytes read.  If different from quantity, something bad happened.
    uint8_t requestFrom(int address,  	//!< 7-bit I2C address
							 uint8_t* acceptBuffer,	//!< buffer pointer to fill
							 int quantity	//!< anticipated length of read
							);
	
	//! Read from a slave I2C device.
	//! @return number of bytes read.  If different from quantity, something bad happened.
    uint8_t requestFrom(int address,  	//!< 7-bit I2C address
							 uint8_t* acceptBuffer, 	//!< buffer pointer to fill
							 int quantity, 	//!< anticipated length of read
							 int sendStop	//!< whether to STOP or anticipate a repeated START
							);
	
	//! Write byte to the transmission buffer
	//! 0 if not written (no room), 1 if written
    size_t write(uint8_t data	//!< byte to add to buffer
							);
							
	//! Write multiple bytes to transmission buffer
	//! always returns quantity back
    size_t write(const uint8_t *data, 	//!< pointer to byte array to add to buffer
							 size_t quantity	//!< number of bytes to add to buffer
							);

  
    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
};

extern TwoWire LT_Wire;

#endif

