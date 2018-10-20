/*!
support_functions.cpp:
This file contains all the support functions used in the main program.

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
#include "Linduino.h"
#include "LT_SPI.h"
#include <SPI.h>
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "stdio.h"
#include "math.h"
#include "configuration_constants.h"
#include "table_coeffs.h"
#include "support_functions.h"

// -----------------------------------------------------------------
//                       Initializing
// -----------------------------------------------------------------
void initialize_serial() {
  Serial.begin(115200);		// Initialize the serial port to the PC
  Serial.print(F("LTC2983\n"));
}

void initialize_spi() {
	quikeval_SPI_init();		// Configure the spi port for 500khz SCK
	// spi_enable(16);			// Configure the spi port for 4Mhz SCK 8 gives 4MHz

	quikeval_SPI_connect();   // Connect SPI to main data port
	quikeval_I2C_init(); 		// Configure the EEPROM I2C port for 100khz  
  
}

// -----------------------------------------------------------------
//                  Memory write functions
// -----------------------------------------------------------------
void assign_channel(int channel_number, long channel_assignment_data) {
	int bytes_per_coeff = 4;
  long start_address = 0x200+4*(channel_number-1);
	
	initialize_memory_write(start_address);
	write_coefficient(channel_assignment_data, bytes_per_coeff);
	finish_memory_write();
}

void write_custom_table(struct table_coeffs coefficients[64], long start_address, long table_length) {
	int bytes_per_coeff = 3;
	
	initialize_memory_write(start_address);	
	for (int i=0; i< table_length; i++) {
	  // write_table multiplies i by 6 and adds 250 to start address
		write_coefficient(coefficients[i].measurement, bytes_per_coeff);
		write_coefficient(coefficients[i].temperature, bytes_per_coeff);
        } 
	finish_memory_write();
} 

void write_custom_steinhart_hart(long steinhart_hart_coeffs[6], long start_address) {
	int bytes_per_coeff = 4;
	
	initialize_memory_write(start_address);
	for (int i = 0; i < 6; i++) {
		write_coefficient(steinhart_hart_coeffs[i], bytes_per_coeff);		
	}
	finish_memory_write();	
}

void write_single_byte(long start_address, int single_byte) {
	initialize_memory_write(start_address);
	spi_write(single_byte);
	finish_memory_write();
}

void initialize_memory_write(long start_address) {
	output_low(LTC2983_CS);
	spi_write(0x02); // instruction Byte Write
	spi_write(highByte(start_address)); // Address MSB Byte
	spi_write(lowByte(start_address)); // Address LSB Byte
}

void write_coefficient(long coeff, int bytes_per_coeff) {
	int data_byte;
	for (int i = bytes_per_coeff - 1; i >= 0; i--) {
		  data_byte = coeff >> (i*8);
			spi_write(data_byte);
	}  
}

void finish_memory_write() {
	output_high(LTC2983_CS);
}

// -----------------------------------------------------------------
//                  Memory read functions
// -----------------------------------------------------------------
void initialize_memory_read(long start_address) {
  output_low(LTC2983_CS);
  spi_write(0x03); // instruction Byte read
  spi_write(highByte(start_address)); // Address MSB Byte
  spi_write(lowByte(start_address)); // Address LSB Byte
}

void finish_memory_read() {
	output_high(LTC2983_CS);
}

// -----------------------------------------------------------------
// 					Channel conversion
// -----------------------------------------------------------------
void convert_channel(byte channel_number)
{
  // initiate a new conversion
	initialize_memory_write(0);
  spi_write(0b10000000 | channel_number); // start a conversion
  finish_memory_write();
  
  while (conversion_done()==0); // wait for conversion to complete
}

boolean conversion_done()
{
	initialize_memory_read(0);
  byte data=spi_read(0x00);
  finish_memory_read();
  return(data & 0b01000000);
}

// -----------------------------------------------------------------
// 					Getting temperature results
// -----------------------------------------------------------------

float read_temperature_results(int channel_number) {
	unsigned char raw_results[4];
	get_raw_results(READ_CH_BASE, channel_number, raw_results);
	float signed_float = convert_to_signed_float(raw_results);
	float temperature_result = get_temperature(signed_float);
	print_temperature_result(channel_number, temperature_result);
	print_fault_data(raw_results[3]);
	return (temperature_result);
}

float read_direct_adc_results(int channel_number) {
	unsigned char raw_results[4];
	get_raw_results(READ_CH_BASE, channel_number, raw_results);
	float signed_float = convert_to_signed_float(raw_results);	
	float direct_adc_reading = get_direct_adc_reading(signed_float);
	print_direct_adc_reading(channel_number, direct_adc_reading);
	print_fault_data(raw_results[3]);
	return (direct_adc_reading);
}

void get_raw_results(long base_address, int channel_number, unsigned char results[4]) {
  long address = base_address+4*(channel_number-1);
  initialize_memory_read(address);

  results[3]=spi_read(0x00); // fault data
  results[2]=spi_read(0x00); // MSB result byte
  results[1]=spi_read(0x00); // 2nd result byte
  results[0]=spi_read(0x00); // LSB result byte
	
	finish_memory_read();
}

float convert_to_signed_float(unsigned char results[4]) {
  // Get the last 24 bits of the results (the first 8 bits are status bits)
  long x = 0L;
  x= x | ((unsigned long) results[2]<<16)
       | ((unsigned long) results[1]<<8)
       | ((unsigned long) results[0]);
	
  // Convert a 24-bit two's complement number into a 32-bit two's complement number
  boolean sign;  
  if ((results[2]&0b10000000)==128) sign=true; else sign=false;	
  if (sign) x=x | 0xFF000000;

  return float(x);  
}

float get_temperature(float x) {
  // The temperature format is (14, 10) so we divide by 2^10
  return x/1024;
}

float get_direct_adc_reading(float x) {
	// The direct ADC format is (3, 21) so we divide by 2^21
	return x/2097152;
}

void print_temperature_result(int channel_number, float temperature_result) {
  Serial.print("Channel=");
  Serial.print(channel_number);
  Serial.print(" Temperature Result =");
  Serial.print(temperature_result);
}

void print_direct_adc_reading(int channel_number, float direct_adc_reading) {
  Serial.print("Channel=");
  Serial.print(channel_number);
  Serial.print("Direct ADC reading in V=");
  Serial.print(direct_adc_reading);
}

// Translate the fault byte into usable fault data and print it out
void print_fault_data(unsigned char fault_byte) {
  Serial.print("  FAULT DATA=");
  Serial.println(fault_byte,BIN);

  if ((fault_byte&SENSOR_HARD_FAILURE)>0) Serial.println("*SENSOR HARD FALURE*");
  if ((fault_byte&ADC_HARD_FAILURE)>0) Serial.println("*ADC_HARD_FAILURE*");
  if ((fault_byte&CJ_HARD_FAILURE)>0) Serial.println("*CJ_HARD_FAILURE*");
  if ((fault_byte&CJ_SOFT_FAILURE)>0) Serial.println("*CJ_SOFT_FAILURE*");
  if ((fault_byte&SENSOR_ABOVE)>0) Serial.println("*SENSOR_ABOVE*");
  if ((fault_byte&SENSOR_BELOW)>0) Serial.println("*SENSOR_BELOW*");
  if ((fault_byte&ADC_RANGE_ERROR)>0) Serial.println("*ADC_RANGE_ERROR*");
  if ((fault_byte&VALID)!=1) Serial.println("!!!!!!! INVALID READING !!!!!!!!!");
  if (fault_byte==0b11111111) Serial.println("&&&&&&&&&& CONFIGURATION ERROR &&&&&&&&&&&&");
  Serial.println(" "); 
}

// -----------------------------------------------------------------
// 		Getting raw results -
//    voltage (for thermocouples), resistance (for RTDs/thermistors)
// -----------------------------------------------------------------

float read_voltage_or_resistance_results(int channel_number) {
	unsigned char raw_results[4];
	get_raw_results(VOUT_CH_BASE, channel_number, raw_results);
	float signed_float = convert_vr_to_signed_float(raw_results);
	float voltage_or_resistance_result = get_voltage_or_resistance(signed_float);
	print_voltage_or_resistance_result(channel_number, voltage_or_resistance_result);
	return (voltage_or_resistance_result);
}

float convert_vr_to_signed_float(unsigned char results[4]) {
  long x = 0L;
  x= x | ((unsigned long) results[3]<<24)
       | ((unsigned long) results[2]<<16)
       | ((unsigned long) results[1]<<8)
       | ((unsigned long) results[0]);
  return float(x);
}

float get_voltage_or_resistance(float x) {
	// The format is (14, 10) so we divide by 2^10
	return x/1024;
}

void print_voltage_or_resistance_result(int channel_number, float voltage_or_resistance_result) {
  Serial.print("Channel=");
  Serial.print(channel_number);
  Serial.print(" Voltage or resistance=");
  Serial.print(voltage_or_resistance_result);
  Serial.print("\n");
}

// -----------------------------------------------------------------

// Find out if a number is an element in an array
bool is_number_in_array(int number, int *array, int array_length) {
	int i;
	bool found = false;
	for (i=0; i< array_length; i++) {
		if (number == array[i]) {
			found = true;
		}
	}
	return found;
}

