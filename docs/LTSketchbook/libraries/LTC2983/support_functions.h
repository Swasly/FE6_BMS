/*!
support_functions.h:
This file contains all the support function prototypes used in the main program.

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

void initialize_serial();
void initialize_spi();
void assign_channel(int channel_number, long channel_assignment_data);
void write_custom_table(struct table_coeffs coefficients[64], long start_address, long table_length);
void write_custom_steinhart_hart(long steinhart_hart_coeffs[6], long start_address);
void write_single_byte(long start_address, int single_byte);
void initialize_memory_write(long start_address);
void write_coefficient(long coeff, int bytes_in_coeff);
void finish_memory_write();
void initialize_memory_read(long start_address);
void finish_memory_read();
void convert_channel(byte channel_number);
boolean conversion_done();

float read_temperature_results(int channel_number);
float read_direct_adc_results(int channel_number);
void get_raw_results(long base_address, int channel_number, unsigned char results[4]);
float convert_to_signed_float(unsigned char results[4]);
float get_temperature(float x);
float get_direct_adc_reading(float x);
void print_temperature_result(int channel_number, float temperature_result);
void print_direct_adc_reading(int channel_number, float direct_adc_reading);
void print_fault_data(unsigned char fault_byte);
bool is_number_in_array(int number, int *array, int array_length);

// -------------- Some raw data result...
float read_voltage_or_resistance_results(int channel_number);
float convert_vr_to_signed_float(unsigned char results[4]);
float get_voltage_or_resistance(float x);
void print_voltage_or_resistance_result(int channel_number, float voltage_or_resistance_result);
