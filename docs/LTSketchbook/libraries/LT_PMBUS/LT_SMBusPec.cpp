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
    Library Header File for LT_SMBUS
*/

#include <Arduino.h>
#include "LT_SMBusPec.h"
	
	const uint8_t	LT_SMBusPec::table_[256] PROGMEM  = { 0, 7, 14, 9, 28, 27, 18, 21, 
											56, 63, 54, 49, 36, 35, 42, 45, 
											112, 119, 126, 121, 108, 107, 98, 101, 
											72, 79, 70, 65, 84, 83, 90, 93, 
											224, 231, 238, 233, 252, 251, 242, 245, 
											216, 223, 214, 209, 196, 195, 202, 205, 
											144, 151, 158, 153, 140, 139, 130, 133, 
											168, 175, 166, 161, 180, 179, 186, 189, 
											199, 192, 201, 206, 219, 220, 213, 210, 
											255, 248, 241, 246, 227, 228, 237, 234, 
											183, 176, 185, 190, 171, 172, 165, 162, 
											143, 136, 129, 134, 147, 148, 157, 154, 
											39, 32, 41, 46, 59, 60, 53, 50, 
											31, 24, 17, 22, 3, 4, 13, 10, 
											87, 80, 89, 94, 75, 76, 69, 66, 
											111, 104, 97, 102, 115, 116, 125, 122, 
											137, 142, 135, 128, 149, 146, 155, 156, 
											177, 182, 191, 184, 173, 170, 163, 164, 
											249, 254, 247, 240, 229, 226, 235, 236, 
											193, 198, 207, 200, 221, 218, 211, 212, 
											105, 110, 103, 96, 117, 114, 123, 124, 
											81, 86, 95, 88, 77, 74, 67, 68, 
											25, 30, 23, 16, 5, 2, 11, 12, 
											33, 38, 47, 40, 61, 58, 51, 52, 
											78, 73, 64, 71, 82, 85, 92, 91, 
											118, 113, 120, 127, 106, 109, 100, 99, 
											62, 57, 48, 55, 34, 37, 44, 43, 
											6, 1, 8, 15, 26, 29, 20, 19, 
											174, 169, 160, 167, 178, 181, 188, 187, 
											150, 145, 152, 159, 138, 141, 132, 131, 
											222, 217, 208, 215, 194, 197, 204, 203, 
											230, 225, 232, 239, 250, 253, 244, 243 };
	
LT_SMBusPec::LT_SMBusPec() : LT_SMBusBase()
{
	poly_ = 0x07;
	crc_polynomial_ = 0x0107;

	// Set up the PEC table.
	constructTable(crc_polynomial_);
}

void LT_SMBusPec::writeByte(uint8_t address, uint8_t command, uint8_t data)
{
	uint8_t buffer[2];
	buffer[0] = data;

	pecClear();
	pecAdd(address << 1);
	pecAdd(command);
	pecAdd(data);
	buffer[1] = pecGet();
	if (i2cbus.writeBlockData(address, command, 2, buffer))
		Serial.print(F("Write Byte With Pec: fail.\n"));
}

void LT_SMBusPec::writeBytes(uint8_t *addresses, uint8_t *commands,
		uint8_t *data, uint8_t no_addresses)
{
	uint8_t buffer[2];
	uint16_t index = 0;

	while(index < no_addresses)
	{
		buffer[0] = data[index];
		pecClear();
		pecAdd(addresses[index] << 1);
		pecAdd(commands[index]);
		pecAdd(data[index]);
		buffer[1] = pecGet();

		if (i2cbus.writeBlockData(addresses[index], commands[index], 2, buffer))
			Serial.print(F("Write Bytes With Pec: fail.\n"));
		index++;
	}
}

uint8_t LT_SMBusPec::readByte(uint8_t address, uint8_t command)
{
	uint8_t input[2];
	input[0] = 0x00;
	input[1] = 0x00;

	pecClear();
	pecAdd(address << 1);
	pecAdd(command);
	pecAdd((address << 1) | 0x01);
	if (i2cbus.readBlockData(address, command, 2, input))
		Serial.print(F("Read Byte With Pec: fail.\n"));

	pecAdd(input[0]);
	if (pecGet() != input[1])
		Serial.print(F("Read Byte With Pec: fail pec\n"));

	return input[0];
}

