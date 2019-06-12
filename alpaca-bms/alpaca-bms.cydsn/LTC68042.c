/*!
	LTC6804-2 Multicell Battery Monitor
@verbatim	
	The LTC6804 is a 3rd generation multicell battery stack
	monitor that measures up to 12 series connected battery
	cells with a total measurement error of less than 1.2mV. The
	cell measurement range of 0V to 5V makes the LTC6804
	suitable for most battery chemistries. All 12 cell voltages
	can be captured in 290uS, and lower data acquisition rates
	can be selected for high noise reduction.
	
	Using the LTC6804-2, multiple devices are connected in
	parallel to the host processor, with each device 
	individually addressed.
@endverbatim	
REVISION HISTORY
$Revision: 1000 $
$Date: 2013-12-13 

Copyright (c) 2013, Linear Technology Corp.(LTC)
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

Copyright 2013 Linear Technology Corp. (LTC)
***********************************************************/

//! @defgroup LTC68042 LTC6804-2: Multicell Battery Monitor

/*! @file
    @ingroup LTC68042
    Library for LTC6804-2 Multicell Battery Monitor
*/

#include "LTC68042.h"
#include "project.h"
/*
   ADC control Variables for LTC6804
*/
 


/*!
  6804 conversion command variables.  
*/
uint8_t ADCV[2]; //!< Cell Voltage conversion command.
uint8_t ADAX[2]; //!< GPIO conversion command.


/*!
  \brief This function will initialize all 6804 variables and the SPI port.

  input: 
  ------
  IC: number of ICs being controlled. The address of the ICs in a LTC6804-2 network will start at 0 and continue in an ascending order.
*/
void LTC6804_initialize(uint8_t adc_mode)
{
  set_adc(adc_mode, DCP_DISABLED, CELL_CH_ALL, AUX_CH_GPIO1); // MD_FILTERED from MD_NORMAL
  LTC6804_init_cfg();
}

/*!******************************************************************************************************************
 \brief Maps  global ADC control variables to the appropriate control bytes for each of the different ADC commands
 
@param[in] uint8_t MD The adc conversion mode
@param[in] uint8_t DCP Controls if Discharge is permitted during cell conversions
@param[in] uint8_t CH Determines which cells are measured during an ADC conversion command
@param[in] uint8_t CHG Determines which GPIO channels are measured during Auxiliary conversion command
 
 Command Code: \n
			|command	|  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
			|-----------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
			|ADCV:	    |   0   |   1   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CH[2] | CH[1] | CH[0] | 
			|ADAX:	    |   1   |   0   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CHG[2]| CHG[1]| CHG[0]| 
 ******************************************************************************************************************/
void set_adc(uint8_t MD, //ADC Mode
			 uint8_t DCP, //Discharge Permit
			 uint8_t CH, //Cell Channels to be measured
			 uint8_t CHG //GPIO Channels to be measured
			 )
{
  uint8_t md_bits;
  
  md_bits = (MD & 0x02) >> 1;
  ADCV[0] = md_bits + 0x02;
  md_bits = (MD & 0x01) << 7;
  ADCV[1] =  md_bits + 0x60 + (DCP<<4) + CH;
 
  md_bits = (MD & 0x02) >> 1;
  ADAX[0] = md_bits + 0x04;
  md_bits = (MD & 0x01) << 7;
  ADAX[1] = md_bits + 0x60 + CHG ;

  // for ADOL (checking both adcs are the same). Get rid of this for real use.
  //ADCV[1] = ADCV[1] & 0x81;
  
}


/*!*********************************************************************************************
  \brief Starts cell voltage conversion
  
  Starts ADC conversions of the LTC6804 Cpin inputs.
  The type of ADC conversion done is set using the associated global variables:
 |Variable|Function                                      | 
 |--------|----------------------------------------------|
 | MD     | Determines the filter corner of the ADC      |
 | CH     | Determines which cell channels are converted |
 | DCP    | Determines if Discharge is Permitted	     |
  
***********************************************************************************************/
void LTC6804_adcv()
{

  uint8_t cmd[4];
  uint16_t temp_pec;
  
  //1
  cmd[0] = ADCV[0]; //ADCV
  cmd[1] = ADCV[1];
  
  //2
  temp_pec = pec15_calc(2, ADCV);
  cmd[2] = (uint8_t)(temp_pec >> 8);
  cmd[3] = (uint8_t)(temp_pec);
  
  //3
  
  
  //4
  CyDelay(1);
  spi_write_array(4,cmd);
  CyDelay(1);
}


/*
 * lt_addr - value from 0 - 17
 */
uint8_t addressify_cmd(uint8_t lt_addr, uint8_t cmd0)
{
    uint8_t addr = lt_addr % IC_PER_BUS;
    addr &= 0b00001111;
    addr <<= 3;
    uint8_t rval = cmd0 | addr;
    return rval;
}

/*
 * Broadcast write command -
 * select - the value that should be selected. -> greatest value = 7
 * orig_cfga_data - old register values we don't want to change
 */
