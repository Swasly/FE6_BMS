/*!
LTC2983.ino: Generated Linduino code from the LTC2983 Demo Software.
This code (plus the other code in this folder) is designed to be used by a Linduino,
but can also be used to understand how to program the LTC2983.

$Revision: 3018 $
$Date: 2014-12-01 15:53:20 -0800 (Mon, 01 Dec 2014) $
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
#include <stdbool.h>
#include "SPI.h"
#include "Wire.h"
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "stdio.h"
#include "math.h"

#include "configuration_constants.h"
#include "support_functions.h"
#include "table_coeffs.h"

// Function prototypes
void configure_memory_table();
void configure_channels();
void configure_global_parameters();


// -------------- Configure the LTC2983 -------------------------------
void setup() {
	initialize_serial();
	initialize_spi();
	
	configure_memory_table();
	
	configure_channels();
	configure_global_parameters();
} 


void configure_memory_table() {
	long start_address;
	long table_length;
	int i;

	// -- Channel 1 custom table --
	table_coeffs ch_1_coefficients[] = {
		{ 15954412, 0 },	// -- -50.22, 0.0
		{ 16282419, 101478 },	// -- -30.2, 99.1
		{ 16690381, 138650 },	// -- -5.3, 135.4
		{ 0, 279706 },	// -- 0.0, 273.15
		{ 658637, 369869 },	// -- 40.2, 361.2
		{ 906035, 534630 },	// -- 55.3, 522.1
		{ 1446707, 737587 },	// -- 88.3, 720.3
		{ 2165965, 830669 },	// -- 132.2, 811.2
		{ 3091661, 944640 },	// -- 188.7, 922.5
		{ 7543194, 1024000 } 	// -- 460.4, 1000.0
	};
	start_address = 592;	// Real address = 6*0 + 0x250 = 592
	table_length = 10;	// Real table length = 9 + 1 = 10
	write_custom_table(ch_1_coefficients, start_address, table_length);

}


void configure_channels() {
	byte channel_number;
	long channel_assignment_data;		

	// ----- Channel 1: Assign Custom Thermocouple -----
	channel_assignment_data =
		(long) SENSOR_TYPE__CUSTOM_THERMOCOUPLE |
		(long) TC_COLD_JUNCTION_CH__2 |
		(long) TC_SINGLE_ENDED |
		(long) TC_OPEN_CKT_DETECT__NO |
		(long) TC_OPEN_CKT_DETECT_CURRENT__10UA |
		(long) 0b000000 << TC_CUSTOM_ADDRESS_LSB |		// tc - custom address: 0.
		(long) 0b001001 << TC_CUSTOM_LENGTH_1_LSB;		// tc - custom length-1: 9.
	assign_channel(1, channel_assignment_data);
	// ----- Channel 2: Assign Off-Chip Diode -----
	channel_assignment_data =
		(long) SENSOR_TYPE__OFF_CHIP_DIODE |
		(long) DIODE_SINGLE_ENDED |
		(long) DIODE_NUM_READINGS__3 |
		(long) DIODE_AVERAGING_ON |
		(long) DIODE_CURRENT__20UA_80UA_160UA |
		(long) 0b0100000000110001001001 << DIODE_IDEALITY_FACTOR_LSB;		// diode - ideality factor(eta): 1.00299930572509765625
	assign_channel(2, channel_assignment_data);

}

void configure_global_parameters() {
	// -- Set rejection frequency and temperature unit
	write_single_byte(0xF0, TEMP_UNIT__C | REJECTION__50_60_HZ);
	// -- Set any extra delay between conversions (in this case, 0*100us)
	write_single_byte(0xFF, 0);
}

// -------------- Run the LTC2983 -------------------------------------
void loop() { 
	float temperature_result;
	byte channel_number; 
	
	int channels_to_measure[] = {1, 2};
	int num_measured_channels = sizeof(channels_to_measure)/sizeof(channels_to_measure[0]);

	
	for (int i = 0; i < num_measured_channels; i++) {
		channel_number = channels_to_measure[i];
		convert_channel(channel_number);

		read_temperature_results( int(channel_number));

	}

}