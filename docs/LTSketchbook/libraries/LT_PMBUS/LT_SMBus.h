/*!
LTC SMBus Support: API for a shared SMBus layer

@verbatim

This API is shared with Linduino and RTOS code. End users should code to this
API to enable use of the PMBus code without modifications.

@endverbatim

REVISION HISTORY
$Revision: 3240 $
$Date: 2015-03-09 16:13:55 -0700 (Mon, 09 Mar 2015) $

Copyright (c) 2014, Linear Technology Corp.(LTC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.
*/

/*! @file
    @ingroup LT_I2CBus
    Library Header File for LT_SMBus
*/

#ifndef LT_SMBus_H_
#define LT_SMBus_H_

#include <stdint.h>

class LT_SMBus 
{

public:
	
	//! Perform ARA
	//! @return address
	virtual uint8_t readAlert(void) = 0;
	
	//! SMBus write byte command
	//! @return void
	virtual void writeByte(uint8_t address, 	//!< Slave address
							uint8_t command, 	//!< Command byte
							uint8_t data		//!< Data to send
							) = 0;

	//! SMBus write byte command for a list of addresses
	//! @return void
	virtual void writeBytes(uint8_t *addresses, 		//!< Slave Addresses
							uint8_t *commands, 			//!< Command bytes
							uint8_t *data, 				//!< Data to send
							uint8_t no_addresses
							) = 0;

	//! SMBus read byte command
	//! @return byte
	virtual uint8_t readByte(uint8_t address, 		//!< Slave Address
							uint8_t command			//!< Command byte
						) = 0;

	//! SMBus write word command
	//! @return void
	virtual void writeWord(uint8_t address, 	//!< Slave Address
							uint8_t command, 	//!< Command byte
							uint16_t data		//!< Data to send
							) = 0;

	//! SMBus read word command
	//! @return word
	virtual uint16_t readWord(uint8_t address, 		//!< Slave Address
								uint8_t command		//!< Command byte
								) = 0;

	//! SMBus write block command
	//! @return void
	virtual void writeBlock(uint8_t address, 		//!< Slave Address
							uint8_t command, 		//!< Command byte
							uint8_t *block, 		//!< Data to send
							uint16_t block_size
							) = 0;

	//! SMBus write then read block command
	//! @return void
	virtual void writeReadBlock(uint8_t address, 		//!< Slave Address
								uint8_t command, 		//!< Command byte
								uint8_t *block_out, 	//!< Data to send
								uint16_t block_out_size, //!< Size of data to send
								uint8_t* block_in, 		//!< Memory to receive data
								uint16_t block_in_size	//!< Size of receive data memory
								) = 0;

	//! SMBus read block command
	//! @return void
	virtual void readBlock(uint8_t address, 		//!< Slave Address
							uint8_t command, 		//!< Command byte
							uint8_t *block, 		//!< Memory to receive data
							uint16_t block_size		//!< Size of receive data memory
							) = 0;

	//! SMBus send byte command
	//! @return void
	virtual void sendByte(uint8_t address, 		//!< Slave Address
							uint8_t command		//!< Command byte
							) = 0;

	//! Read with the address and command in loop until ack, then issue stop
	//! @return void
	virtual void waitForAck(uint8_t address, 		//!< Slave Address
							uint8_t command			//!< Command byte
							) = 0;

	//! SMBus bus probe
	//! @return array of addresses (caller must delete return memory)
	virtual uint8_t* probe(uint8_t address		//!< Command byte
							) = 0;
							
};

#endif /* LT_SMBus_H_ */