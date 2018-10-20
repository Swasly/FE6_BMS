#include "current_sense.h"
#include "cell_interface.h"
#include "math.h"

extern BAT_PACK_t bat_pack;


void current_init(void)
{
	ADC_current_Start();
    MUX_current_Start();
    set_current_interval(100);
} // current_init()


void set_current_interval(uint16_t interval){
	// set current interval
    current_timer_WritePeriod(interval);
	return;
}

int16_t get_current(void)
{
	//uint16_t raw_vol = 0;
	//float current=0;
	float current;
    int32_t raw_voltage;
    int16_t raw_supply;

	// Delta Sigma measurment of Hall effect sensor
	/*
	ADC_current_StartConvert();
	ADC_current_IsEndConversion(ADC_current_WAIT_FOR_RESULT);
	ADC_current_StopConvert();
	raw_vol = ADC_current_GetResult16();
	raw_vol = (5*(unsigned)raw_vol/0xffff);
	*/
    MUX_current_Connect(1);
    CyDelay(5);
    raw_supply= (int32_t)ADC_current_CountsTo_mVolts(ADC_current_Read32());
    MUX_current_Connect(0);
    CyDelay(5);
	raw_voltage = (int32_t)ADC_current_CountsTo_mVolts(ADC_current_Read32());

	//todo: convert current to float
	// current = (Vi - 2.5)/4
	// Vi referenced to 5V
	//current = ((10*current-25)/4)/10;
    // current in ampere
	current = CURRENT_DIR*(float)(raw_voltage-2584)/4;

	//mypack.current = (int16_t)floor(current);
	bat_pack.current = (int16_t)floor(current*10); 
    //bat_pack.current = (int16_t)raw_supply;
    
    /*
    // comment out for 2/11/2017
	if(bat_pack.current<(-2)){
		bat_pack.status |= CHARGEMODE;
	}else{
		bat_pack.status &= ~CHARGEMODE;
	}
    */
    
	return (int16_t)floor(current*10);
}// get_current()


