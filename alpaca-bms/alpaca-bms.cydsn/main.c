#include <project.h>
#include <stdint.h>
#include "cell_interface.h"
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
}BMS_MODE;

#define BOARD_TEMPS_PER_PACK 15u
#define CELL_TEMPS_PER_PACK 9u
#define TEMPS_PER_PACK 24u

uint8_t cfga_on_init[6];
uint8_t auxa[6];
volatile uint8_t CAN_UPDATE_FLAG=0;
extern volatile BAT_PACK_t bat_pack;
extern BAT_SUBPACK_t bat_subpack[N_OF_SUBPACK];
extern volatile float32 sortedTemps[N_OF_TEMP]; 
extern float32 high_temp_overall;
extern volatile BAT_ERR_t* bat_err_array;
extern volatile uint8_t bat_err_index;
extern volatile uint8_t bat_err_index_loop;
volatile uint8_t CAN_DEBUG=0;
volatile uint8_t RACING_FLAG=0;    // this flag should be set in CAN handler
BAT_SOC_t bat_soc;
uint8_t rx_soc;
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

float32 getMedianTemp()
{
    return (sortedTemps[45] + sortedTemps[44]) / 2;
}

/*
    Code:
    v = cell voltage
    b = board temp
    t = temp

    //Data sent individually in following format.
    code.subpack_index.index.value
*/
void printUsbData(char code, uint subpack, uint index, void *data)
{
    USBUART_PutChar(code);
    USBUART_PutChar('.');
    USBUART_PutChar(subpack);
    USBUART_PutChar('.');
    USBUART_PutChar(index);
    USBUART_PutChar('.');
    uint16 cell;
    uint32 temp;
    
    switch(code)
    {
        case 'c': 
            cell = *((uint16_t*)data);
            char lsb = cell & 0x0f;
            char msb = (cell & 0xf0) >> 8;
            USBUART_PutChar(msb);
            USBUART_PutChar(lsb);            
            break;
        case 'b':
        case 't': //NOTE: ignore the naming here
            temp = *((uint32 *)data);
            char ms = (temp & 0xf000) >> 24;
            char ns = (temp & 0x0f00) >> 16;
            char os = (temp & 0x00f0) >> 8;
            char ls = temp & 0x000f;
            USBUART_PutChar(ms);
            USBUART_PutChar(os);
            USBUART_PutChar(ns);
            USBUART_PutChar(ls);
            break;
    }
    USBUART_PutChar('\n');
}

uint8 lastHighTemp = 0;
uint8 lastHighSubpack = 0;
uint8 lastHighIndex = 0;
int repeatHighsCount = 0; //count the number of times the same high is reached
int repeatHighNode = 0; //count the number of times in a row the same node is high

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
    #ifdef DEBUG_MODE
        // Send board and thermistors temperatures over USB

        char buffer[12];

        
        // send cell voltages 
        for(uint subpack = 0; subpack < 6; subpack++) {
            for(uint ind = 0; ind < 28; ind++) {
                printUsbData('c', subpack, ind, (void *)&bat_pack.subpacks[subpack]->cells[ind]->voltage);
            }
        }
        // send board temps
        for(uint subpack = 0; subpack < 6; subpack++) {
            for(uint ind = 0; ind < 9; ind++) {
                printUsbData('b', subpack, ind, (void *)&bat_pack.subpacks[subpack]->board_temps[ind]->temp_c);
            }
        }
        // send cell temps
        for(uint subpack = 0; subpack < 6; subpack++) {
            for(uint ind = 0; ind < 15; ind++) {
                printUsbData('t', subpack, ind, (void *)&bat_pack.subpacks[subpack]->temps[ind]->temp_c);
            }
        }
    #endif
    
    
    uint8 currHighTemp = bat_pack.HI_temp_c;
    uint8 currHighNode = bat_pack.HI_temp_node;
    uint8 currHighIndex = bat_pack.HI_temp_node_index;
    
    // TEST_DAY_1
    //send temp only if within reasonable range from last temperature

    can_send_temp(bat_pack.subpacks[0]->high_temp,
			bat_pack.subpacks[1]->high_temp,
            bat_pack.subpacks[2]->high_temp,
            bat_pack.subpacks[3]->high_temp,
            bat_pack.subpacks[4]->high_temp,
            bat_pack.subpacks[5]->high_temp,
			bat_pack.HI_temp_node,
			bat_pack.HI_temp_c);
    
    can_send_volt(bat_pack.LO_voltage, bat_pack.HI_voltage, bat_pack.voltage);
    // send current
    //can_send_current(bat_pack.current);
    CyDelay(10);

    CyGlobalIntEnable;
}

