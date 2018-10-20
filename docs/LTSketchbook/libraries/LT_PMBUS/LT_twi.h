/*!
  twi.h - TWI/I2C library for Wiring & Arduino
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
	Library Header File for LT_twi: Routines to communicate with ATmega328P's hardware I2C port.
*/

#ifndef twi_h
#define twi_h

  #include <inttypes.h>

  //#define ATMEGA8

  #ifndef TWI_FREQ
  #define TWI_FREQ 100000L
  #endif


  #define TWI_READY 0	//!< master is not performing a communication
  #define TWI_MRX   1	//!< master is attempting to read from slave
  #define TWI_MTX   2	//!< master is attempting to write to slave

  
  //! readys twi pins and sets twi bitrate
  //! @return void
  void twi_init(void);

  //! attempts to become twi bus master and read a series of bytes from a device on the bus
  //! @return number of bytes read
  uint16_t twi_readFrom(uint8_t address, 	//! 7bit i2c device address
						 uint8_t* data, 	//! pointer to byte array
						 uint16_t length, 	//! number of bytes to read into array
						 uint8_t sendStop	//! Boolean indicating whether to send a stop at the end
						);
						
  //! attempts to become twi bus master and write a series of bytes to a device on the bus
  //! @return 0 if success, 2 if address nack, 3 if data nack, 4 if other error
  uint8_t twi_writeTo(uint8_t address, 	//! 7bit i2c device address
						 uint8_t* data, 	//! pointer to byte array
						 uint8_t length, 	//! number of bytes in array
						 uint8_t wait, 	//! boolean indicating to wait for write or not
						 uint8_t sendStop	//! boolean indicating whether or not to send a stop at the end
						);

  
  //! sends byte or readys receive line
  //! @return void
  void twi_reply(uint8_t ack	//! byte indicating to ack or to nack
						);
						
  //! relinquishes bus master status
  //! @return void
  void twi_stop(void);
  
  //! releases bus control
  //! @return void
  void twi_releaseBus(void);

#endif