void LTC6804_wrcfga(uint8_t lt_addr, uint8_t select, uint8_t orig_cfga_data[5])
{
    uint8_t cmd[12];
    uint16_t temp_pec;
    
    
    // see LTC6811 datasheet for command codes
    if (lt_addr == 0xFF) {
        cmd[0] = 0; // For global write
    }
    else {    
        cmd[0] = 128;     // For addressed write
        cmd[0] = addressify_cmd(lt_addr, cmd[0]);
    }
    cmd[1] = 1;     // specifies wrcfga cmd
    
    // calculate pec for command code

    temp_pec = pec15_calc(2, (uint8_t *)cmd);
    cmd[2] = (uint8_t)(temp_pec >> 8);
    cmd[3] = (uint8_t)(temp_pec);

    /*
    cfga is an 8 byte register on the LTC6811, we care about cfga[0]:
    bits -  |7     |6       |5        |4        |3     |2     |1      |0          |
            |gpio5 |gpio4   |gpio3    |gpio2    |gpio1 |refon | dten  |adcopt (important) |
            | x    |select2 | select1 | select 0|   1  | 1    | 1     | 0         |
    
    In testing, we found that dten and refon must be 1, otherwise the function doesn't
    write the select (gpio) bits.
    GPIO1 is the output of the mux and must be written high. 
    If GPIO1 is written low then read function will get microvolts.
    */
    
    uint8_t cfgr0 = (select << 5) >> 1; // 00000xxx -> 0xxx0000
    
    cmd[4] = cfgr0 | 0b1100;       // gpio1 = 1 refon = 1 dten = 1 adcopt = 0
    cmd[5] = orig_cfga_data[1];    // rest of the register is written with its prev. values
    cmd[6] = orig_cfga_data[2];
    cmd[7] = orig_cfga_data[3];
    cmd[8] = orig_cfga_data[4];
    cmd[9] = orig_cfga_data[5];

    // calculate pec on data
    temp_pec = pec15_calc(6, (uint8_t*)(cmd + 4));
    cmd[10] = (uint8_t)(temp_pec >> 8);
    cmd[11] = (uint8_t)(temp_pec);
    
    // wakeup device and send cmd
    wakeup_idle();
    spi_write_array(12, cmd);
}

/*
 *  Address write command for sending cell balance signals
 *  data[5] bytes 4 and 5 contain discharge time and cells to discharge
 *  lt_addr (0-17) corresponds to the address of an lt chip
*/

void LTC6804_wrcfga_balance(uint8_t lt_addr, uint8_t cfga_data[5]) {
    
    uint8_t cmd[12];
    uint16_t temp_pec;
    
    cmd[0] = 128;
    cmd[0] = addressify_cmd(lt_addr, cmd[0]);
    
    cmd[1] = 1;
    
    temp_pec = pec15_calc(2, (uint8_t*) cmd);
    cmd[2] = (uint8_t) (temp_pec >> 8);
    cmd[3] = (uint8_t) temp_pec;
    
    cmd[4] = 0x0E;
    cmd[5] = cfga_data[1];
    cmd[6] = cfga_data[2];
    cmd[7] = cfga_data[3];
    cmd[8] = cfga_data[4];
    cmd[9] = cfga_data[5] | 0x20;
    
    temp_pec = pec15_calc(6, (uint8_t*) cmd + 4);
    cmd[10] = (uint8_t) (temp_pec >> 8);
    cmd[11] = (uint8_t) temp_pec;
    
    wakeup_idle();
    spi_write_array(12, cmd);
}


/*
 * Address read command 
 * TODO: implement address parameter
 * cfga[6] - stores values read from the 6811
 */
int8_t LTC6804_rdcfga(uint8_t lt_addr, uint8_t cfga[6])
{
    uint8_t cmd[4];         // bytes for rdcfga cmd; sent to slaves
    int8_t pec_error;       // returned, indicates whether cmd failed or not
    uint16_t data_pec;      // stores our calculated pec
    uint16_t recieved_pec;  // stores 6811's calculated pec
    uint16_t cmd_pec;
    uint8_t rx_data[8];     // stores slave to master data (6 bytes data, 2 bytes pec)
   
    cmd[0] = 128;           // See pg. 55-63 of 6811 datasheet for cmd formats
    cmd[0] = addressify_cmd(lt_addr, cmd[0]);
    
    cmd[1] = 2;                                              
    cmd_pec = pec15_calc(2, cmd);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);
    
    wakeup_idle();
    spi_write_read(cmd, 4, rx_data, 8);
    
    recieved_pec = *(uint16_t *)(rx_data + 6);
    data_pec = pec15_calc(6, rx_data);
    
    if(recieved_pec != data_pec){
        pec_error = -1;
    }
    
    for(int i = 0; i < 6; i++){
        cfga[i] = rx_data[i];
    }
    
    return pec_error;
}

/*
 *  Read the voltage of a specific pin
 *  for reference:
 *  group 0 (auxa): set 0 = gpio1, set 1 = gpio2, set 2 = gpio2
 *  group 1 (auxb): set 0 = gpio4, set 1 = gpio5, set 2 = vref2
 *  group 2 (auxc): ?? look for in data sheet
 *  group 3 (auxd): ?? look for in data sheet
 */
int8_t LTC6804_rdaux_fe6(uint8_t lt_addr, enum AuxPins pin, uint16_t *aux)
{
    uint8_t cmd[4];
    uint16_t cmd_pec;
    uint8_t rx_data[8];
    int8_t pec_error;
    uint16_t received_pec;
    uint16_t data_pec;
    
    cmd[0] = 128;
    cmd[0] = addressify_cmd(lt_addr, cmd[0]);
    
    uint8_t group = pin / 3;
    uint8_t set = 2* (pin % 3);
    
    switch(group) {
        case 0: cmd[1] = 12; break;     // GPIO 1 2 3 
        case 1: cmd[1] = 14; break;     // GPIO 4 5 VRef
        case 2: cmd[1] = 13; break;     // Different Register
        case 3: cmd[1] = 15; break;     // Different Register
    }
    
    cmd_pec = pec15_calc(2, cmd);
    cmd[2] = (cmd_pec >> 8);
    cmd[3] = cmd_pec;
    
    int num_tries = 0;
    
    //do {
        spi_write_read(cmd, 4, rx_data, 8);
    /*    received_pec = (*(rx_data + 6) << 8) + *(rx_data + 7);
        data_pec = pec15_calc(6, rx_data);
        num_tries++;
        if (num_tries > 2) {
            num_tries = 0;
            *aux = 0xFFFF;
            return -1;
        }
    }while (data_pec != received_pec);*/
    *aux = rx_data[set] | (rx_data[set + 1] << 8);
    
    return 0;                   // Implement error path
}

