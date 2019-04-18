/* LICENSE
	cell_interface.h and cell_interface.c are derivatives of source from Linear
	Technology Corp.(LTC)
*/

/************************************

***********************************************************/



#include "cell_interface.h"
#include "bat_pack_interface.h"
#include "current_sense.h"
#include "LTC68042.h"
#include "Select6820.h"
#include <math.h>
#include <stdlib.h>

uint8_t fatal_err;

// FE3 new structure
BAT_CELL_t bat_cell[N_OF_CELL];
BAT_TEMP_t bat_temp[N_OF_TEMP];
BAT_TEMP_t board_temp[N_OF_TEMP_BOARD];
BAT_SUBPACK_t bat_subpack[N_OF_SUBPACK];
volatile BAT_ERR_t bat_err;
BAT_PACK_t bat_pack;

extern volatile uint8_t CAN_DEBUG;
volatile uint8_t bad_therm=0;
volatile BAT_ERR_t bat_err_array[100];
volatile uint8_t bat_err_index;
volatile uint8_t bat_err_index_loop;
uint32_t deltaTime;
uint32_t lastTime;


/**
 * @initialize. In case need to setup anything, write them in it.
 *
 * @param no input parameters.
 * @return 1 if everything is OK. 0 for hard failure.
 */
void  bms_init(uint8_t adc_mode){
    SS_SetDriveMode(SS_DM_RES_UP);
    LTC68_Start();
    LTC6804_initialize(adc_mode);
    Select6820_Write(0); // Configure each bus
    LTC6804_wrcfg(IC_PER_BUS, tx_cfg);
    Select6820_Write(1);
    LTC6804_wrcfg(IC_PER_BUS, tx_cfg);
}


/**
 * @initialize the mypack struct. 
 *
 * @param no input parameters.
 * @return 1 if everything is OK. 0 for hard failure.
 */
void mypack_init(){
    
    uint8_t cell = 0;
    uint8_t subpack = 0;
    uint8_t bus = 0;
    uint8_t temp = 0;
    bat_err_index = 0;
    bat_err_index_loop = 0;

    // initialize cells and temps
    
    for (cell = 0; cell < N_OF_CELL; cell++){
        bat_cell[cell].voltage = cell;
        bat_cell[cell].bad_counter = 0;
    }
    
    for (temp = 0; temp < N_OF_TEMP; temp++){
        bat_temp[temp].temp_c = (uint8_t)temp;
        bat_temp[temp].bad_counter = 0;
        bat_temp[temp].bad_type = 0;
        bat_temp[temp].type = THERM_CELL;
    }
    for (temp = 0; temp < N_OF_TEMP_BOARD; temp++){
        board_temp[temp].temp_c = (uint8_t)temp;
        board_temp[temp].bad_counter = 0;
        board_temp[temp].bad_type = 0;
        board_temp[temp].type = THERM_BOARD;
    }

    for (subpack = 0; subpack < N_OF_SUBPACK; subpack++){
        for (cell = 0; cell < (N_OF_CELL / N_OF_SUBPACK); cell++){    
            bat_subpack[subpack].cells[cell] = &(bat_cell[subpack*(N_OF_CELL / N_OF_SUBPACK)+cell]);
        }
        for (temp = 0; temp < (N_OF_TEMP / N_OF_SUBPACK); temp++){
            bat_subpack[subpack].temps[temp] = &(bat_temp[subpack*(N_OF_TEMP / N_OF_SUBPACK)+temp]);
        }
        for (temp = 0; temp < (N_OF_TEMP_BOARD / N_OF_SUBPACK); temp++){
            bat_subpack[subpack].board_temps[temp] = &(board_temp[subpack*(N_OF_TEMP_BOARD / N_OF_SUBPACK)+temp]);
        }
        
        bat_subpack[subpack].over_temp = 0;
        bat_subpack[subpack].under_temp = 0;
        bat_subpack[subpack].over_voltage = 0;
        bat_subpack[subpack].under_voltage = 0;
        bat_subpack[subpack].voltage = subpack; // non zero for debugging
    }
    
    // Register pack
    for (subpack = 0; subpack < N_OF_SUBPACK; subpack++){
        bat_pack.subpacks[subpack] = &(bat_subpack[subpack]);
    }
    
    /*
    // get SOC
    uint32_t temp_SOC = SOC_Store_ReadByte(0x00)<<24;
    temp_SOC |= SOC_Store_ReadByte(0x01)<<16;
    temp_SOC |= SOC_Store_ReadByte(0x02)<<8;
    temp_SOC |= SOC_Store_ReadByte(0x03);
    
    if(temp_SOC<SOC_SOC_LOW || temp_SOC > SOC_FULL){
        temp_SOC = SOC_NOMIAL;
    }
    bat_pack.current_charge = temp_SOC;
    */
    
    //give the pack non-0 value help debuging CAN
    bat_pack.voltage = 12;
    bat_pack.current = 34;
    bat_pack.fuse_fault = 0;
    bat_pack.status = 0; 
    bat_pack.health = NORMAL;
    bat_pack.SOC_cali_flag =0;
    bat_pack.HI_temp_c = 0;
    bat_pack.HI_temp_raw = 0;
    bat_pack.HI_voltage = 0;
    bat_pack.LO_voltage = 0;
    bat_pack.time_stamp = 0;
    bat_pack.SOC_percent = 0;
}




