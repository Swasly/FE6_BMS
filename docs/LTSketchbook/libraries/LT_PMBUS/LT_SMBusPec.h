/*!
LTC SMBus Support: Implementation for a shared SMBus layer

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
    Library Header File for LT_SMBusPec
*/

#ifndef LT_SMBusPec_H_
#define LT_SMBusPec_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "LT_I2CBus.h"
#include "LT_SMBusBase.h"

class LT_SMBusPec : public LT_SMBusBase
{
protected:

	uint8_t				running_pec_;		//!< Temporary pec calc value
	unsigned char 		poly_;				//!< The poly used in the calc
	uint16_t 			crc_polynomial_;	//!< The crc poly used in the calc
	//uint8_t 			table_[256];		//!< A table used by pec to speed up
	static const uint8_t	table_[256] PROGMEM;	//!< A table used by pec to speed up

	
	//! Clear the pec value so it can start a new calculation
	//! @return void
	void pecClear(void);
	
	//! Add a byte to the pec calculation
	//! @return void
	void pecAdd(uint8_t byte_value);
	
	//! Get the current pec result
	//! @return the pec
	uint8_t pecGet(void);
	
	//! Initialize the table used to speed up pec calculations
	//! @return void
	void constructTable(uint16_t user_polynomial	//!< The controlling polynomial
						);
	
	//! Helper function for pec
	//! @return value
	uint8_t doCalculate(uint8_t data, 
						uint8_t begining_value	//!< The initial value
						);
	
	//! Helper function for pec
	//! @return value
	uint8_t calculate(uint8_t *data, 			//!< Data to be pec'ed
						uint8_t begining_value, //!< Starting value for pec
						uint8_t start_index, 	//!< Starting index
						uint8_t length			//!< Length of data
						);

public:

	LT_SMBusPec();

	//! SMBus write byte command
	//! @return void
	void writeByte(uint8_t address, 	//!< Slave address
							uint8_t command, 	//!< Command byte
							uint8_t data		//!< Data to send
							);

	//! SMBus write byte command for a list of addresses
	//! @return void
	void writeBytes(uint8_t *addresses, 		//!< Slave Addresses
							uint8_t *commands, 			//!< Command bytes
							uint8_t *data, 				//!< Data to send
							uint8_t no_addresses
							);

	//! SMBus read byte command
	//! @return byte
	uint8_t readByte(uint8_t address, 		//!< Slave Address
							uint8_t command			//!< Command byte
						);

	//! SMBus write word command
	//! @return void
	void writeWord(uint8_t address, 	//!< Slave Address
							uint8_t command, 	//!< Command byte
							uint16_t data		//!< Data to send
							);

	//! SMBus read word command
	//! @return word
	uint16_t readWord(uint8_t address, 		//!< Slave Address
								uint8_t command		//!< Command byte
								);

	//! SMBus write block command
	//! @return void
	void writeBlock(uint8_t address, 		//!< Slave Address
							uint8_t command, 		//!< Command byte
							uint8_t *block, 		//!< Data to send
							uint16_t block_size
							);

	//! SMBus write then read block command
	//! @return void
	void writeReadBlock(uint8_t address, 		//!< Slave Address
								uint8_t command, 		//!< Command byte
								uint8_t *block_out, 	//!< Data to send
								uint16_t block_out_size, //!< Size of data to send
								uint8_t* block_in, 		//!< Memory to receive data
								uint16_t block_in_size	//!< Size of receive data memory
								);

	//! SMBus read block command
	//! @return void
	void readBlock(uint8_t address, 		//!< Slave Address
							uint8_t command, 		//!< Command byte
							uint8_t *block, 		//!< Memory to receive data
							uint16_t block_size		//!< Size of receive data memory
							);

	//! SMBus send byte command
	//! @return void
	void sendByte(uint8_t address, 		//!< Slave Address
							uint8_t command		//!< Command byte
							);
};

#endif /* LT_SMBusPec_H_ */