int LTC6804_rdauxa(uint8_t lt_addr, uint8_t aux_data[6])
{
    uint8_t cmd[4];
    uint8_t rx_data[8];
    uint16_t cmd_pec;
    uint16_t received_pec;
    uint16_t data_pec;
    
    uint16_t gpio1_data;
    
    // set up read command
    cmd[0] = 128;
    
    //cmd[1] = 12; //auxa for gpio1
    cmd[1] = 14; //auxb for vref2
    
    cmd_pec = pec15_calc(2, cmd);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);
    
    // read 
    spi_write_read(cmd, 4, rx_data, 8);
    
    //CyDelay(20);
    gpio1_data = *(uint16_t *)rx_data;
    received_pec = *(uint16_t *)(rx_data + 6);
    data_pec = pec15_calc(6, rx_data);
    
    aux_data[0] = rx_data[0];
    aux_data[1] = rx_data[1];
    aux_data[2] = rx_data[2];
    aux_data[3] = rx_data[3];
    aux_data[4] = rx_data[4];
    aux_data[5] = rx_data[5];
    
    if(data_pec != received_pec) {
        return -1;
    }
    
    return 1; 
}

void LTC6804_adow(uint8_t pup)
{

  uint8_t cmd[4];
  uint16_t temp_pec;
  
  //1
  cmd[0] = ADCV[0];
  cmd[1] = (((ADCV[1] & 0xBF) | (pup << 6)) | (0x1 << 3)); 
  
  //2
  temp_pec = pec15_calc(2, ADCV);
  cmd[2] = (uint8_t)(temp_pec >> 8);
  cmd[3] = (uint8_t)(temp_pec);
  
  //3
  wakeup_idle (); //This will guarantee that the LTC6804 isoSPI port is awake. This command can be removed.
  
  //4
  spi_write_array(4,cmd);

}

/*
void LTC6804_cvst()
{
  uint8_t cmd[4];
  uint16_t temp_pec;
  
  //1
  cmd[0] = ADCV[0];
  cmd[1] = ADCV[1];
  cmd[1] = cmd[1] & 
  
  //2
  temp_pec = pec15_calc(2, ADCV);
  cmd[2] = (uint8_t)(temp_pec >> 8);
  cmd[3] = (uint8_t)(temp_pec);
  
  //3
  wakeup_idle (); //This will guarantee that the LTC6804 isoSPI port is awake. This command can be removed.
  
  //4
  spi_write_array(4,cmd);
}
*/
/*
  LTC6804_adcv Function sequence:
  
  1. Load adcv command into cmd array
  2. Calculate adcv cmd PEC and load pec into cmd array
  3. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
  4. send broadcast adcv command to LTC6804 stack
*/


/*!******************************************************************************************************
 \brief Start an GPIO Conversion
 
  Starts an ADC conversions of the LTC6804 GPIO inputs.
  The type of ADC conversion done is set using the associated global variables:
 |Variable|Function                                      | 
 |--------|----------------------------------------------|
 | MD     | Determines the filter corner of the ADC      |
 | CHG    | Determines which GPIO channels are converted |
 
*********************************************************************************************************/
void LTC6804_adax()
{
  uint8_t cmd[4];
  uint16_t temp_pec;
 
  cmd[0] = ADAX[0];
  cmd[1] = ADAX[1];
  temp_pec = pec15_calc(2, ADAX);
  cmd[2] = (uint8_t)(temp_pec >> 8);
  cmd[3] = (uint8_t)(temp_pec);
 
  wakeup_idle (); //This will guarantee that the LTC6804 isoSPI port is awake. This command can be removed.
  
  spi_write_array(4,cmd);

}



/*
  LTC6804_adax Function sequence:
  
  1. Load adax command into cmd array
  2. Calculate adax cmd PEC and load pec into cmd array
  3. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
  4. send broadcast adax command to LTC6804 stack
*/


/***********************************************//**
 \brief Reads and parses the LTC6804 cell voltage registers.
 
 The function is used to read the cell codes of the LTC6804.
 This function will send the requested read commands parse the data
 and store the cell voltages in cell_codes variable. 
 
 
@param[in] uint8_t reg; This controls which cell voltage register is read back. 
 
          0: Read back all Cell registers 
		  
          1: Read back cell group A 
		  
          2: Read back cell group B 
		  
          3: Read back cell group C 
		  
          4: Read back cell group D 
 
@param[in] uint8_t total_ic; This is the number of ICs in the network
 

@param[out] uint16_t cell_codes[]; An array of the parsed cell codes from lowest to highest. The cell codes will
  be stored in the cell_codes[] array in the following format:
  |  cell_codes[0]| cell_codes[1] |  cell_codes[2]|    .....     |  cell_codes[11]|  cell_codes[12]| cell_codes[13] |  .....   |
  |---------------|----------------|--------------|--------------|----------------|----------------|----------------|----------|
  |IC1 Cell 1     |IC1 Cell 2      |IC1 Cell 3    |    .....     |  IC1 Cell 12   |IC2 Cell 1      |IC2 Cell 2      | .....    |
 
 @return int8_t, PEC Status.
 
	0: No PEC error detected
  
	-1: PEC error detected, retry read
 *************************************************/
