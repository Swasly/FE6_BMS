/*!
Linear Technology DC1096B Demonstration Board.
LTC2642: 16-Bit DAC with SPI Interface

@verbatim

NOTES

   Set the terminal baud rate to 115200 and select the newline terminator.

   No external power supply is required. Two on-board reference voltages of
   2.5V and 5V are available.

   The board features LTC2642A with direct and buffered output.

   The program displays calculated voltages which are based on the reference
   voltage used. A precision voltmeter is needed to verify the actual
   measured voltages against the calculated voltage displayed.

   There is also an option of generating a square wave of required voltage
   levels and frequency. This proves the fast settling time of the corresponding
   parts.

  Explanation of Commands:

  Setup:
   Option 1: Enter a digital value to obtain analog voltage output.
   Option 2: Enter the frequency and voltage levels to obtain a square wave.
   Option 3: Vary the reference voltage used.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2642

http://www.linear.com/product/LTC2642#demoboards

REVISION HISTORY
$Revision: 3237 $
$Date: 2015-03-05 17:45:18 -0800 (Thu, 05 Mar 2015) $

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
*/

/*! @file
    @ingroup LTC2642
*/

// Headerfiles
#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>
#include "LT_SPI.h"
#include "LT_I2C.h"
#include "Linduino.h"
#include "QuikEval_EEPROM.h"
#include "UserInterface.h"
#include "LTC2642.h"
#include <QVariant>   // for parseFloat()

// Macros
#define VREF1 5
#define VREF2 2.5

// Global Variables
float VREF = VREF1;   // Set the reference voltage

// Function Declarations
void print_title();
void print_prompt();
uint8_t voltage_output();
uint8_t square_wave_output();
void change_VREF();
uint8_t discover_DC1096(char *demo_name);

//! Initialize Linduino
void setup()
{
  uint8_t demo_board_connected;

  char demo_name[] = "LTC2642";           // Demo Board Name stored in QuikEval EEPROM
  output_high(LTC2642_CS);                //! Pulls LTC2642 Chip Select (LD pin) High

  quikeval_I2C_init();                    //! Initializes Linduino I2C port.

  quikeval_SPI_init();                    //! Configures the SPI port for 4MHz SCK
  quikeval_SPI_connect();                 //! Connects SPI to QuikEval port

  Serial.begin(115200);                   //! Initializes the serial port to the PC
  print_title();                          //! Displays the title

  demo_board_connected = discover_DC1096(demo_name); //! Checks if correct demo board is connected.

  if (demo_board_connected)               //! Prints the prompt if the correct demo board is connected
  {
    print_prompt();                       // Prints prompt and indicates that "A" is selected.
  }
}

//! Repeats Linduino loop
void loop()
{
  int8_t user_command;                // The user input command
  uint8_t acknowledge = 0;
  if (Serial.available())             // Check for user input
  {
    user_command = read_int();        // Read the user command
    if (user_command != 'm')
      Serial.println(user_command);   // Prints the user command to com port
    Serial.flush();
    switch (user_command)
    {
      case 1:
        acknowledge |= voltage_output();
        break;
      case 2:
        acknowledge |= square_wave_output();
        break;
      case 3:
        change_VREF();
        break;
      default:
        Serial.println(F("Incorrect Option"));
    }
    if (acknowledge)
      Serial.println(F("***** I2C ERROR *****"));
    Serial.print(F("\n\n\t\t\tMake sure the on-board VREF jumper is set to "));
    Serial.print(VREF);
    Serial.println(" V");
    Serial.println(F("\t\t\t\tPress Enter to Continue..."));
    read_int();
    print_prompt();
  }
}

//! Function to enter a digital value and get the analog output
uint8_t voltage_output()
{
  uint16_t data;
  float voltage;
  Serial.print("\nEnter the 16-bit data as decimal or hex: ");
  data = read_int();
  Serial.print("0x");
  Serial.println(data, HEX);

  LTC2642_write(QUIKEVAL_CS, data);
  voltage = LTC2642_code_to_voltage(data, VREF);

  Serial.print("\nOutput voltage = ");
  Serial.print(voltage);
  Serial.println(" V");
  return 0;
}

//! Function to generate a square wave of desired frequency and voltage ranges
uint8_t square_wave_output()
{
  uint16_t freq;
  float time;
  float voltage_high, voltage_low;
  uint16_t code_high, code_low;
  uint8_t receive_enter;  // To receive enter key pressed

  Serial.print("\nEnter voltage_high: ");
  while (!Serial.available());
  voltage_high = Serial.parseFloat();
  Serial.print(voltage_high);
  Serial.println(" V");
  receive_enter = read_int();

  Serial.print("\nEnter voltage_low: ");
  while (!Serial.available());
  voltage_low = Serial.parseFloat();
  Serial.print(voltage_low);
  Serial.println(" V");
  receive_enter = read_int();

  Serial.print("\nEnter the required frequency in Hz: ");
  freq = read_int();
  Serial.print(freq);
  Serial.println(" Hz");

  time = (float)1000/freq;
  Serial.print("\nT = ");
  Serial.print(time);
  Serial.println(" ms");

  //! Converting data into voltage
  code_high = LTC2642_voltage_to_code(voltage_high, VREF);
  code_low = LTC2642_voltage_to_code(voltage_low, VREF);

  while (!Serial.available()) //! Generate square wave until a key is pressed
  {
    LTC2642_write(LTC2642_CS, code_high);
    delayMicroseconds(time * 500);
    LTC2642_write(LTC2642_CS, code_low);
    delayMicroseconds(time * 500);
  }
  receive_enter = read_int();
  return 0;
}

//! Function to change the reference voltage to be used
void change_VREF()
{
  uint8_t choice;
  Serial.println("\n1 - 5.00 V \n2 - 2.50 V");
  Serial.print("Enter your choice: ");
  choice = read_int();
  Serial.println(choice);

  if (choice == 1)
    VREF = VREF1;
  else if (choice == 2)
    VREF = VREF2;
  else
    Serial.println("\nWrong choice!");
}

//! Read the ID string from the EEPROM and determine if the correct board is connected.
//! Returns 1 if successful, 0 if not successful
uint8_t discover_DC1096(char *demo_name)
{
  Serial.print(F("\nChecking EEPROM contents..."));
  read_quikeval_id_string(&ui_buffer[0]);
  ui_buffer[48] = 0;
  Serial.println(ui_buffer);

  if (!strcmp(demo_board.product_name, demo_name))
  {
    Serial.print("Demo Board Name: ");
    Serial.println(demo_board.name);
    Serial.print("Product Name: ");
    Serial.println(demo_board.product_name);
    if (demo_board.option)
    {
      Serial.print("Demo Board Option: ");
      Serial.println(demo_board.option);
    }
    Serial.println(F("Demo board connected"));
    Serial.println(F("\n\n\t\t\t\tPress Enter to Continue..."));
    read_int();
    return 1;
  }
  else
  {
    Serial.print("Demo board ");
    Serial.print(demo_name);
    Serial.print(" not found, \nfound ");
    Serial.print(demo_board.name);
    Serial.println(" instead. \nConnect the correct demo board, then press the reset button.");
    return 0;
  }
}

//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* DC1096B Demonstration Program                                 *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send data to the 16-bit DAC. *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.print(F("\n----------------------------------------------------------------------"));
  Serial.print(F("\n\n\t\t\t\tOPTIONS\n"));
  Serial.print(F("\n1 - Voltage output\n"));
  Serial.print(F("2 - Square Wave output\n"));
  Serial.print(F("3 - Change VREF\n"));

  Serial.print(F("\nENTER A COMMAND: "));
}
