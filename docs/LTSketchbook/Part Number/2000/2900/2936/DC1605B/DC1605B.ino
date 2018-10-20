/*
Linear Technology DC1605B Demonstration Board
LTC2936: Programable Hex Voltage Supervisor with Comparator Outputs

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

@endverbatim
 
http://www.linear.com/product/LTC2933
http://www.linear.com/demo/DC1633B

REVISION HISTORY
$Revision:  $
$Date:  $

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

#define LTC2936_I2C_ADDRESS 0x50

#define LTC2936_V2_THR 0x05
#define LTC2936_STATUS_WORD 0x1F

// Global variables
static uint8_t ltc2936_i2c_address;
static LT_SMBus *smbus = new LT_SMBusNoPec();

//! Initialize Linduino
void setup()
{
  Serial.begin(115200);         //! Initialize the serial port to the PC
  print_title();
  ltc2936_i2c_address = LTC2936_I2C_ADDRESS;
  print_prompt();
}

//! Repeats Linduino loop
void loop()
{
  uint8_t user_command;
  uint8_t *addresses = NULL;
  uint8_t res;
  
  if (Serial.available())                          //! Checks for user input
  {
    user_command = read_int();                     //! Reads the user command
    if (user_command != 'm')
  	Serial.println(user_command);
  
    switch (user_command)                          //! Prints the appropriate submenu
    {
  	case 1:
          res = smbus->readWord(ltc2936_i2c_address, LTC2936_STATUS_WORD);
          Serial.println(res, HEX);
  	  break;
  	case 2:
          smbus->writeWord(ltc2936_i2c_address, LTC2936_V2_THR, 0x501E >> 1);
  	  break;
  	case 3:
          smbus->writeWord(ltc2936_i2c_address, LTC2936_V2_THR, 0x501E);   
  	  break;
  	case 4:
  	  addresses = smbus->probe(0);
  	  while(*addresses != 0)
  	  {
  		Serial.print(F("ADDR 0x"));
  		Serial.println(*addresses++, HEX);
  	  }
  	  break;
  	default:
  	  Serial.println(F("Incorrect Option"));
  	  break;
    }
    print_prompt();
  }

}

// Function Definitions

//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* DC1636B Demonstration Program                                 *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send and receive data from   *\n"));
  Serial.print(F("* the DC1633B demo board.                                       *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.print(F("  1-Read Status\n"));
  Serial.print(F("  2-Chan 2 Low Voltage\n"));
  Serial.print(F("  3-Chan 2 Nominal Voltage\n"));
  Serial.print(F("  4-Bus Probe\n"));
  Serial.print(F("\nEnter a command:"));
}

//! Prints a warning if the demo board is not detected.
void print_warning_prompt()
{
  Serial.println(F("\nWarning: Demo board not detected. Linduino will attempt to proceed."));
}