/**
 * @wake all BMSs up, waiting for commands
 *
 * @param no input parameters.
 * @return 1 if everything is OK. 0 for hard failure.
 */
void  wake_up(){
    wakeup_sleep(0);
}

void check_cfg(uint8_t rx_cfg[][8]){
    LTC68_ClearFIFO();
    int error1 = 0;
    int error2 = 0;
    
    Select6820_Write(0);
    wakeup_sleep(0);
    CyDelay(1);
    //for (int i = 0; i < 100; i++) {
        LTC6804_rdcfg(IC_PER_BUS, rx_cfg);
        LTC6804_rdcfg(IC_PER_BUS, rx_cfg);
    /*
    //DEBUG_UART_PutString("Enter Check_CFG\n");
    int i=0;
    wakeup_sleep();
    LTC6804_rdcfg(TOTAL_IC,rx_cfg);
    //LCD_Position(1u,0u);
    for (i=0;i<8;i++){
        if (rx_cfg[i] != tx_cfg[i]){
            fatal_err = COM_FAILURE;
            return;
        }
    }
    */
}



void check_chips(){
    
}// check_chips()



uint8_t get_cell_volt(){
    
    LTC68_ClearFIFO();
    int error1 = 0;
    int error2 = 0;
    
    CyDelay(1);
    Select6820_Write(0); // allow conversions to happen on bus 0
    CyDelay(1);
    wakeup_sleep(0);
    CyDelay(1); // Waited more
    //for (int i = 0; i < 100; i++) {
        LTC6804_adcv();
        LTC6804_adcv();
    //}
    CyDelay(1); // Give it time before switching
    
    Select6820_Write(1); // allow conversions to happen in bus 1
    wakeup_sleep(1);
    //for (int i = 0; i < 100; i++) {
        LTC6804_adcv();
        LTC6804_adcv();
    //}
    
    CyDelay(200);
    
    Select6820_Write(0); // Select bus 0
    wakeup_sleep(0);
    
    //uint16_t *test = (uint16_t*)(&cell_codes[IC_PER_BUS]); // SHOULD PROBABLY REPLACE WITH SOMETHING LIKE 
                                                             // instead of copying values from two arrays
    
    error1 = LTC6804_rdcv(0, IC_PER_BUS, cell_codes_lower); // Set to read back all cell voltage registers
    
    CyDelay(1); // Give it a moment before switching.
    Select6820_Write(1); // Select bus 1
    wakeup_sleep(1);
    CyDelay(10);
    error2 = LTC6804_rdcv(0, IC_PER_BUS, cell_codes_higher);
    
    if (error1 == -1 || error2 == -1)
    {
        #ifdef DEBUG_LCD
            LCD_Position(0u,10u);
            LCD_PrintString("ERROR");
        #endif
       return 1;
    }
    
    for (unsigned int i = 0; i < IC_PER_BUS; i++) {
        for (int j = 0; j < 12; j++) {
            cell_codes[i][j] = cell_codes_lower[i][j];
        } 
    }
    
    for (unsigned int i = 0; i < IC_PER_BUS; i++) {
        for (int j = 0; j < 12; j++) {
            cell_codes[i + IC_PER_BUS][j] = cell_codes_higher[i][j];
        } 
    }
    
    //get information
    update_volt(cell_codes);
    
    //check error
    check_volt();

    
    if (error1 == -1 || error2 == -2) {
        return -1;   
    } else {
        return 0;   
    }
}// get_cell_volt()

