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
#include "support_functions.h"
#include "table_coeffs.h"

// Function prototypes
void configure_channels();



// -------------- Main stuff --------------------------------------------------
void setup() {
	initialize_serial();
	initialize_spi();
	
	configure_channels();
} 



void configure_channels() {
	byte channel_number;
	long channel_assignment_data;		

	// ----- Channel 1: Assign Type K Thermocouple -----
	channel_assignment_data =
		(long) SENSOR_TYPE__TYPE_K_THERMOCOUPLE |
		(long) TC_COLD_JUNCTION_CH__2 |
		(long) TC_SINGLE_ENDED |
		(long) TC_OPEN_CKT_DETECT__YES |
		(long) TC_OPEN_CKT_DETECT_CURRENT__10UA;
	assign_channel(1, channel_assignment_data);

	// ----- Channel 2: Assign Off-Chip Diode -----
	channel_assignment_data =
		(long) SENSOR_TYPE__OFF_CHIP_DIODE |
		(long) DIODE_SINGLE_ENDED |
		(long) DIODE_NUM_READINGS__3 |
		(long) DIODE_AVERAGING_ON |
		(long) DIODE_CURRENT__20UA_80UA_160UA |
		(long) 0b0100000000110001001001 << DIODE_IDEALITY_FACTOR_LSB;		// diode - eta: 1.00299930572509765625
	assign_channel(2, channel_assignment_data);

	// ----- Channel 4: Assign Type T Thermocouple -----
	channel_assignment_data =
		(long) SENSOR_TYPE__TYPE_T_THERMOCOUPLE |
		(long) TC_COLD_JUNCTION_CH__2 |
		(long) TC_DIFFERENTIAL |
		(long) TC_OPEN_CKT_DETECT__YES |
		(long) TC_OPEN_CKT_DETECT_CURRENT__100UA;
	assign_channel(4, channel_assignment_data);


}

void loop() { 
	float temperature_result;
	byte channel_number; 
	
	int channels_to_measure[] = {1, 2, 4};
	int num_measured_channels = sizeof(channels_to_measure)/sizeof(channels_to_measure[0]);
	

	
	for (int i = 0; i < num_measured_channels; i++) {
		channel_number = channels_to_measure[i];
		convert_channel(channel_number);

			read_temperature_results( int(channel_number));

	}

}