void DEBUG_send_cell_voltage(){
    /*
    uint8_t node, cell;
    for (node = 0; node< N_OF_NODE; node++){
        cell = 0;
        for (cell = 0;cell<14;cell++){
            can_send_volt((node<<8 | cell),
				bat_pack.nodes[node]->cells[cell]->voltage,
				bat_pack.voltage);
            CyDelay(1);
        }
    }
    */
}

void DEBUG_send_temp(){
    /*
    uint8_t node, temp;
    for (node = 0; node< N_OF_NODE; node++){
        temp = 0;
        for (temp = 0;temp<10;temp++){
 //           can_send_temp(temp,
//				node,
//				bat_pack.nodes[node]->temps[temp]->temp_c,
//				bat_pack.nodes[node]->temps[temp]->temp_raw,
  //              bat_pack.HI_temp_c);
            CyDelay(1);
        }
    }
    */
}

void DEBUG_send_current(){
    /*
    uint8_t node, temp;
    for (node = 0; node< N_OF_NODE; node++){
        temp = 0;
        for (temp = 0;temp<10;temp++){
//            can_send_temp(temp,
//				node,
//				bat_pack.nodes[node]->temps[temp]->temp_c,
//				bat_pack.nodes[node]->temps[temp]->temp_raw,
  //              bat_pack.HI_temp_c);
            CyDelay(1);
        }
    }
    */
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

/*void debugMain() {
    //while(1) { wakeup_sleep(); };
    uint8_t txData[2] = {0x1E, 0xEE};
    uint8_t rxData[23];
    int t = 0;
    while(1) {

        // For debugging stay in here.
        
        //get_cell_volt();
        //wakeup_sleep();
        CyDelay(500);
        //Select6820_Write(0);
        //my_spi_write_read(txData, 2, rxData, 23);
        //open_wire_adow(0x1);
        //open_wire_adow(0x0);
        
        get_cell_volt();
        get_cell_temp();
        t++;
    }
    while(1) {
        // DO nothing
    }
        
}*/
bool BALANCE_FLAG = true;
BMS_MODE previous_state = BMS_BOOTUP;

typedef enum {
    FAN_MAX,
    FAN_ZERO,
    FAN_MIN
} FAN_MODE;



int main(void)
{
    // Stablize the BMS OK signal when system is still setting up
    OK_SIG_Write(1);
    SOC_Store_Enable();
    
    
	// Initialize state machine
	BMS_MODE bms_status = BMS_BOOTUP;
    FAN_MODE fan_mode = FAN_ZERO;
    
	uint32_t system_interval = 0;
    uint8_t led = 0;
    
    FanController_Start();
    FanController_SetDesiredSpeed(1, 0);
    FanController_SetDesiredSpeed(2, 0);
    FanController_SetDesiredSpeed(3, 0);
    FanController_SetDesiredSpeed(4, 0);
    
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
				bms_init(MD_FILTERED);
				mypack_init();
                //current_init();              
			    //monitor_init();
			    
			    //enable global interrupt
			    CyGlobalIntEnable;
		    
			    //some variables and states
			    OK_SIG_Write(1);
                bms_status = BMS_NORMAL;
                previous_state = BMS_BOOTUP;
		        //terminal_run();
                #ifdef DEBUG_MODE
                    //debugMain();
                #endif
				break;

			case BMS_NORMAL:
                // while loop with get volt get temp and bat_balance no delays
                // DCP Enable in 68042.c!!!
			    OK_SIG_Write(1);
                
                // read SOC from EEPROM on bootup
                // send CAN message with SOC
                //if(previous_state == BMS_BOOTUP) {
                   //uint8_t soc = read_rom_soc();
                   //can_send_soc();
                //}
                
			    //check_cfg(rx_cfg);  //CANNOT be finished, because 
				
                /*Only here to check that the old voltage reading still works*/
                bms_init(MD_FILTERED);
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
                bms_init(MD_NORMAL);
                get_cell_temps_fe6();
                
                
#ifdef DEBUG_MODE         
                float32 temperatures[6][24];
                
                // grab all of the temperatures into single array for cleaner processing later
                for(int i = 0; i < 6; i++) {
                    //15 thermistors and 9 board temps per subpack
                    for(int j = 0; j < 15; j++) {
                        temperatures[i][j] = bat_pack.subpacks[i]->temps[j]->temp_c;
                    }
                    for(int j = 15; j < 24; j++) {
                        temperatures[i][j] = bat_pack.subpacks[i]->board_temps[j - 15]->temp_c;
                    }
                }
#endif

                // grab median temperature
                uint8_t medianTemp = (uint8_t)getMedianTemp();
                //uint16 desiredRPM = (bat_pack.HI_temp_c * 650) - (17000);
                uint16 desiredRPM = (medianTemp * 650) - (17000);
                uint16 saturation = 4 * desiredRPM;
                if (desiredRPM > 12500)
                    desiredRPM = 12500;
                
                if (bat_pack.HI_temp_c < 30) {
                    FanController_SetDesiredSpeed(1, 0);
                    FanController_SetDesiredSpeed(2, 0);
                    FanController_SetDesiredSpeed(3, 0);
                    FanController_SetDesiredSpeed(4, 0);     
                }
                else {
                    FanController_SetSaturation(1, saturation, 0);
                    FanController_SetSaturation(2, saturation, 0);
                    FanController_SetSaturation(3, saturation, 0);
                    FanController_SetSaturation(4, saturation, 0);
                    FanController_SetDesiredSpeed(1, desiredRPM);
                    FanController_SetDesiredSpeed(2, desiredRPM);
                    FanController_SetDesiredSpeed(3, desiredRPM);
                    FanController_SetDesiredSpeed(4, desiredRPM);                     
                }
                CyDelay(10);
                
                // TODO: Calculate SOC
                //get_current(); // TODO get current reading from sensor
			    //bat_soc = get_soc(); // TODO calculate SOC()
				// because it is normal mode, just set a median length current reading interval
			    
                //SKY_TODO update_soc()
              
#ifdef DEBUG_MODE

                /* Data structure for tracking cell voltages over time - only used for debugging purposes*/
                uint16_t pack_voltages[6][28];
                uint8_t pack;
                uint8_t cell;
                for (pack = 0; pack < 6; pack++){
                    for (cell = 0; cell < 28; cell++) {
                        pack_voltages[pack][cell] = bat_pack.subpacks[pack]->cells[cell]->voltage;
                    }
                }
#endif
                /*
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
                    CyDelay(10000);
                    bat_clear_balance();
                    // Let the boards cool down
                    CyDelay(1000);
                }
                */
                       
                bat_health_check();
                if (bat_pack.health == FAULT){
					bms_status = BMS_FAULT;
				}

                set_current_interval(100);
				system_interval = 10;
                
                rx_soc = can_rx_soc();
                //if(rx_soc != 255) {
                //    write_soc(rx_soc);
                //}
                
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