uint8_t open_wire_adow(uint8_t pup){
    LTC68_ClearFIFO();
    int error1 = 0;
    int error2 = 0;
    
    Select6820_Write(0);
    wakeup_sleep(0);
    CyDelay(100); // Waited more
    for (int i = 0; i < 3; i++) {
        LTC6804_adow(pup);
    }
    CyDelay(100); // Give it time before switching
    
    Select6820_Write(1);
    wakeup_sleep(1);
    LTC6804_adow(pup);
    
    CyDelay(10);
    
    Select6820_Write(0); // Select a bus
    wakeup_sleep(0);
    CyDelay(1);
    //uint16_t *test = (uint16_t*)(&cell_codes[IC_PER_BUS]); // SHOULD PROBABLY REPLACE WITH SOMETHING LIKE 
                                                             // instead of copying values from two arrays
    error1 = LTC6804_rdcv(0, IC_PER_BUS, cell_codes_lower); // Set to read back all cell voltage registers
    error1 = LTC6804_rdcv(0, IC_PER_BUS, cell_codes_lower); // Set to read back all cell voltage registers
    
    CyDelay(10); // Give it a moment before switching.
    Select6820_Write(1); // Select a bus
    wakeup_sleep(1);
    error2 = LTC6804_rdcv(0, IC_PER_BUS, cell_codes_higher);
    
    if (error1 == -1 || error2 == -1)
    {
        #ifdef DEBUG_LCD
            LCD_Position(0u,10u);
            LCD_PrintString("ERROR");
        #endif
       return 1;
    }
    
    for (unsigned int i = 0; i < IC_PER_BUS; i++) {
        for (int j = 0; j < 12; j++) {
            cell_codes[i][j] = cell_codes_lower[i][j];
        } 
    }
    
    for (unsigned int i = 0; i < IC_PER_BUS; i++) {
        for (int j = 0; j < 12; j++) {
            cell_codes[i + IC_PER_BUS][j] = cell_codes_higher[i][j];
        } 
    }
    
    //get information
    update_volt(cell_codes);
    
    //check error
    check_volt();

    
    if (error1 == -1 || error2 == -2) {
        return -1;   
    } else {
        return 0;   
    }
}// get_cell_volt()

/*
    Used to get the cfga register values on init,
    We need these values as we only want to write
    to 3 bits in the register, and in order to write
    to 3 bits you must write to them all.
*/
uint8_t get_cfga_on_init(uint8_t lt_addr, uint8_t cfga_data[5]){
    
    int8_t pec_error;
    uint8_t cfga[6];
    
    uint8_t bus = lt_addr / IC_PER_BUS;
    
    LTC68_ClearFIFO();
    
    //Select6820_Write(0);
    Select6820_Write(bus);
    
    wakeup_sleep(bus);
    CyDelay(100);
    
    pec_error = LTC6804_rdcfga(lt_addr, cfga);
    pec_error = LTC6804_rdcfga(lt_addr, cfga);
    
    cfga_data[0] = cfga[1];
    cfga_data[1] = cfga[2];
    cfga_data[2] = cfga[3];
    cfga_data[3] = cfga[4];
    cfga_data[4] = cfga[5];
    
    //TODO: add pec_error check
    return 0;
}


float32 get_median_temp(float32 temps[6][24])
{
    int num_temps = (TEMPS_ON_BOARD + 16) * 6;
    float32 local_temps[24 * 6];
    for(int i = 0; i < 6; i++) {
        memcpy(local_temps + 24*i, temps[i], 24 * sizeof(float32));
    }
    
    float32 temp, start;
    int j;
    
    //insertion sort
    for(int i = 1; i < num_temps; i++) {
        start = local_temps[i];
        j = i - 1;
        while(start < local_temps[j] && j >= 0) {
            local_temps[j + 1] = local_temps[j];
            j--;;
        }
        local_temps[j + 1] = start;
    }
    
    return local_temps[num_temps/2];
}

//#define NUM_LTS (3u) // TODO modify to be fll number of lt chips by the end.
#define TEMPS_ON_BOARD (8u)


/**
    Get all temperatures on a single lt chip
*/
uint8_t get_lt_temps(uint8_t lt_addr, uint8_t orig_cfga_data[5])
{
    uint16_t auxa;
    uint8_t subpack_num = lt_addr / LT_PER_PACK;
    // uint8_t offset = (lt_addr % LT_PER_PACK) * TEMPS_ON_BOARD;
    uint8_t offset;
    
    for(uint8_t mux_sel = 0; mux_sel < 8; mux_sel++) {
        get_cell_temp_fe6(lt_addr, mux_sel, orig_cfga_data, &auxa);
        float32 temp = (float32)auxa/10000;
        temp = (1/((1/298.15) + ((1/3428.0)*log(temp/(3-temp))))) - 273.15;
        if (mux_sel <= 4) {
            offset = ((lt_addr % LT_PER_PACK) * 5) + mux_sel;
            //bat_pack.subpacks[subpack_num]->temps[offset]->temp_c = temp;
            set_subpack_celltemp(subpack_num, offset, temp);
            
        }
        else {
            offset = ((lt_addr % LT_PER_PACK) * LT_PER_PACK) + (mux_sel - 5);
            set_subpack_boardtemp(subpack_num, offset, temp);
        }
    }
    
    return 0;
}