uint8_t LTC6804_rdcv(uint8_t reg,
					 uint8_t total_ic,
					 uint16_t cell_codes[][12]
					 )
{
  //bytes to recieve
  const uint8_t NUM_RX_BYT = 8;
  // bytes in a 6811 reg
  const uint8_t BYT_IN_REG = 6;
  // each cell gets 2 bytes in a reg
  const uint8_t CELL_IN_REG = 3;
  
  int8_t pec_error = 0;
  uint16_t parsed_cell;
  uint16_t received_pec;
  uint16_t data_pec;
  uint8_t data_counter=0; //data counter
  uint8_t cell_data[NUM_RX_BYT * total_ic];

  //1.a
  if (reg == 0)
  {
    //a.i
    for(uint8_t cell_reg = 1; cell_reg<5; cell_reg++)         			 //executes once for each of the LTC6804 cell voltage registers
    {
      data_counter = 0;
      //LTC6804_rdcv_reg(cell_reg, total_ic,cell_data);
    
      for (int ic_num = 0; ic_num < total_ic; ic_num++) {
         LTC6804_rdcv_FE6(cell_reg, cell_data, ic_num);
         LTC6804_rdcv_FE6(cell_reg, cell_data, ic_num);
      }
    
      if (cell_reg == 1) {
          for (int ic_num = 0; ic_num < total_ic; ic_num += 3) {
             LTC6804_rdcv_FE6(cell_reg, cell_data, ic_num);
             CyDelay(1);
             LTC6804_rdcv_FE6(cell_reg, cell_data, ic_num);
             CyDelay(1);
          }
      }
    
      for (uint8_t current_ic = 0 ; current_ic < total_ic; current_ic++) // executes for every LTC6804 in the stack
      {																 	  // current_ic is used as an IC counter
        //a.ii
		for(uint8_t current_cell = 0; current_cell<CELL_IN_REG; current_cell++)	// This loop parses the read back data. Loops 
        {														   		  // once for each cell voltages in the register 
          parsed_cell = cell_data[data_counter] + (cell_data[data_counter + 1] << 8);
          cell_codes[current_ic][current_cell  + ((cell_reg - 1) * CELL_IN_REG)] = parsed_cell;
          data_counter = data_counter + 2;
        }
		//a.iii
        received_pec = (cell_data[data_counter] << 8) + cell_data[data_counter+1];
        data_pec = pec15_calc(BYT_IN_REG, &cell_data[current_ic * NUM_RX_BYT ]);
        if(received_pec != data_pec)
        {
          pec_error--;//pec_error = -1;
        }
        data_counter=data_counter+2;
      }
    }
  }
 //1.b
  else
  {
	//b.i
	
    LTC6804_rdcv_reg(reg, total_ic,cell_data);
    for (uint8_t current_ic = 0 ; current_ic < total_ic; current_ic++) // executes for every LTC6804 in the stack
    {							   									// current_ic is used as an IC counter
		//b.ii
		for(uint8_t current_cell = 0; current_cell < CELL_IN_REG; current_cell++)   									// This loop parses the read back data. Loops 
		{						   									// once for each cell voltage in the register 
			parsed_cell = cell_data[data_counter] + (cell_data[data_counter+1]<<8);
			cell_codes[current_ic][current_cell + ((reg - 1) * CELL_IN_REG)] = 0x0000FFFF & parsed_cell;
			data_counter= data_counter + 2;
		}
		//b.iii
	    received_pec = (cell_data[data_counter] << 8 )+ cell_data[data_counter + 1];
        data_pec = pec15_calc(BYT_IN_REG, &cell_data[current_ic * NUM_RX_BYT * (reg-1)]);
		if(received_pec != data_pec)
		{
			pec_error--;//pec_error = -1;
		}
	}
  }

    CyDelay(1);
 //2
return(pec_error);
}
/*
	LTC6804_rdcv Sequence
	
	1. Switch Statement:
		a. Reg = 0
			i. Read cell voltage registers A-D for every IC in the stack
			ii. Parse raw cell voltage data in cell_codes array
			iii. Check the PEC of the data read back vs the calculated PEC for each read register command
		b. Reg != 0 
			i.Read single cell voltage register for all ICs in stack
			ii. Parse raw cell voltage data in cell_codes array
			iii. Check the PEC of the data read back vs the calculated PEC for each read register command
	2. Return pec_error flag
*/


/***********************************************//**
 \brief Read the raw data from the LTC6804 cell voltage register
 
 The function reads a single cell voltage register and stores the read data
 in the *data point as a byte array. This function is rarely used outside of 
 the LTC6804_rdcv() command. 
 
 @param[in] uint8_t reg; This controls which cell voltage register is read back. 
         
          1: Read back cell group A 
		  
          2: Read back cell group B 
		  
          3: Read back cell group C 
		  
          4: Read back cell group D 
		  
 @param[in] uint8_t total_ic; This is the number of ICs in the network
 
 @param[out] uint8_t *data; An array of the unparsed cell codes 
 *************************************************/
void LTC6804_rdcv_reg(uint8_t reg,
					  uint8_t total_ic, 
					  uint8_t *data
					  )
{
  uint8_t cmd[4];
  uint16_t temp_pec;
  
  //1
  if (reg == 1)
  {
    cmd[1] = 0x04;
    cmd[0] = 0x00;
  }
  else if(reg == 2)
  {
    cmd[1] = 0x06;
    cmd[0] = 0x00;
  } 
  else if(reg == 3)
  {
    cmd[1] = 0x08;
    cmd[0] = 0x00;
  } 
  else if(reg == 4)
  {
    cmd[1] = 0x0A;
    cmd[0] = 0x00;
  } 

  //2
 
  
  //3
  wakeup_idle (); //This will guarantee that the LTC6804 isoSPI port is awake. This command can be removed.
  
  //4
  for(int current_ic = 0; current_ic<total_ic; current_ic++)
  {
	cmd[0] = 0x80 + (current_ic<<3); //Setting address
    temp_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(temp_pec >> 8);
	cmd[3] = (uint8_t)(temp_pec); 
	CyDelay(1);
    CS_Write(0);
	spi_write_read(cmd,4,&data[current_ic*8],8);
	CS_Write(1);
    CyDelay(1);
  }
}