void LT_SMBusPec::writeWord(uint8_t address, uint8_t command, uint16_t data)
{
	uint8_t buffer[3];
	buffer[0] = (uint8_t) (data & 0xff);
	buffer[1] = (uint8_t) (data >> 8);

	pecClear();
	pecAdd(address << 1);
	pecAdd(command);
	pecAdd(data & 0xff);
	pecAdd(data >> 8);
	buffer[2] = pecGet();
	if (i2cbus.writeBlockData(address, command, 3, buffer))
		Serial.print(F("Write Word With Pec: fail.\n"));
}

uint16_t LT_SMBusPec::readWord(uint8_t address, uint8_t command)
{
	uint8_t input[3];
	input[0] = 0x00;
	input[1] = 0x00;
	input[2] = 0x00;

	pecClear();
	pecAdd(address << 1);
	pecAdd(command);
	pecAdd((address << 1) | 0x01);
	
	if (i2cbus.readBlockData(address, command, 3, input))
		Serial.print(F("Read Word With Pec: fail.\n"));

	pecAdd(input[0]);
	pecAdd(input[1]);
	if (pecGet() != input[2])
		Serial.print(F("Read Word With Pec: fail pec\n"));

	return input[1] << 8 | input[0];
}

void LT_SMBusPec::writeBlock(uint8_t address, uint8_t command,
		uint8_t *block, uint16_t block_size)
{
	uint16_t pos = 0;

	pecClear();
	pecAdd(address << 1);
	pecAdd(command);
	pecAdd(block_size);

	while (pos < block_size)
		pecAdd(block[pos++]);
	uint8_t pec = pecGet();
	
	uint8_t *data_with_pec = (uint8_t*) malloc(block_size + 2);
	data_with_pec[0] = block_size;
	memcpy(data_with_pec + 1, block, block_size);
	data_with_pec[block_size + 1] = pec;

	if (i2cbus.writeBlockData(address, command, block_size + 2, data_with_pec))
		Serial.print(F("Write Block With Pec: fail.\n"));
	free(data_with_pec);
}

void LT_SMBusPec::writeReadBlock(uint8_t address, uint8_t command,
		uint8_t *block_out, uint16_t block_out_size, uint8_t* block_in, uint16_t block_in_size)
{
	uint16_t pos = 0;

	pecClear();
	pecAdd(address << 1);
	pecAdd(command);
	pecAdd(block_out_size);
	while (pos < block_out_size)
		pecAdd(block_out[pos++]);


	uint8_t *buffer = (uint8_t*)malloc(block_out_size + 1);
	buffer[0] = block_out_size;
	memcpy(buffer + 1, block_out, block_out_size);
	
	i2cbus.startGroupProtocol();
	if (i2cbus.writeBlockData(address, command, block_out_size + 1, buffer))
		Serial.print(F("Write/Read Block w/PEC: write fail\n"));
	free(buffer);
	
	

	pecAdd((address << 1) | 0x01);
	
	i2cbus.endGroupProtocol();
	buffer = (uint8_t*)malloc(block_in_size + 2);
	if (i2cbus.readBlockData(address, block_in_size + 2, buffer))
		Serial.print(F("Write/Read Block w/PEC: read fail.\n"));
	if (buffer[0] != block_in_size) {
		Serial.print(F("Write/Read Block w/PEC: fail read size.\n"));
		}
	memcpy(block_in, buffer + 1, block_in_size);
	
	for (pos = 0; pos<buffer[0] + 1; pos++)
		pecAdd(buffer[pos]);
	if (pecGet() != buffer[buffer[0]+1])
		Serial.print(F("Write/Read Block w/Pec: fail pec\n"));
		
	free(buffer);





/*	uint8_t *pos;
	uint8_t block_read_size;
	uint8_t pec;
/*	
	pecClear();
	
	i2cbus.start();
	i2cbus.write(address << 1);
	pecAdd(address << 1);
	i2cbus.write(command);
	pecAdd(command);
	i2cbus.write(block_out_size);
	pecAdd(block_out_size);
	pos == block_out;
	while (pos < block_out + block_out_size)
	{
		i2cbus.write(*pos);
		pecAdd(*pos++);
	}
	i2cbus.repeatedStart();
	i2cbus.write(address << 1 & 0x01);
	pecAdd(address << 1 & 0x01);
	block_read_size = i2cbus.read(0);
	pecAdd(block_read_size);
	pos = block_in;
	while (pos - block_in < block_in_size && pos < block_in + block_read_size)
	{
		*pos = i2cbus.read(0);
		pecAdd(*pos++);
	}
	pec = i2cbus.read(0);
	i2cbus.stop();
	
	if (pos != block_in + block_in_size)
		Serial.print(F("Write Read Block: fail read size.\n"));
		
	if (pecGet() != pec)
		Serial.print(F("Write Read Block: fail pec.\n"));
		*/
}

