#include <project.h>
#include <stdint.h>

#include "cell_interface.h"
#include "bat_pack_interface.h"
#include "current_sense.h"
#include "WDT.h"
#include "data.h"
#include "can_manager.h"
#include "uart-terminal.h"
#include "BMS_monitor.h"
#include "LTC68042.h"
#include "math.h"

//#define WDT_ENABLE

#define DEBUG_MODE //TODO: comment this out when running  on actual car

typedef enum 
{
	BMS_BOOTUP,
	BMS_NORMAL,
	BMS_CHARGEMODE,
	BMS_RACINGMODE,
	BMS_DEBUGMODE,
	BMS_SLEEPMODE,
	BMS_FAULT
} BMS_MODE;

uint8_t cfga_on_init[6];
uint8_t auxa[6];
volatile uint8_t CAN_UPDATE_FLAG=0;
extern volatile BAT_PACK_t bat_pack;
extern BAT_SUBPACK_t bat_subpack[N_OF_SUBPACK];
extern volatile BAT_ERR_t* bat_err_array;
extern volatile uint8_t bat_err_index;
extern volatile uint8_t bat_err_index_loop;
volatile uint8_t CAN_DEBUG=0;
volatile uint8_t RACING_FLAG=0;    // this flag should be set in CAN handler
BAT_SOC_t bat_soc;


uint8_t rx_cfg[IC_PER_BUS][8];

void DEBUG_send_cell_voltage();
void DEBUG_send_temp();
void DEBUG_send_current();


int loop_count = 0; // TODO remove when fan controller tests stops
int increment_mode = 1;

CY_ISR(current_update_Handler){
    current_timer_STATUS;
	update_soc();
	return;
}

void process_event(){
    CyGlobalIntDisable
    // heartbeat
    CyDelay(10);
    can_send_status(0xFE,
    	bat_pack.SOC_percent,
    	bat_pack.status,
    	0,0,0);
    CyDelay(10);
    // send voltage   
    can_send_volt(bat_pack.LO_voltage,
				bat_pack.HI_voltage,
				bat_pack.voltage);
    CyDelay(10);

    // send temperature
    
    // TEST_DAY_1
    can_send_temp(bat_pack.subpacks[0]->high_temp,
				bat_pack.subpacks[1]->high_temp,
                bat_pack.subpacks[2]->high_temp,
                bat_pack.subpacks[3]->high_temp,
                bat_pack.subpacks[4]->high_temp,
                bat_pack.subpacks[5]->high_temp,
				bat_pack.HI_temp_node,
				bat_pack.HI_temp_c);
    
    // send current
    //CyDelay(10);
    //can_send_current(bat_pack.current);
    CyDelay(10);
    
    CyGlobalIntEnable;
}

void process_failure_helper(BAT_ERR_t err){
	switch(err.err){
		case CELL_VOLT_OVER:
        	can_send_volt(((err.bad_node<<8) | err.bad_cell),
			    bat_subpack[err.bad_node].cells[err.bad_cell]->voltage, bat_pack.voltage);
		case CELL_VOLT_UNDER:
			can_send_volt(((err.bad_node<<8) | err.bad_cell),
				bat_subpack[err.bad_node].cells[err.bad_cell]->voltage, bat_pack.voltage);
			break;
		case PACK_TEMP_OVER:
		case PACK_TEMP_UNDER:
			// waiting for CAN mesg been defined clearly
			break;

	}
	return;
}

void process_failure(){
	uint8_t i=0;
	// broadcast error in inverse chronological order
	if (bat_err_index_loop){
		// start from bat_err_index back to 0
		for (i=bat_err_index;i>=0;i--){
			process_failure_helper(bat_err_array[i]);
		}
		// start from index=99 to bat_err_index+1
		for (i=99;i>bat_err_index;i--){
			process_failure_helper(bat_err_array[i]);
		}
	}else{
		// start from bat_err_index back to 0
		for (i=bat_err_index;i>=0;i--){
			process_failure_helper(bat_err_array[i]);
		}
	}
}

bool BALANCE_FLAG = true;

typedef enum {
    FAN_MAX,
    FAN_ZERO,
    FAN_MIN
} FAN_MODE;