/*
  LTC6804_rdcv_reg Function Process:
  1. Determine Command and initialize command array
  2. Calculate Command PEC
  3. Wake up isoSPI, this step is optional
  4. Send Global Command to LTC6804 stack
*/


/***********************************************************************************//**
 \brief Reads and parses the LTC6804 auxiliary registers.
 
 The function is used
 to read the  parsed GPIO codes of the LTC6804. This function will send the requested 
 read commands parse the data and store the gpio voltages in aux_codes variable 
 
 @param[in] uint8_t reg; This controls which GPIO voltage register is read back. 
		  
          0: Read back all auxiliary registers 
		  
          1: Read back auxiliary group A 
		  
          2: Read back auxiliary group B 
		  
 
 @param[in] uint8_t total_ic; This is the number of ICs in the network

 @param[out] uint8_t aux_codes[]; An array of the aux codes from lowest to highest. The GPIO codes will
 be stored in the aux_codes[] array in the following format:
 |  aux_codes[0]| aux_codes[1] |  aux_codes[2]|  aux_codes[3]|  aux_codes[4]|  aux_codes[5]| aux_codes[6] |aux_codes[7]|  .....    |
 |--------------|--------------|--------------|--------------|--------------|--------------|--------------|------------|-----------|
 |IC1 GPIO1     |IC1 GPIO2     |IC1 GPIO3     |IC1 GPIO4     |IC1 GPIO5     |IC1 Vref2     |IC2 GPIO1     |IC2 GPIO2   |  .....    |
 
 
 @return int8_t, PEC Status.
 
	0: No PEC error detected
  
	-1: PEC error detected, retry read
 *************************************************/

void LTC6804_rdcv_FE6(uint8_t reg, uint8_t * data, int ic_num){
    uint8_t cmd[4];
    uint16_t temp_pec;
    uint16_t data_pec;
    uint16_t received_pec;

    if (reg == 1) {
        cmd[1] = 0x04;
        cmd[0] = 0x00;
    }
    else if (reg == 2) {
        cmd[1] = 0x06;
        cmd[0] = 0x00;
    }
    else if (reg == 3) {
        cmd[1] = 0x08;
        cmd[0] = 0x00;
    }
    else if (reg == 4) {
        cmd[1] = 0x0A;
        cmd[0] = 0x00;
    }

    cmd[0] = 0x80 + (ic_num << 3);

    temp_pec = pec15_calc(2, cmd);
    cmd[2] = (uint8_t)(temp_pec >> 8);
    cmd[3] = (uint8_t)(temp_pec);

    wakeup_idle();
    
    int num_tries = 0;
    int try_again = 0;
    
    
    do {
        spi_write_read(cmd, 4, &data[ic_num*8], 8);
        try_again = 0;
        
        // check for missing voltages in reg (6553's)
        for (int i = 0; i < 3; i+= 2) {
            if (data[ic_num*8 + i] == 0xFF && data[ic_num*8 + i + 1] == 0xFF)
                try_again = 1;
        }
        // check for pec Errors
        received_pec = (*(&data[ic_num*8] + 6) << 8) + *(&data[ic_num*8] + 7);
        data_pec = pec15_calc(6, &data[ic_num*8]);
        num_tries++;
        if (num_tries > 2)
            return;
        
    } while ((data_pec != received_pec) || try_again);
}
int8_t LTC6804_rdaux(uint8_t reg,
					 uint8_t total_ic, 
					 uint16_t aux_codes[][6]
					 )
{


  const uint8_t NUM_RX_BYT = 8;
  const uint8_t BYT_IN_REG = 6;
  const uint8_t GPIO_IN_REG = 3;
  
  uint8_t data_counter = 0; 
  int8_t pec_error = 0;
  uint16_t received_pec;
  uint16_t data_pec;
  uint8_t data[NUM_RX_BYT * total_ic];
  //1.a
  if (reg == 0)
  {
	//a.i
    for(uint8_t gpio_reg = 1; gpio_reg<3; gpio_reg++)		 	   		 //executes once for each of the LTC6804 aux voltage registers
    {
      data_counter = 0;
      LTC6804_rdaux_reg(gpio_reg, total_ic,data);
      for (uint8_t current_ic = 0 ; current_ic < total_ic; current_ic++) // This loop executes once for each LTC6804
      {									  								 // current_ic is used as an IC counter
        //a.ii
		for(uint8_t current_gpio = 0; current_gpio< GPIO_IN_REG; current_gpio++)	// This loop parses GPIO voltages stored in the register
        {								   													
          
          aux_codes[current_ic][current_gpio +((gpio_reg-1)*GPIO_IN_REG)] = data[data_counter] + (data[data_counter+1]<<8);
          data_counter=data_counter+2;
		  
        }
		//a.iii
        received_pec = (data[data_counter]<<8)+ data[data_counter+1];
        data_pec = pec15_calc(BYT_IN_REG, &data[current_ic*NUM_RX_BYT*(gpio_reg-1)]);
        if(received_pec != data_pec)
        {
          pec_error = -1;
        }
       
        data_counter=data_counter+2;
      }
   

    }
  
  }
  else
  {
	//b.i
    LTC6804_rdaux_reg(reg, total_ic, data);
    for (int current_ic = 0 ; current_ic < total_ic; current_ic++) // executes for every LTC6804 in the stack
    {							   // current_ic is used as an IC counter
		//b.ii
		for(int current_gpio = 0; current_gpio<GPIO_IN_REG; current_gpio++)   // This loop parses the read back data. Loops 
		{						   // once for each aux voltage in the register 
			aux_codes[current_ic][current_gpio +((reg-1)*GPIO_IN_REG)] = 0x0000FFFF & (data[data_counter] + (data[data_counter+1]<<8));
			data_counter=data_counter+2;
		}
		//b.iii
		received_pec = (data[data_counter]<<8) + data[data_counter+1];
        data_pec = pec15_calc(6, &data[current_ic*8*(reg-1)]);
        if(received_pec != data_pec)
        {
          pec_error = -1;
        }
	}
  }
  return (pec_error);
}
/*
	LTC6804_rdaux Sequence
	
	1. Switch Statement:
		a. Reg = 0
			i. Read GPIO voltage registers A-D for every IC in the stack
			ii. Parse raw GPIO voltage data in cell_codes array
			iii. Check the PEC of the data read back vs the calculated PEC for each read register command
		b. Reg != 0 
			i.Read single GPIO voltage register for all ICs in stack
			ii. Parse raw GPIO voltage data in cell_codes array
			iii. Check the PEC of the data read back vs the calculated PEC for each read register command
	2. Return pec_error flag
*/


