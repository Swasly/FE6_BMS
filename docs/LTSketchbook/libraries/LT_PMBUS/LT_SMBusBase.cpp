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

//! @defgroup DoxygenExampleGroupName Doxygen Documentation Example

/*! @file
    @ingroup LT_I2CBus
    Library Header File for LT_SMBusNoPec
*/

#include "LT_SMBusBase.h"

#define FOUND_SIZE 0x79

bool LT_SMBusBase::open_ = false;
uint8_t LT_SMBusBase::found_address_[FOUND_SIZE + 1];
		
LT_SMBusBase::LT_SMBusBase()
{
	
	if (open_ == false)
	{
		i2cbus.quikevalI2CInit();          //! Initializes Linduino I2C port.
		i2cbus.quikevalI2CConnect();       //! Connects I2C port to the QuikEval connector

		open_ = true;
	}
}


uint8_t LT_SMBusBase::readAlert(void)
{
	uint8_t address;

	if (i2cbus.readByte(0x0C, &address))
		Serial.print(F("Read Alert: fail.\n"));

	return address;
}

void LT_SMBusBase::waitForAck(uint8_t address, uint8_t command)
{

	while (1)
	{

		if (2 != i2cbus.writeByte(address, command))
			break;

	}
	
}


uint8_t* LT_SMBusBase::probe(uint8_t command)
{
	uint8_t 	address;
	uint8_t 	buffer[1];
	uint8_t		input[1];
	uint8_t		found = 0;
	buffer[0] = command;

	for (address = 0x10; address < 0x7F; address++)
	{
		if (address == 0x0C)
			continue;

			if (0==i2cbus.writeByte(address, command))
			{

					if (found < FOUND_SIZE)
						found_address_[found++] = address;

			}


	 }

	found_address_[found] = 0;

	return found_address_;
}