void LT_SMBusPec::readBlock(uint8_t address, uint8_t command,
		uint8_t *block, uint16_t block_size)
{
	uint16_t pos;
	uint8_t *buffer = (uint8_t*)malloc(block_size + 2);

	pecClear();
	pecAdd(address << 1);
	pecAdd(command);
	pecAdd((address << 1) | 0x01);

	if (i2cbus.readBlockData(address, command, block_size + 2, buffer))

	if (buffer[0] != block_size)
		Serial.print(F("Read Block with PEC: fail size.\n"));

	memcpy(block, buffer + 1, block_size);

	for (pos = 0; pos<buffer[0] + 1; pos++)
		pecAdd(buffer[pos]);
	if (pecGet() != buffer[buffer[0]+1])
		Serial.print(F("Read Block With Pec: fail pec\n"));
	free(buffer);
}

void LT_SMBusPec::sendByte(uint8_t address, uint8_t command)
{
	uint8_t pec;

	pecClear();
	pecAdd(address << 1);
	pecAdd(command);
	pec = pecGet();
	
	if (i2cbus.writeBlockData(address, command, 1, &pec))
		Serial.print(F("Send Byte With Pec: fail.\n"));
}



/*
 * Construct a table for PEC calculation
 */
void LT_SMBusPec::constructTable(uint16_t user_polynomial)
{
	uint16_t i;
	uint16_t j;
	uint8_t temp;
	uint8_t a;
	uint8_t poly = (uint8_t)(user_polynomial & 0x0FF);

//	if ((user_polynomial & 0xFFFFFF01) != 0x00000101)
//		throw new ApplicationException("Invalid userPolynomial.");

	crc_polynomial_ = user_polynomial;
	// for (i = 0; i < 256; i++)
	// {
		// temp = 0;
		// a = (uint8_t)i;
		// for (j = 0; j < 8; j++)
		// {
			// if (((temp ^ a) & 0x80) != 0)
			// {
				// temp = (uint8_t)((temp << 1) ^ poly);
			// }
			// else
			// {
				// temp <<= 1;
			// }
			// a <<= 1;
		// }
		// table_[i] = temp;
	// }
}

/*
 * Helper function for calculating PEC
 */
uint8_t LT_SMBusPec::doCalculate(uint8_t data, uint8_t begining_value)
{
	//return table_[(0xFF & (begining_value ^ data))];
	return (uint8_t) pgm_read_byte_near(table_ + (0xFF & (begining_value ^ data)));
}

/*
 * Helper function for calculating PEC
 */
uint8_t LT_SMBusPec::calculate(uint8_t *data, uint8_t begining_value, uint8_t start_index, uint8_t length)
{
	uint8_t i;
	uint8_t ret_value = begining_value;

	for (i = start_index; i < (start_index + length); i++)
	{
		ret_value = doCalculate(data[i], ret_value);
	}

	return ret_value;
}

/*
 * Clear the PEC value and get ready to calculate.
 */
void LT_SMBusPec::pecClear(void)
{
	running_pec_ = 0;
}

/*
 * Add a value to the PEC
 */
void LT_SMBusPec::pecAdd(uint8_t byte_value)
{
	uint8_t i;
	running_pec_ = running_pec_ ^ byte_value;

	for(i=0; i<8; i++)
	{
		if ((running_pec_ & 0x80) > 0x00)
		{
			running_pec_ = (running_pec_ << 1) ^ poly_;
		}
		else
		{
			running_pec_ = running_pec_ << 1;
		}

	}
}

/*
 * Get the final PEC value
 */
uint8_t LT_SMBusPec::pecGet(void)
{
	return running_pec_;
}