/**
    starting point for getting cell temperatures on the slave boards
               
    New cell temperature getter
    1. For each 6811 on each slave
    2. For mux select 0-7
    3. wrcfga to set mux select
    4. adax to convert mux output to digital and store in register
    5. rdaux to read stored digital value

    Temperature values written to bat_pack.subpacks[subpack]->temps[temp]->temp_c

*/
uint8_t get_cell_temps_fe6()
{
    // the number of lt chips. 
    uint8_t num_lts = 18;
    uint8_t orig_cfga_data[5];
    
    for(int lt = 0; lt < num_lts; lt++) {
        get_cfga_on_init(lt, orig_cfga_data);
        get_lt_temps(lt, orig_cfga_data);
    }
    
    check_temp();
 
    return 0;
}


/*
 * get a temperature for a single cell
 */
uint8_t get_cell_temp_fe6(uint8_t lt_addr, uint8_t mux_sel, uint8_t orig_cfga_data[5], uint16_t *auxa){
    /*
     * 1. Prepare LTC6820 for traffic
     * 2. Select the correct bus
     * 2. Write the analog mux select value to GPIO pins 
     * 3. Start LTC6811 ADC on GPIO1
     * 4. Read the voltage from auxillary register A
    */

    int8_t pec_error;
    uint8_t bus = lt_addr / IC_PER_BUS;
    
    // 1
    LTC68_ClearFIFO();
    
    // 2
    Select6820_Write(bus);
    wakeup_sleep(bus);
    
    // 3
    LTC6804_wrcfga(lt_addr, mux_sel, orig_cfga_data);
    LTC6804_wrcfga(lt_addr, mux_sel, orig_cfga_data);
    
    // 4
    LTC6804_adax();
    LTC6804_adax();
    
    // 5
    LTC6804_rdaux_fe6(lt_addr, GPIO1, auxa);
    LTC6804_rdaux_fe6(lt_addr, GPIO1, auxa);
    
    return 0;
}

uint8_t get_cell_temp(){
    uint8_t command[3];
    uint8_t rawTemp[(N_OF_TEMP + N_OF_TEMP_BOARD) * 2];
    
    // PSOCs have addresses 9 through 11
    for (uint8_t bus = 0; bus < N_OF_BUSSES; bus++) {
        Select6820_Write(bus);
        int address = 9;
        for (int i = 0; i < 3; i++, address++) {
            for (uint8_t j = 0; j < 3; j++)
                command[j] = (0x80 | (address << 3)) + j;
            my_spi_write_read(command, 3, rawTemp + (46 * i) + 
                              (bus * (N_OF_TEMP + N_OF_TEMP_BOARD)), 46); // There are 69 thermistors on each bus, 2 bytes per reading
        }
    }
    
    check_temp();

    return 0;
}// get_cell_temp()


void update_volt(volatile uint16_t cell_codes[IC_PER_BUS * N_OF_BUSSES][12]){
    uint8_t cell = 0;
    uint8_t raw_cell = 0;
    uint8_t node = 0;
    uint8_t ic = 0;
    uint8_t subpack = 0;
    uint32_t temp_volt;
    // Update cell voltages
    for (ic = 0; ic < IC_PER_BUS * N_OF_BUSSES; ic++){
        for (raw_cell=0;raw_cell<12;raw_cell++){
            if (ic == 2 || ic == 5 || ic == 8 || ic == 11 || ic == 14 || ic == 17) { // These ICs have different cells used
                if ((CELL_ENABLE_HIGH & (0x1<<raw_cell))){
                    bat_cell[cell].voltage = cell_codes[ic][raw_cell]/10;  //only record in mV not in 0.1mV
                    cell++;
                }    
            } else {
                if ((CELL_ENABLE_LOW & (0x1<<raw_cell))){
                    bat_cell[cell].voltage = cell_codes[ic][raw_cell]/10;  //only record in mV not in 0.1mV
                    cell++;
                } 
            }
        }
    }
    
    // TODO
    /* if (bat_subpack[2].cells[22]->voltage > 4199)                           // fixes a bug
        bat_subpack[2].cells[22]->voltage = 4199;                           // from hardware
    if (bat_subpack[2].cells[23]->voltage > 4199)
        bat_subpack[2].cells[23]->voltage = 4199; */
    // Update subpack and pack voltages
    temp_volt = 0;
    uint32_t temp_pack_volt = 0;
    for (subpack = 0; subpack < N_OF_SUBPACK; subpack++){
        temp_volt = 0;
        for (cell = 0; cell < (N_OF_CELL / N_OF_SUBPACK); cell++){
            temp_volt += (uint32_t)(bat_subpack[subpack].cells[cell]->voltage);
        }
        bat_subpack[subpack].voltage = temp_volt;
        temp_pack_volt += temp_volt;
    }
    bat_pack.voltage = temp_pack_volt / N_OF_SUBPACK;   
}


