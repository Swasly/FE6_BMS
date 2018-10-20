/*!
LTC SMBus Support: API for a shared SMBus layere

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
    Library Header File for LT_SMBUS
*/

#include <Arduino.h>
#include "LT_SMBusNoPec.h"
	
#define USE_BLOCK_TRANSACTION 1
	
LT_SMBusNoPec::LT_SMBusNoPec() : LT_SMBusBase()
{
}

void LT_SMBusNoPec::writeByte(uint8_t address, uint8_t command, uint8_t data)
{	
	if (i2cbus.writeByteData(address, command, data))
		Serial.print(F("Write Byte: fail.\n"));
}

void LT_SMBusNoPec::writeBytes(uint8_t *addresses, uint8_t *commands, uint8_t *data, uint8_t no_addresses)
{
	uint16_t index = 0;

	while(index < no_addresses)
	{
		if (i2cbus.writeBlockData(addresses[index], commands[index], 1, &data[index]))
			Serial.print(F("Write Bytes: fail.\n"));
		index++;
	}
}

uint8_t LT_SMBusNoPec::readByte(uint8_t address, uint8_t command)
{
	uint8_t result;

	if (i2cbus.readByteData(address, command, &result))
		Serial.print(F("Read Byte: fail.\n"));
	return result;
}

void LT_SMBusNoPec::writeWord(uint8_t address, uint8_t command, uint16_t data)
{
	uint8_t buffer[2];
	buffer[0] = (uint8_t) (data & 0xff);
	buffer[1] = (uint8_t) (data >> 8);
	
#if USE_BLOCK_TRANSACTION
	if (i2cbus.writeBlockData(address, command, 2, buffer))
		Serial.print(F("Write Word: fail.\n"));
#else
	uint16_t rdata;
	rdata = (data << 8) | (data >> 8);
	if (i2cbus.writeWordData(address, command, rdata))
		Serial.print(F("Write Word: fail.\n"));
#endif
}

uint16_t LT_SMBusNoPec::readWord(uint8_t address, uint8_t command)
{
	uint8_t input[2];
	input[0] = 0x00;
	input[1] = 0x00;

#if USE_BLOCK_TRANSACTION
	if (i2cbus.readBlockData(address, command, 2, input))
		Serial.print(F("Read Word: fail.\n"));
	return input[1] << 8 | input[0];
#else
	uint16_t rdata;
	if (i2cbus.readWordData(address, command, &rdata))
		Serial.print(F("Read Word: fail.\n"));
	return rdata << 8 | rdata >> 8;
#endif
}

void LT_SMBusNoPec::writeBlock(uint8_t address, uint8_t command,
		uint8_t *block, uint16_t block_size)
{
	uint8_t *buffer = (uint8_t*)malloc(block_size + 1);
	buffer[0] = block_size;
	memcpy(buffer + 1, block, block_size);
	if (i2cbus.writeBlockData(address, command, block_size + 1, buffer))
		Serial.print(F("Write Block: fail.\n"));
	free(buffer);
}

void LT_SMBusNoPec::writeReadBlock(uint8_t address, uint8_t command,
		uint8_t *block_out, uint16_t block_out_size, uint8_t* block_in, uint16_t block_in_size)
{
	uint8_t *buffer = (uint8_t*)malloc(block_out_size + 1);
	buffer[0] = block_out_size;
	memcpy(buffer + 1, block_out, block_out_size);
	
	i2cbus.startGroupProtocol();
	if (i2cbus.writeBlockData(address, command, block_out_size + 1, buffer))
		Serial.print(F("Write/Read Block write fail\n"));
	free(buffer);
	
	i2cbus.endGroupProtocol();
	buffer = (uint8_t*)malloc(block_in_size + 1);
	if (i2cbus.readBlockData(address, block_in_size + 1, buffer))
		Serial.print(F("Write/Read Block: read fail.\n"));
	if (buffer[0] != block_in_size) {
		Serial.print(F("Write/Read Block: fail size.\n"));
		}
	memcpy(block_in, buffer + 1, block_in_size);
	free(buffer);

	/*uint8_t *pos;
	uint8_t block_read_size;
	/*
	i2cbus.start();
	i2cbus.write(address << 1);
	i2cbus.write(command);
	i2cbus.write(block_out_size);
	pos == block_out;
	while (pos < block_out + block_out_size)
		i2cbus.write(*pos++);
	i2cbus.repeatedStart();
	i2cbus.write(address << 1 & 0x01);
	block_read_size = i2cbus.read(0);
	pos = block_in;
	while (pos - block_in < block_in_size && pos < block_in + block_read_size)
		*pos++ = i2cbus.read(0);
	i2cbus.stop();
	
	if (pos != block_in + block_in_size)
		Serial.print(F("Write Read Block: fail read size.\n"));
		*/
}

void LT_SMBusNoPec::readBlock(uint8_t address, uint8_t command,
		uint8_t *block, uint16_t block_size)
{
	uint8_t *buffer = (uint8_t*)malloc(block_size + 1);

	if (i2cbus.readBlockData(address, command, block_size + 1, buffer))
		Serial.print(F("Read Block: fail.\n"));
	if (buffer[0] != block_size) {
		Serial.print(F("Read Block: fail size.\n"));
		}
	memcpy(block, buffer + 1, block_size);
	free(buffer);
}

void LT_SMBusNoPec::sendByte(uint8_t address, uint8_t command)
{
	if (i2cbus.writeByte(address, command))
		Serial.print(F("Send Byte: fail.\n"));
}