/***********************************************//**
 \brief Read the raw data from the LTC6804 auxiliary register
 
 The function reads a single GPIO voltage register and stores thre read data
 in the *data point as a byte array. This function is rarely used outside of 
 the LTC6804_rdaux() command. 
 
 @param[in] uint8_t reg; This controls which GPIO voltage register is read back. 
		  
          1: Read back auxiliary group A
		  
          2: Read back auxiliary group B 

         
 @param[in] uint8_t total_ic; This is the number of ICs in the stack
 
 @param[out] uint8_t *data; An array of the unparsed aux codes 
 *************************************************/
void LTC6804_rdaux_reg(uint8_t reg, 
					   uint8_t total_ic,
					   uint8_t *data
					   )
{
  uint8_t cmd[4];
  uint16_t cmd_pec;
  
  //1
  if (reg == 1)
  {
    cmd[1] = 0x0C;
    cmd[0] = 0x00;
  }
  else if(reg == 2)
  {
    cmd[1] = 0x0e;
    cmd[0] = 0x00;
  } 
  else
  {
     cmd[1] = 0x0C;
     cmd[0] = 0x00;
  }
  //2
  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);
  
  //3
  wakeup_idle (); //This will guarantee that the LTC6804 isoSPI port is awake, this command can be removed.
  //4
   for(int current_ic = 0; current_ic<total_ic; current_ic++)
  {
	cmd[0] = 0x80 + (current_ic<<3); //Setting address
    cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec); 
	spi_write_read(cmd,4,&data[current_ic*8],8);
  }
}
/*
  LTC6804_rdaux_reg Function Process:
  1. Determine Command and initialize command array
  2. Calculate Command PEC
  3. Wake up isoSPI, this step is optional
  4. Send Global Command to LTC6804 stack
*/

/********************************************************//**
 \brief Clears the LTC6804 cell voltage registers
 
 The command clears the cell voltage registers and intiallizes 
 all values to 1. The register will read back hexadecimal 0xFF
 after the command is sent.
************************************************************/
void LTC6804_clrcell()
{
  uint8_t cmd[4];
  uint16_t cmd_pec;
  
  //1
  cmd[0] = 0x07;
  cmd[1] = 0x11;
  
  //2
  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec );
  
  //3
  wakeup_idle (); //This will guarantee that the LTC6804 isoSPI port is awake. This command can be removed.
  
  //4
  spi_write_read(cmd,4,0,0);
}
/*
  LTC6804_clrcell Function sequence:
  
  1. Load clrcell command into cmd array
  2. Calculate clrcell cmd PEC and load pec into cmd array
  3. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
  4. send broadcast clrcell command to LTC6804 stack
*/


/***********************************************************//**
 \brief Clears the LTC6804 Auxiliary registers
 
 The command clears the Auxiliary registers and intiallizes 
 all values to 1. The register will read back hexadecimal 0xFF
 after the command is sent.
***************************************************************/
void LTC6804_clraux()
{
  uint8_t cmd[4];
  uint16_t cmd_pec;
  
  //1
  cmd[0] = 0x07;
  cmd[1] = 0x12;
  
  //2
  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);
  
  //3
  wakeup_idle (); //This will guarantee that the LTC6804 isoSPI port is awake.This command can be removed.
  //4
  spi_write_read(cmd,4,0,0);
}
/*
  LTC6804_clraux Function sequence:
  
  1. Load clraux command into cmd array
  2. Calculate clraux cmd PEC and load pec into cmd array
  3. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
  4. send broadcast clraux command to LTC6804 stack
*/