void check_volt(){
    uint8_t cell = 0;
    uint8_t subpack = 0;
    uint16_t voltage16 = 0;

    // Check each cell for errors
    for (cell = 0; cell < N_OF_CELL; cell++){
        voltage16 = bat_cell[cell].voltage;
        if (voltage16 > (uint16_t)OVER_VOLTAGE){
            bat_cell[cell].bad_counter++;
            bat_cell[cell].bad_type = 1;
        }else if (voltage16 < (uint16_t)UNDER_VOLTAGE){
            bat_cell[cell].bad_counter++;
            bat_cell[cell].bad_type = 0;
        }else{
            if (bat_cell[cell].bad_counter > 0){
                bat_cell[cell].bad_counter--;
            }           
        }
    }

    // Update subpacks for errors
    for (subpack = 0; subpack < N_OF_SUBPACK; subpack++){
        for (cell = 0; cell < (N_OF_CELL / N_OF_SUBPACK); cell++){
            if (bat_subpack[subpack].cells[cell]->bad_counter > ERROR_VOLTAGE_LIMIT){
                if (bat_subpack[subpack].cells[cell]->bad_type == 0){
                    bat_subpack[subpack].under_voltage |= (1u<<cell);
                }else{
                    bat_subpack[subpack].over_voltage |= (1u<<cell);
                }
            }
        }
    }

    // Update pack for errors
    for (subpack = 0; subpack < N_OF_SUBPACK; subpack++){
        if (bat_subpack[subpack].over_voltage != 0){
            bat_pack.status |= CELL_VOLT_OVER;
            bat_err_add(CELL_VOLT_OVER, bat_subpack[subpack].over_voltage, subpack);
        }

        if (bat_subpack[subpack].under_voltage != 0){
            bat_pack.status  |= CELL_VOLT_UNDER;
            bat_err_add(CELL_VOLT_UNDER, bat_subpack[subpack].under_voltage, subpack);
        }
    }
    
    // Find the max voltage and min voltage
    uint16_t max_voltage = bat_cell[0].voltage;
    uint16_t min_voltage = bat_cell[0].voltage;
    cell=0;
    for(cell = 0; cell < N_OF_CELL; cell++){
        if (max_voltage < bat_cell[cell].voltage){
            max_voltage = bat_cell[cell].voltage;
        }
        if(min_voltage > bat_cell[cell].voltage){
            min_voltage = bat_cell[cell].voltage;
        }
    }
    
    bat_pack.HI_voltage = max_voltage;
    bat_pack.LO_voltage = min_voltage;   
}

                
// Convert mv to degrees C
double mvToC(uint16_t mv) {
    double v = (double)mv / 1000;
    double RT = (v*10000/(5.0-v));
    double T = (1/((1/298.15)+((double)1/3428)*log(RT/10000)))-273.15;
    return T;
}


