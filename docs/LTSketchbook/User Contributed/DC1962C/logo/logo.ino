/*
Linear Technology DC1962C Demonstration Board
LTC3880, LTC2974, LTC2977: Power Management Solution for Application Processors

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

@endverbatim
 
http://www.linear.com/product/LTC3880
http://www.linear.com/product/LTC2974
http://www.linear.com/product/LTC2977

http://www.linear.com/demo/DC1962C

REVISION HISTORY
$Revision: 3033 $
$Date: 2014-12-05 14:58:30 -0800 (Fri, 05 Dec 2014) $

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

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_I2CBus.h"
#include "LT_SMBusNoPec.h"
#include "LT_SMBusPec.h"
#include "LT_PMBus.h"

#define LTC3880_I2C_ADDRESS 0x30


// Global variables
static uint8_t ltc3880_i2c_address;
static LT_SMBus *smbus = new LT_SMBusPec();
static LT_PMBus *pmbus = new LT_PMBus(smbus);

static uint16_t ch1_values[] =
{
  0x800, 0x8B4, 0x9A6, 0xB32, 0xD77, 0x1011, 0x12CC, 0x16FC, 0x1B6F, 0x2165, 0x29A3, 
  0x2FAD, 0x3423, 0x3786, 0x3A38, 0x3C5D, 0x3DFB, 0x3EFA, 0x3F96, 0x3FDE, 0x3FF6,
  0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000,
  0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000,
  0x4000, 0x4000, 0x4000
};

static uint16_t ch2_values[] =
{
 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800,
 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800,
 0x800, 0x800, 0x800, 0x83B, 0x8CB, 0x9C1, 0xB5F, 0xD78, 0x101F, 0x1375, 0x17EA, 
 0x1DDC, 0x261C, 0x2C12, 0x309B, 0x34C5, 0x379E, 0x3A3D, 0x3C94, 0x3E1C,
 0x3F29, 0x4000
};

//! Initialize Linduino
void setup()
{
	Serial.begin(115200);         //! Initialize the serial port to the PC
	ltc3880_i2c_address = LTC3880_I2C_ADDRESS;
	Serial.print(F("\n******** Observe Logo on Scope in Rolling Mode *******\n"));
}

//! Repeats Linduino loop
void loop()
{
	uint16_t delay_ms = 30;
	float amp = 1.0f;
	uint16_t offset = 0x800;
	uint16_t starting_voltage;
	int16_t num_interp = 4;
	int16_t len = sizeof(ch1_values)/sizeof(uint16_t);
	int16_t i;
	int16_t j;
	int16_t k;
	float percent_there;
	uint16_t ch1_interp, ch2_interp;
	
	while (1)
	{
		for (k = 0; k < 4; k++)
		{
		   delay_ms = 12;
		   amp = 0.25f;
		   offset = (uint16_t) (0x800+(k/4.0*(0x4000-0x800)));

		   starting_voltage = offset;

		   write_voltage(0x30, 0x00, starting_voltage);
		   write_voltage(0x30, 0x01, starting_voltage);
		   delay((uint16_t) (500.0f*delay_ms/60.0f));

		   for (i = 0; i < len - 1; i++)
		   {

			   for (j = 0; j < num_interp; j++)
			   {
				   percent_there = 1.0f*j/num_interp;
				   // interopolate line segment
				   ch1_interp =
					   (uint16_t) (ch1_values[i]*(1.0 - percent_there) + ch1_values[i + 1]*percent_there);
				   ch2_interp =
					   (uint16_t) (ch2_values[i]*(1.0 - percent_there) + ch2_values[i + 1]*percent_there);
				   write_voltage(0x30, 0x00, (uint16_t) ((ch1_interp-0x800)*amp+offset));
				   write_voltage(0x30, 0x01, (uint16_t) ((ch2_interp-0x800)*amp+offset));
				   delay(4);
				   if(k == num_interp) delay((uint16_t)(2.0*delay_ms/4.0f));
			   }
		   }
                        }
		   delay_ms = 30;
		   amp = 1.0f;
		   offset = 0x800;
		   starting_voltage = 0x0800;
		   delay((uint16_t)(2000.0f * delay_ms / 60.0f));

		   write_voltage(0x30, 0x00, starting_voltage);
		   write_voltage(0x30, 0x01, starting_voltage);
		   delay((uint16_t)(2000.0f * delay_ms / 60.0f));

		   for (i = 0; i < len - 1; i++)
		   {
			   for (j = 0; j < num_interp; j++)
			   {
				   percent_there = 1.0f*j/num_interp;
				   // interopolate line segment
				   ch1_interp = (uint16_t) (ch1_values[i]*(1.0 - percent_there) + ch1_values[i + 1]*percent_there);
				   ch2_interp = (uint16_t) (ch2_values[i]*(1.0 - percent_there) + ch2_values[i + 1]*percent_there);
				   write_voltage(0x30, 0x00, (uint16_t) ((ch1_interp - 0x800)*amp + offset));
				   write_voltage(0x30, 0x01, (uint16_t) ((ch2_interp - 0x800)*amp + offset));
				   if (k == num_interp) delay((uint16_t)(2.0*delay_ms/4.0f));
			   }
		   }
		   delay((uint16_t)(2000.0f * delay_ms / 60.0f));
		   for (i = len - 1; i > 0; i--)
		   {
			   for (j = 0; j < num_interp; j++)
			   {
				   percent_there = 1.0f * j / num_interp;
				   // interopolate line segment
				   ch1_interp = (uint16_t)(ch1_values[i] * (1.0 - percent_there) + ch1_values[i - 1] * percent_there);
				   ch2_interp = (uint16_t)(ch2_values[i] * (1.0 - percent_there) + ch2_values[i - 1] * percent_there);
				   ch1_interp = ch2_interp;
				   write_voltage(0x30, 0x00, (uint16_t)((ch1_interp - 0x800) * amp + offset));
				   write_voltage(0x30, 0x01, (uint16_t)((ch2_interp - 0x800) * amp + offset));
				   delay((uint16_t) (2.0*delay_ms / 12));
			   }
		   }
		   delay((uint16_t)(600.0f * delay_ms / 30.0f));
		}
  
}

// write voltage using rail address
static void write_voltage(uint8_t address, uint8_t page, uint16_t vout_command)
{
	smbus->writeByte(address, PAGE, page);
	smbus->writeWord(address, VOUT_COMMAND, vout_command);
}