int main(void)
{
    // Stablize the BMS OK signal when system is still setting up
    OK_SIG_Write(1);
    
    
	// Initialize state machine
	BMS_MODE bms_status = BMS_BOOTUP;
    FAN_MODE fan_mode = FAN_ZERO;
    
	uint32_t system_interval = 0;
    uint8_t led = 0;
    //FanController_1_SetDutyCycle(1, 44000);
    FanController_Start();
    FanController_SetDesiredSpeed(1, 0);
    FanController_SetDesiredSpeed(2, 0);
    FanController_SetDesiredSpeed(3, 0);
    FanController_SetDesiredSpeed(4, 0);
    
    LTC6804_initialize(); // make  sure command info is set to begin?
    
    
	while(1){
		switch (bms_status){
			case BMS_BOOTUP:
                //current_update_ISR_StartEx(current_update_Handler);
                //current_timer_Start();
				can_init();
				
				#ifdef WDT_ENABLE
                    // TODO Watchdog Timer
			        CyWdtStart(CYWDT_1024_TICKS,CYWDT_LPMODE_NOCHANGE);
                #endif
                                
				// Initialize
                //SOC_Store_Start();
                //SOC_Timer_Start();
				bms_init();
				mypack_init();
                //current_init();              
			    //monitor_init();
			    
			    //enable global interrupt
			    CyGlobalIntEnable;
		    
			    //some variables and states
			    OK_SIG_Write(1);
                bms_status = BMS_NORMAL;
		        //terminal_run();
                #ifdef DEBUG_MODE
                    //debugMain();
                #endif
				break;

			case BMS_NORMAL:
                // while loop with get volt get temp and bat_balance no delays
                // DCP Enable in 68042.c!!!
			    OK_SIG_Write(1);
			    //check_cfg(rx_cfg);  //CANNOT be finished, because 
				
                /*Only here to check that the old voltage reading still works*/
		        get_cell_volt();// TODO test voltage
                
				//TESTDAY_2_TODO. check_stack_fuse(); // TODO: check if stacks are disconnected
                
				//get_cell_temp(); Old temperature getting function (not needed for FE6)
                                
                /*
                    New cell temperature getter
                    1. For each 6811 on each slave
                    2. For mux select 0-7
                    3. wrcfga to set mux select
                    4. adax to convert mux output to digital and store in register
                    5. rdaux to read stored digital value
                
                    Temperature values written to bat_pack.subpacks[subpack]->temps[temp]->temp_c
                */
                get_cell_temps_fe6();
                
                //float32 med_temp = get_median_temp(temperatures)
                float32 temperatures[NUM_SUBPACKS][NUM_TEMPS];
                
                // grab all of the temperatures into single array for cleaner processing later
                for(uint subpack_num = 0; subpack_num < NUM_SUBPACKS; subpack_num++) { //15 cell, 9 board
                    for(uint temp_index = 0; temp_index < NUM_CELL_TEMPS; temp_index++) {
                        temperatures[subpack_num][temp_index] = get_subpack_celltemp(subpack_num, temp_index);
                    }
                    for(uint temp_index = NUM_CELL_TEMPS; temp_index < NUM_TEMPS; temp_index++) {
                        temperatures[subpack_num][temp_index] = get_subpack_boardtemp(subpack_num, temp_index - NUM_CELL_TEMPS);
                    }
                }
                
                //float32 med_temp = get_median_temp(temperatures); 
                float32 med_temp = 28; //hardcoded for now
                //fan control
                if(med_temp >= 55.0 && fan_mode != FAN_MAX) {
                    //full speed
                    //TODO: set all fans to same speed. 
                    FanController_SetDesiredSpeed(1, 6000); //TODO: check actual speed range
                    FanController_SetDesiredSpeed(2, 6000);
                    FanController_SetDesiredSpeed(3, 6000);
                    FanController_SetDesiredSpeed(4, 6000);
                    fan_mode = FAN_MAX;
                }
                else if(med_temp >= 50.0 && fan_mode != FAN_MIN) {
                    //start up fans
                    FanController_SetDesiredSpeed(1, 4500);
                    FanController_SetDesiredSpeed(2, 4500);
                    FanController_SetDesiredSpeed(3, 4500);
                    FanController_SetDesiredSpeed(4, 4500);
                    fan_mode = FAN_MIN;
                } 
                else if(fan_mode != FAN_ZERO){
                    FanController_SetDesiredSpeed(1, 0);
                    FanController_SetDesiredSpeed(2, 0);
                    FanController_SetDesiredSpeed(3, 0);
                    FanController_SetDesiredSpeed(4, 0);
                    fan_mode = FAN_ZERO;
                }    
                CyDelay(10);
                
                // TODO: Calculate SOC
                //get_current(); // TODO get current reading from sensor
			    //bat_soc = get_soc(); // TODO calculate SOC()
				// because it is normal mode, just set a median length current reading interval
			    
                //SKY_TODO update_soc()
              
#ifdef DEBUG_MODE
                /*TODO: remove  fan testing code*/
                
                if(loop_count == 0) {
                    FanController_SetDesiredSpeed(1, 4400);
                    FanController_SetDesiredSpeed(2, 4400);
                    FanController_SetDesiredSpeed(3, 4400);
                    FanController_SetDesiredSpeed(4, 4400);
                    loop_count = 1;
                } else {
                    FanController_SetDesiredSpeed(1, 0);
                    FanController_SetDesiredSpeed(2, 0);
                    FanController_SetDesiredSpeed(3, 0);
                    FanController_SetDesiredSpeed(4, 0);
                    loop_count = 0;
                }
    
                /* Data structure for tracking cell voltages over time - only used for debugging purposes*/
                uint16_t pack_voltages[NUM_SUBPACKS][28];
                uint8_t pack;
                uint8_t cell;
                for (pack = 0; pack < NUM_SUBPACKS; pack++){
                    for (cell = 0; cell < 28; cell++) {
                        pack_voltages[pack][cell] = get_subpack_voltage(pack, cell);
                    }
                }
#endif
                
                //Uncomment all of this to balance
                if (bat_pack.HI_temp_board_c >= 60) {
                    BALANCE_FLAG = false;
                } else if (bat_pack.HI_temp_board_c < 55) {
                    BALANCE_FLAG = true;
                }
                
                if (BALANCE_FLAG) {
                    
                    // Turn on cell discharging
                    bat_balance();
                    // Let it discharge for ... seconds
                    CyDelay(1000);
                    bat_clear_balance();
                    // Let the boards cool down
                    CyDelay(1000);
                }
                
                
                bat_health_check();
                if (bat_pack.health == FAULT){
					bms_status = BMS_FAULT;
				}

                set_current_interval(100);
				system_interval = 500;
				break;

/*
			case BMS_CHARGEMODE:
				OK_SIG_Write(1);

				//check_cfg();  //CANNOT be finished, because 
				//check_cells();// TODO This function will be finished in get_cell_volt/check stack fuse
		        get_cell_volt();// TODO Get voltage
				check_stack_fuse(); // TODO: check if stacks are disconnected
				get_cell_temp();// TODO Get temperature
				get_current(); // TODO get current reading from sensor
				update_soc(); // TODO calculate SOC()
				// because it is normal mode, just set a median length current reading interval
				bat_health_check();
                set_current_interval(100);
				system_interval = 1000;

                if (bat_pack.health == FAULT){
					bms_status = BMS_FAULT;
				}
				if (!(bat_pack.status || CHARGEMODE)){
					bms_status = CHARGEMODE;
				}
				break;

			case BMS_RACINGMODE:
				OK_SIG_Write(1);

				//check_cfg();  //CANNOT be finished, because 
				//check_cells();// TODO This function will be finished in get_cell_volt/check stack fuse
		        get_cell_volt();// TODO Get voltage
				check_stack_fuse(); // TODO: check if stacks are disconnected
				get_cell_temp();// TODO Get temperature
				get_current(); // TODO get current reading from sensor
				bat_soc = get_soc(); // TODO calculate SOC()
                update_soc();
                bat_health_check();
				system_interval = 1000;
				set_current_interval(1);

                
				if (bat_pack.health == FAULT){
					bms_status = BMS_FAULT;
				}
				if (!RACING_FLAG){
					bms_status = BMS_NORMAL;
				}
                
				break;

			case BMS_SLEEPMODE:
				OK_SIG_Write(1);
                bat_health_check();
                if (bat_pack.health == FAULT){
					bms_status = BMS_FAULT;
				}
				break;
*/
			case BMS_FAULT:
				OK_SIG_Write(0u);
				// send
                process_event(); // CAN broadcasting
                /*
                * Sirius: looks like the CAN broadcasting is redundant above. 
                * But I am just being extra cautious for the FE4 competition
                */
				bms_status = BMS_FAULT;
				system_interval = 500;
				process_failure();
                break;
			default:
				bms_status = BMS_FAULT;
                break;
		}
        #ifdef WDT_ENABLE
		    CyWdtClear();
        #endif
		process_event();
		CyDelay(system_interval);
	}
} // main()