void check_temp(){
    // TEST_DAY_1
    uint8_t temp=0;
    uint8_t subpack=0;
    uint8_t cell=0;
    float32 temp_c=0;
    
    // check temp
    for (cell = 0; cell < N_OF_TEMP; cell++){
        temp_c = bat_temp[cell].temp_c;
        if (temp_c > CRITICAL_TEMP_H){
            //if over temperature
            bat_temp[cell].bad_counter++;
            bat_temp[cell].bad_type = TEMP_HIGH;
        }else if (temp_c < CRITICAL_TEMP_L){
            // if under temperature
            bat_temp[cell].bad_counter++;
            bat_temp[cell].bad_type = TEMP_LOW;
        }else{
            //if there is no error
            if (bat_temp[cell].bad_counter > 0){
                bat_temp[cell].bad_counter--;
            }           
        }
    }
    
    // check board temps and count number of bad temps
    for (cell = 0; cell < N_OF_TEMP_BOARD; cell++){
        temp_c = board_temp[cell].temp_c;
        if (temp_c > CRITICAL_TEMP_BOARD_H){
            //if over temperature
            board_temp[cell].bad_counter++;
            board_temp[cell].bad_type = 1;
        }else if (temp_c < CRITICAL_TEMP_BOARD_L){
            // if under temperature
            board_temp[cell].bad_counter++;
            board_temp[cell].bad_type = 0;
        }else{
            //if there is no error
            if (board_temp[cell].bad_counter > 0){
                board_temp[cell].bad_counter--;
            }           
        }
    }

    // Update subpacks
    for (subpack = 0; subpack < N_OF_SUBPACK; subpack++){
        for (temp = 0; temp < (N_OF_TEMP / N_OF_SUBPACK); temp++){
            if (bat_subpack[subpack].temps[temp]->bad_counter > ERROR_TEMPERATURE_LIMIT){
                if (bat_subpack[subpack].temps[temp]->bad_type == 0){
                    bat_subpack[subpack].under_temp |= (1u<<temp);
                }else{
                    bat_subpack[subpack].over_temp |= (1u<<temp);
                }
            }
        }
        for (temp = 0; temp < (N_OF_TEMP_BOARD / N_OF_SUBPACK); temp++){
            if (bat_subpack[subpack].board_temps[temp]->bad_counter > ERROR_TEMPERATURE_LIMIT){
                if (bat_subpack[subpack].board_temps[temp]->bad_type == 0){
                    bat_subpack[subpack].under_temp_board |= (1u<<temp);
                }else{
                    bat_subpack[subpack].over_temp_board |= (1u<<temp);
                }
            }
        }
    }
    
    // update temperature highest to each node
    subpack = 0;
    uint8_t temp_temp=0;
    uint8_t i=0;
    for (subpack = 0; subpack < N_OF_SUBPACK; subpack++){
        temp_temp = get_subpack_celltemp(subpack, 0);
        bat_pack.subpacks[subpack]->high_temp = temp_temp;
        for (i = 1; i < NUM_CELL_TEMPS; i++){
            if (temp_temp < get_subpack_celltemp(subpack, i)){
                temp_temp = get_subpack_celltemp(subpack, i);
                bat_pack.subpacks[subpack]->high_temp = temp_temp;
            }
        }
    }

    // Update the battery_pack highest temperature
    bat_pack.HI_temp_c = bat_temp[0].temp_c;
    for (i = 1; i < N_OF_TEMP; i++){
        if (bat_temp[i].temp_c > bat_pack.HI_temp_c){
            bat_pack.HI_temp_c = bat_temp[i].temp_c;
            bat_pack.HI_temp_node = i / (N_OF_TEMP / N_OF_SUBPACK);
        }    
    }
    
    // Update the battery_pack highest temperature
    bat_pack.HI_temp_board_c = board_temp[0].temp_c;
    bat_pack.HI_temp_board_node = 0;
    
    for (i = 1; i < N_OF_TEMP_BOARD; i++){
        if (board_temp[i].temp_c > bat_pack.HI_temp_board_c){
            bat_pack.HI_temp_board_c = board_temp[i].temp_c;
            bat_pack.HI_temp_board_node = i / (N_OF_TEMP_BOARD / N_OF_SUBPACK);
        }    
    }
    
    // update pack of temp error
    for (subpack = 0; subpack < N_OF_SUBPACK; subpack++){
        if (bat_pack.subpacks[subpack]->over_temp != 0){
            bat_pack.status |= PACK_TEMP_OVER;
            bat_err_add(PACK_TEMP_OVER, bat_subpack[subpack].over_temp, subpack);
        }

        if (bat_pack.subpacks[subpack]->under_temp != 0){
            bat_pack.status  |= PACK_TEMP_UNDER;
            bat_err_add(PACK_TEMP_UNDER, bat_subpack[subpack].under_temp, subpack);
        }
        
        if (bat_pack.subpacks[subpack]->over_temp_board != 0){
            bat_pack.status |= PACK_TEMP_OVER;
            bat_err_add(PACK_TEMP_OVER, bat_subpack[subpack].over_temp, subpack);
        }

        if (bat_pack.subpacks[subpack]->under_temp_board != 0){
            bat_pack.status  |= PACK_TEMP_UNDER;
            bat_err_add(PACK_TEMP_UNDER, bat_subpack[subpack].under_temp, subpack);
        }
    }
    
}

