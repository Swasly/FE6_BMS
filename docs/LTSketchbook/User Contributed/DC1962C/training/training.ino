/*
Linear Technology DC1962C Demonstration Board
LTC3880, LTC2974, LTC2977: Power Management Solution for Application Processors
Setups for training.

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
#define LTC2974_I2C_ADDRESS 0x32
#define LTC2977_I2C_ADDRESS 0x33

// Global variables
static uint8_t ltc3880_i2c_address;
static uint8_t ltc2974_i2c_address;
static uint8_t ltc2977_i2c_address;
static LT_SMBus *smbus = new LT_SMBusPec();
static LT_PMBus *pmbus = new LT_PMBus(smbus);

//! Initialize Linduino
void setup()
{
  Serial.begin(115200);         //! Initialize the serial port to the PC
  print_title();
  ltc3880_i2c_address = LTC3880_I2C_ADDRESS;
  ltc2974_i2c_address = LTC2974_I2C_ADDRESS;
  ltc2977_i2c_address = LTC2977_I2C_ADDRESS;
  print_prompt();
}

//! Repeats Linduino loop
void loop()
{
  uint8_t user_command;
  uint8_t model[8];
  uint8_t *addresses = NULL;
  float v1, v2;
  uint8_t config;
  uint16_t config2;
  
 if (Serial.available())                          //! Checks for user input
  {
    user_command = read_int();                     //! Reads the user command
    if (user_command != 'm')
      Serial.println(user_command);

    switch (user_command)                          //! Prints the appropriate submenu
    {
      case 1:
        pmbus->setPage(ltc3880_i2c_address, 0x00);
        v1 = pmbus->readVout(ltc3880_i2c_address, false);
        pmbus->setVoutUvFaultLimit(ltc3880_i2c_address, 1.05 * v1);
        break;
      case 2:
        pmbus->setPage(ltc2974_i2c_address, 0x03);
        v1 = pmbus->readVout(ltc2974_i2c_address, false);
        pmbus->setVout(ltc2974_i2c_address, 1.15 * v1);
        break;
      case 3:
        pmbus->setVinUvFaultLimit(ltc2977_i2c_address, 13.0);
        break;  
      case 4:
        smbus->writeByte(ltc2974_i2c_address, 0xFF, 0x00);
        break;
      case 5:
        pmbus->setPage(ltc2977_i2c_address, 0x06);
        v1 = pmbus->readVout(ltc2977_i2c_address, false);
        pmbus->setVoutMax(ltc2977_i2c_address, 0.5 * v1);
        break;
      case 6:
        pmbus->setPage(ltc3880_i2c_address, 0x01);
        pmbus->setTonRise(ltc3880_i2c_address, 3000.0);
        break;
      case 7:
        pmbus->setPage(ltc3880_i2c_address, 0x01);
        pmbus->setUtFaultLimit(ltc3880_i2c_address, 50.0);  
        break;        
      case 8:
        pmbus->setPage(ltc3880_i2c_address, 0x00);
        pmbus->setTonRise(ltc3880_i2c_address, 100.0);          
        pmbus->setTonMaxFaultLimit(ltc3880_i2c_address, 60.0); 
        pmbus->setVoutUvFaultLimit(ltc3880_i2c_address, 0.6);
        pmbus->sequenceOffGlobal();   
        delay(200);      
        pmbus->sequenceOnGlobal();         
        break;
      case 9:
        smbus->writeByte(ltc3880_i2c_address, WRITE_PROTECT, 0x80);
        pmbus->setVout(ltc3880_i2c_address, 0.8);
        break;
      case 10:
        pmbus->setPage(ltc3880_i2c_address, 0x00);
        pmbus->setTemp1Gain(ltc3880_i2c_address, 0x4CCD);
        break;
      case 11:
        pmbus->setPage(ltc2974_i2c_address, 0x03);
        config2 = smbus->readWord(ltc2974_i2c_address, MFR_CONFIG_LTC2974);
        config2 |= 1;
        smbus->writeWord(ltc2974_i2c_address, MFR_CONFIG_LTC2974, config2);
        break;
      case 12:
        pmbus->setMfrWatchdogFirst(ltc2977_i2c_address, 1.0);
        pmbus->setMfrWatchdog(ltc2977_i2c_address, 1.0);
        break;
      case 13:
  	  pmbus->enablePec(ltc3880_i2c_address); 
  	  pmbus->enablePec(ltc2974_i2c_address); 
  	  pmbus->enablePec(ltc2977_i2c_address);
          delete smbus;
          delete pmbus;
  	  smbus = new LT_SMBusPec();
  	  pmbus = new LT_PMBus(smbus);
  	  break;
      case 14:
  	  pmbus->disablePec(ltc3880_i2c_address);  
  	  pmbus->disablePec(ltc2974_i2c_address);  
  	  pmbus->disablePec(ltc2977_i2c_address);  
          delete smbus;
          delete pmbus;
  	  smbus = new LT_SMBusNoPec();
  	  pmbus = new LT_PMBus(smbus);
  	  break;
      case 15:
        addresses = smbus->probe(0);
        while(*addresses != 0)
        {
          Serial.print("ADDR 0x");
          Serial.println(*addresses++, HEX);
        }
        break;
      case 16 :
        smbus->writeByte(ltc3880_i2c_address, WRITE_PROTECT, 0x00);
        pmbus->restoreFromNvmGlobal();
        delay(2000);
        pmbus->resetGlobal();
        //delay(2000);
        //pmbus->clearFaultsGlobal();
        //delay(10);
        //pmbus->immediateOffGlobal();
        //delay(50);
        //pmbus->sequenceOnGlobal();
        break;
      default:
        Serial.println("Incorrect Option");
        break;
    }
    print_prompt();
  }
}

// Function Definitions

//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n***************************************************************\n"));
  Serial.print(F("* DC1962C Training Program                                      *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program teachings debugging by creating configuration    *\n"));
  Serial.print(F("* errors on the LTC1962C demo board.                            *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.print(F("\n  1-Problem 1\n"));
  Serial.print(F("  2-Problem 2\n"));
  Serial.print(F("  3-Problem 3\n"));
  Serial.print(F("  4-Problem 4\n"));
  Serial.print(F("  5-Problem 5\n"));
  Serial.print(F("  6-Problem 6\n"));
  Serial.print(F("  7-Problem 7\n"));
  Serial.print(F("  8-Problem 8\n"));
  Serial.print(F("  9-Problem 9\n"));
  Serial.print(F("  10-Problem 10\n"));
  Serial.print(F("  11-Problem 11\n"));
  Serial.print(F("  12-Problem 12\n"));
  Serial.print(F("  13-PEC On\n"));
  Serial.print(F("  14-PEC Off\n"));
  Serial.print(F("  15-Bus Probe\n"));
  Serial.print(F("  16-Reset\n"));
  Serial.print(F("\nEnter a command:"));
}