/*****************************************************//**
 \brief Write the LTC6804 configuration register 
 
 This command will write the configuration registers of the stacks 
 connected in a stack stack. The configuration is written in descending 
 order so the last device's configuration is written first.
 

@param[in] uint8_t total_ic; The number of ICs being written. 
 
@param[in] uint8_t *config an array of the configuration data that will be written, the array should contain the 6 bytes for each
 IC in the stack. The lowest IC in the stack should be the first 6 byte block in the array. The array should
 have the following format:
 |  config[0]| config[1] |  config[2]|  config[3]|  config[4]|  config[5]| config[6] |  config[7]|  config[8]|  .....    |
 |-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|
 |IC1 CFGR0  |IC1 CFGR1  |IC1 CFGR2  |IC1 CFGR3  |IC1 CFGR4  |IC1 CFGR5  |IC2 CFGR0  |IC2 CFGR1  | IC2 CFGR2 |  .....    |
 
 The function will calculate the needed PEC codes for the write data
 and then transmit data to the ICs on a stack.
********************************************************/
void LTC6804_wrcfg(uint8_t total_ic,uint8_t config[][6])
{
  const uint8_t BYTES_IN_REG = 6;
  const uint8_t CMD_LEN = 4+(8*total_ic);
  uint16_t temp_pec;
  uint8_t cmd_index; //command counter
  
  uint8_t cmd[CMD_LEN];
  //1
  cmd[0] = 0x00;
  cmd[1] = 0x01;
  cmd[2] = 0x3d;
  cmd[3] = 0x6e;
  
  //2
  cmd_index = 4;
  for (uint8_t current_ic = 0; current_ic < total_ic; current_ic++) 			// executes for each LTC6804 in stack, 
  {								
    for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++) // executes for each byte in the CFGR register
    {							// i is the byte counter
	
      cmd[cmd_index] = config[current_ic][current_byte]; 		//adding the config data to the array to be sent 
      cmd_index = cmd_index + 1;                
    }
	//3
    temp_pec = (uint16_t)pec15_calc(BYTES_IN_REG, &config[current_ic][0]);// calculating the PEC for each board
    cmd[cmd_index] = (uint8_t)(temp_pec >> 8);
    cmd[cmd_index + 1] = (uint8_t)temp_pec;
    cmd_index = cmd_index + 2;
  }
  
  //4
  wakeup_idle (); 															 //This will guarantee that the LTC6804 isoSPI port is awake.This command can be removed.
  //5
    int fake_ic = 13;
    uint8_t full_cmd[12];
	cmd[0] = 0x80 + (fake_ic<<3); //Setting address
    temp_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(temp_pec >> 8);
	cmd[3] = (uint8_t)(temp_pec); 
    
    memcpy(full_cmd, cmd, 4);
    memcpy(full_cmd + 4, &cmd[4+(8*fake_ic)], 8);
	spi_write_array(12, full_cmd);
    
   for(int current_ic = 0; current_ic < total_ic; current_ic++)
  {
    uint8_t full_cmd[12];
	cmd[0] = 0x80 + (current_ic<<3); //Setting address
    temp_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(temp_pec >> 8);
	cmd[3] = (uint8_t)(temp_pec); 
    
    memcpy(full_cmd, cmd, 4);
    memcpy(full_cmd + 4, &cmd[4+(8*current_ic)], 8);
	spi_write_array(12, full_cmd);
    //spi_write_array(4,cmd);
	//spi_write_array(8,&cmd[4+(8*current_ic)]);
  }
}
/*
	1. Load cmd array with the write configuration command and PEC
	2. Load the cmd with LTC6804 configuration data
	3. Calculate the pec for the LTC6804 configuration data being transmitted
	4. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
	5. Write configuration of each LTC6804 on the stack

*/

/*!******************************************************
 \brief Reads configuration registers of a LTC6804 stack
 



@param[in] uint8_t total_ic: number of ICs in the stack

@param[out] uint8_t *r_config: array that the function will write configuration data to. The configuration data for each IC 
is stored in blocks of 8 bytes with the configuration data of the lowest IC on the stack in the first 8 bytes 
of the array, the second IC in the second 8 byte etc. Below is an table illustrating the array organization:

|r_config[0]|r_config[1]|r_config[2]|r_config[3]|r_config[4]|r_config[5]|r_config[6]  |r_config[7] |r_config[8]|r_config[9]|  .....    |
|-----------|-----------|-----------|-----------|-----------|-----------|-------------|------------|-----------|-----------|-----------|
|IC1 CFGR0  |IC1 CFGR1  |IC1 CFGR2  |IC1 CFGR3  |IC1 CFGR4  |IC1 CFGR5  |IC1 PEC High |IC1 PEC Low |IC2 CFGR0  |IC2 CFGR1  |  .....    |


@return int8_t PEC Status.
	0: Data read back has matching PEC
 
	-1: Data read back has incorrect PEC
********************************************************/
int8_t LTC6804_rdcfg(uint8_t total_ic, uint8_t r_config[][8])
{
  const uint8_t BYTES_IN_REG = 8;
  
  uint8_t cmd[4];
  int8_t pec_error = 0; 
  uint16_t data_pec;
  uint16_t received_pec;
  uint8_t rx_data[8*total_ic];
  //1
  cmd[0] = 0x00;
  cmd[1] = 0x02;
  cmd[2] = 0x2b; // PEC
  cmd[3] = 0x0A; // PEC
 
  //2
  wakeup_idle (); //This will guarantee that the LTC6804 isoSPI port is awake. This command can be removed.
  //3
   for(int current_ic = 0; current_ic<total_ic; current_ic++)
  {
	cmd[0] = 0x80 + (current_ic<<3); //Setting address bits in cmd
    data_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(data_pec >> 8); // PEC changes since address changed
	cmd[3] = (uint8_t)(data_pec); 
	spi_write_read(cmd,4,&rx_data[current_ic*8],8);
  }
 
  for (uint8_t current_ic = 0; current_ic < total_ic; current_ic++) //executes for each LTC6804 in the stack
  { 
    //4.a
    for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)
    {
      r_config[current_ic][current_byte] = rx_data[current_byte + (current_ic*BYTES_IN_REG)];
    }
    //4.b
    received_pec = (r_config[current_ic][6]<<8) + r_config[current_ic][7];
    data_pec = pec15_calc(6, &r_config[current_ic][0]);
    if(received_pec != data_pec)
    {
      pec_error = -1;
    }  
  }
  //5
  return(pec_error);
}
/*
	1. Load cmd array with the write configuration command and PEC
	2. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
	3. read configuration of each LTC6804 on the stack
	4. For each LTC6804 in the stack
	  a. load configuration data into r_config array
	  b. calculate PEC of received data and compare against calculated PEC
	5. Return PEC Error

*/