void bat_err_add(uint16_t err, uint8_t bad_cell, uint8_t bad_subpack){
    bat_pack.health = FAULT; 
    /* 
    * Sirius: I know this set-to-fault is redundant with the check in bat_health_check(), 
    * but just want to have an extra assurance for FE4 Competition because we haven't run this new firmware
    * on track
    */
    
    uint8_t i=0;
    // check array, dont duplicate
    if (bat_err_index_loop){
        for (i=0;i<100;i++){
            if (err == bat_err_array[i].err
             || bad_cell == bat_err_array[i].bad_cell
             || bad_subpack == bat_err_array[i].bad_node){
                return;
            }
        }
    }else{
        for (i=0;i<bat_err_index;i++){
            if (err == bat_err_array[i].err
             || bad_cell == bat_err_array[i].bad_cell
             || bad_subpack == bat_err_array[i].bad_node){
                return;
            }
        }
    }

    if (bat_err_index>=100){
        bat_err_index_loop = 1;
        bat_err_index = 0;
    }else{
        bat_err_index++;
    }

    bat_err_array[bat_err_index].err = err;
    bat_err_array[bat_err_index].bad_cell = bad_cell;
    bat_err_array[bat_err_index].bad_node = bad_subpack;

    return;
}


uint8_t temp_transfer(uint16_t in_raw, uint16_t ref){
    //using 1/T = 1/T0 +(1/B)(R/R0)
    //V = raw/0xffff*5
    //R is R=10K(5-V)/V;
    //translate raw reading to C temperature
    //B25=3900
    //B75=3936
    if (in_raw==65535){
        //bad reading
        return 0;
    }
    uint16_t raw = in_raw;
    raw=raw+1;
    float v = 3.0*(1.0*raw/(ref*1.0));
    float R=(1.0*10000.0*v/5)/(1-v/5.0);
    float beta = 3950.0;
    float A = 0.01764;
    float T = beta/log(R/A)-273.16;
    return (uint8_t)ceil(T);
}

// The basic idea of SOC estimation is reading current value and integrate them by time.
// Time can be estimated from delta.Time
BAT_SOC_t get_soc()
{
    BAT_SOC_t tempSOC;
    tempSOC.absolute_SOC = bat_pack.current_charge;
    tempSOC.percent_SOC = 100*tempSOC.absolute_SOC/SOC_FULL_CAP;  //percentile SOC
    return tempSOC;
} // get_soc()

void update_soc(){
    // attention!! timer is counting down!
    if (bat_pack.time_stamp==0){
        bat_pack.time_stamp = SOC_Timer_ReadCounter();
        deltaTime = 0;
    }else{
        deltaTime = (SOC_Timer_ReadCounter()<bat_pack.time_stamp ? 
            bat_pack.time_stamp - SOC_Timer_ReadCounter() :
            bat_pack.time_stamp+(65535-SOC_Timer_ReadCounter()));
        bat_pack.time_stamp = SOC_Timer_ReadCounter();
    }
    
    // calibrate SOC when voltage reach linear fion.
    // if its high SOC
    if ((bat_pack.SOC_cali_flag==0) && (bat_pack.voltage >= SOC_CALI_HIGH)){
        bat_pack.current_charge = SOC_SOC_HIGH;
        bat_pack.SOC_cali_flag = 1;
    }
    // if its low SOC
    if ((bat_pack.SOC_cali_flag==0) && (bat_pack.voltage <= SOC_CALI_LOW)){
        bat_pack.current_charge = SOC_SOC_LOW;
        bat_pack.SOC_cali_flag = 1;
    }

    // reset cali falg when charge droped into nomial voltage
    if (bat_pack.voltage < (SOC_CALI_HIGH-1000) || bat_pack.voltage > (SOC_CALI_LOW+1000)){
        bat_pack.SOC_cali_flag = 0;
    }

    // if it is full?
    if (bat_pack.voltage >= SOC_FULL){
        bat_pack.status |= FULL;
    }

    float deltaCharge = 1.0*deltaTime*bat_pack.current*100.0/1000.0;   //get_current returned mA
    // deltaTime = (getTime) - lastTime;
    if (deltaCharge<0){
        //charging
        bat_pack.current_charge = bat_pack.current_charge + (uint32_t)abs(floor(deltaCharge));
    }else{
        bat_pack.current_charge = bat_pack.current_charge - (uint32_t)abs(floor(deltaCharge));
    }
    
    bat_pack.SOC_percent = (uint8_t)floor(100.0*bat_pack.current_charge/SOC_SOC_HIGH);
    
    // write current charge back to EEPROM
    _SOC_log();
  
    
    return;
}

uint8_t bat_health_check(){
    if (
        (bat_pack.status & PACK_TEMP_OVER) ||
        (bat_pack.status & STACK_FUSE_BROKEN) ||
        (bat_pack.status & PACK_TEMP_UNDER) ||
        (bat_pack.status & COM_FAILURE) ||
        (bat_pack.status & CELL_VOLT_OVER) ||
        (bat_pack.status & CELL_VOLT_UNDER)       
    ){
        bat_pack.health = FAULT;
        return 1;
    }else{
        return 0;
    }   
}

void _SOC_log(){
    uint32_t temp_SOC = bat_pack.current_charge;
    SOC_Store_WriteByte((uint8_t)(0xff&temp_SOC>>24), 0x00);
    SOC_Store_WriteByte((uint8_t)(0xff&temp_SOC>>16), 0x01);
    SOC_Store_WriteByte((uint8_t)(0xff&temp_SOC>>8), 0x02);
    SOC_Store_WriteByte((uint8_t)(0xff&temp_SOC), 0x03);
    return;
}


void bat_balance(){
    LTC68_ClearFIFO();
    
    uint8_t ic=0;
    uint8_t cell=0;
    uint8_t i=0;
    uint8_t temp_cfg[IC_PER_BUS * N_OF_BUSSES][6];
    uint16_t low_voltage = bat_pack.LO_voltage <= UNDER_VOLTAGE ? UNDER_VOLTAGE : bat_pack.LO_voltage;
    
    for (ic = 0; ic < (IC_PER_BUS * N_OF_BUSSES); ic++){
        for (i = 0; i < 6; i++){
            temp_cfg[ic][i] = tx_cfg[ic][i];
        }
    }
    
    for (ic = 0; ic < (IC_PER_BUS * N_OF_BUSSES); ic++){
        
        for (cell = 0; cell < 12; cell++){
            uint16_t diff = 0;
            if (cell_codes[ic][cell] / 10 > low_voltage)
                diff = cell_codes[ic][cell]/10 - low_voltage;
            
            // These two cases will set diff = 0 if there is an empty input on the 6811
            // Each 6811 can measure 12 voltages. We use 9/9/10 per slave.
            // First case is for chips that have 10 cells attached
            // Second case is for chips that have 9 cells attached
            if (ic == 2 || ic == 5 || ic == 8 || ic == 11 || ic == 14 || ic == 17) { // These ICs have different cells used
                if (!(CELL_ENABLE_HIGH & (0x1 << cell))){ // If not enabled
                    diff = 0;
                }    
            } else { // If not enabled
                if (!(CELL_ENABLE_LOW & (0x1 << cell))){
                    diff = 0;
                } 
            }
            
            // This is here to handle a bug which causes the first cell voltage to be 
            // wrong sometimes. I think I have fixed it, but I am putting this hear 
            // just in case
            if (cell_codes[ic][cell]/10 > 6000) {
                diff = 0;   
            }
            
            if (diff > 0 && diff > BALANCE_THRESHOLD){    
                // if this cell is BALANCE_THRESHOLD or more higher than the lowest cell
                if (cell < 8){
                    temp_cfg[ic][4] |= (0x1 << cell);
                }else{
                    temp_cfg[ic][5] |= (0x1 << (cell - 8));
                }
            }
            
        }
        
    }
    
    
    /*Test code!!!!*/
    //temp_cfg[9][4] = 0xFF;
    /* end purely test code */
    
    // discharge time has been set in void LTC6804_init_cfg() already. 0x2 = 1 min
    uint8_t cfga[18][6]; 
    
    Select6820_Write(0);
    wakeup_sleep(0);
    CyDelay(1);
    
    for (ic = 0; ic < 9; ic++) {
        LTC6804_wrcfga_balance(ic, temp_cfg[ic]);
        LTC6804_wrcfga_balance(ic, temp_cfg[ic]);
        
        CyDelay(50);
        
        LTC6804_rdcfga(ic, cfga[ic]);
        LTC6804_rdcfga(ic, cfga[ic]);
    }

    CyDelay(50);
    
    Select6820_Write(1);
    wakeup_sleep(1);
    CyDelay(1);
    
    for (ic = 9; ic < 18; ic++) {
        LTC6804_wrcfga_balance(ic, temp_cfg[ic]);
        LTC6804_wrcfga_balance(ic, temp_cfg[ic]);  
        CyDelay(50);
        LTC6804_rdcfga(ic, cfga[ic]);
        LTC6804_rdcfga(ic, cfga[ic]);
    }

    CyDelay(100);
}

void bat_clear_balance() {
    uint8_t cfg_data[6];
    for (int i = 0; i < 6; i++) {
        cfg_data[i] = 0x00;
    }
    
    LTC68_ClearFIFO();

    Select6820_Write(0);
    wakeup_sleep(0);
    CyDelay(1);
    
    
    LTC6804_wrcfga(0xFF, 0x00, cfg_data);
    LTC6804_wrcfga(0xFF, 0x00, cfg_data);
    
    Select6820_Write(1);
    wakeup_sleep(1);
    CyDelay(1);
    
    LTC6804_wrcfga(0xFF, 0x00, cfg_data);
    LTC6804_wrcfga(0xFF, 0x00, cfg_data);
    
}