/*!****************************************************
  \brief Wake isoSPI up from idle state
 Generic wakeup commannd to wake isoSPI up out of idle
 *****************************************************/
void wakeup_idle()
{
  /*
  output_low(LTC6804_CS);
  delayMicroseconds(10); //Guarantees the isoSPI will be in ready mode
  output_high(LTC6804_CS);
  */
  // Just commented out in FE3BMS, maybe need to write a dummy byte?
    
  // FIXME
  
}

/*!****************************************************
  \brief Wake the LTC6804 from the sleep state
  
 Generic wakeup commannd to wake the LTC6804 from sleep
 *****************************************************/
void wakeup_sleep(int bus)
{
    Select6820_Write(bus);
    CyDelay(1);
    LTC68_WriteTxData(0x4D);  //write dummy byte to wake up (ascii 'M')
    CyDelay(1);
    while(! (LTC68_ReadTxStatus() & LTC68_STS_SPI_DONE)){}
    LTC68_ReadRxData();
    CyDelayUs(WAKE_UP_DELAY_US);
}
/*!**********************************************************
 \brief calaculates  and returns the CRC15
  

@param[in]  uint8_t len: the length of the data array being passed to the function
               
@param[in]  uint8_t data[] : the array of data that the PEC will be generated from
  

@return  The calculated pec15 as an unsigned int16_t
***********************************************************/
uint16_t pec15_calc(uint8_t len, uint8_t *data)
{
	uint16_t remainder,addr;
	
	remainder = 16;//initialize the PEC
	for(uint8_t i = 0; i<len;i++) // loops for each byte in data array
	{
		addr = ((remainder>>7)^data[i])&0xff;//calculate PEC table address 
		remainder = (remainder<<8)^crc15Table[addr];
	}
	return(remainder*2);//The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
}


/*!
 \brief Writes an array of bytes out of the SPI port
 
 @param[in] uint8_t len length of the data array being written on the SPI port
 @param[in] uint8_t data[] the data array to be written on the SPI port
 
*/
void spi_write_array(uint8_t len, // Option: Number of bytes to be written on the SPI port
					 uint8_t data[] //Array of bytes to be written on the SPI port
					 )
{ // SKY_ADDED
  CyDelay(1);
  for(uint8_t i = 0; i < len; i++)
  {
     LTC68_WriteTxData((int8_t)data[i]);
  }
  CyDelay(1);
}
/*!
 \brief Writes and read a set number of bytes using the SPI port.

@param[in] uint8_t tx_data[] array of data to be written on the SPI port
@param[in] uint8_t tx_len length of the tx_data array
@param[out] uint8_t rx_data array that read data will be written too. 
@param[in] uint8_t rx_len number of bytes to be read from the SPI port.

*/

void spi_write_read(uint8_t tx_Data[],//array of data to be written on SPI port 
					uint8_t tx_len, //length of the tx data arry
					uint8_t *rx_data,//Input: array that will store the data read by the SPI port
					uint8_t rx_len //Option: number of bytes to be read from the SPI port
					)
{
    LTC68_ClearRxBuffer();
   
    
    uint8_t i = 0;
    uint8_t dummy_read;
    for(i = 0; i < tx_len; i++)
  {

   LTC68_WriteTxData(tx_Data[i]);
    }
   

   for(i = 0; i < rx_len; i++)
  {

  LTC68_WriteTxData(0x00);
    }
     

    CyDelay(1);
//    while(dummy_read!=rx_len+tx_len){
//    dummy_read=(uint8_t)LTC68_GetRxBufferSize();
//    };
    while(! (LTC68_ReadTxStatus() & LTC68_STS_SPI_DONE)){
    }


    for(i = 0; i < tx_len; i++)
    {
        dummy_read = (uint8_t)LTC68_ReadRxData();
    }

    for(i = 0; i < rx_len; i++)
    {
        rx_data[i] = (uint8_t)LTC68_ReadRxData();

    }



CyDelayUs(200);
}

void my_spi_write_read(uint8_t tx_Data[],//array of data to be written on SPI port 
					uint8_t tx_len, //length of the tx data arry
					uint8_t *rx_data,//Input: array that will store the data read by the SPI port
					uint8_t rx_len //Option: number of bytes to be read from the SPI port
					)
{
    LTC68_ClearRxBuffer();
    uint8_t i = 0;
    uint8_t dummy_read;
    for(i = 0; i < tx_len; i++)
  {

   LTC68_WriteTxData(tx_Data[i]);
    CyDelay(1);
    }
   

   for(i = 0; i < rx_len; i++)
  {
    //CyDelay(1);
  LTC68_WriteTxData(0x00);
    
    }
     

    CyDelay(1);
//    while(dummy_read!=rx_len+tx_len){
//    dummy_read=(uint8_t)LTC68_GetRxBufferSize();
//    };
    while(! (LTC68_ReadTxStatus() & LTC68_STS_SPI_DONE)){
    }


    for(i = 0; i < tx_len; i++)
    {
        dummy_read = (uint8_t)LTC68_ReadRxData();
    }

    for(i = 0; i < rx_len; i++)
    {
        rx_data[i] = (uint8_t)LTC68_ReadRxData();
    }



CyDelayUs(200);
CyDelay(10);
}

void LTC6804_init_cfg()
{
  uint8_t i = 0;
  for(i = 0; i<IC_PER_BUS;i++)
  {
    tx_cfg[i][0] = 0xFE; // See notes and page 59 for reasoning.
    tx_cfg[i][1] = 0x00; 
    tx_cfg[i][2] = 0x00;
    tx_cfg[i][3] = 0x00; 
    tx_cfg[i][4] = 0x00;
    tx_cfg[i][5] = 0x20; // DCTO=0x2 1 min
  }
 
}


