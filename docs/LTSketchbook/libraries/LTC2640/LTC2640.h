/*!
LTC2640: Single 12-/10-/8-Bit Rail-to-Rail DACs with 10ppm/C Reference

@verbatim

The LTC2640 is a family of 12-, 10-, and 8-bit voltage-output DACs with an 
integrated, high-accuracy, low-drift reference in an 8-lead TSOT-23 package. It 
has a rail-to-rail output buffer that is guaranteed monotonic. 

The LTC2640-L has a full-scale output of 2.5V, and operates from a single 2.7V 
to 5.5V supply. The LTC2640-H has a full-scale output of 4.096V, and operates 
from a 4.5V to 5.5V supply. A 10 ppm/C reference output is available at the REF 
pin. 

Each DAC can also operate in External Reference mode, in which a voltage 
supplied to the REF pin sets the full- scale output. The LTC2640 DACs use a 
SPI/MICROWIRE compatible 3-wire serial interface which operates at clock rates 
up to 50 MHz. 

The LTC2640 incorporates a power-on reset circuit. Options are available for 
Reset to Zero Scale or Reset to Midscale after power-up. 

SPI DATA FORMAT (MSB First):

            Byte #1                            Byte #2          Byte #3
(LTC2640-12)
Data In  :  C3 C2 C1 C0 X X X X    D11 D10 D9 D8 D7 D6 D5 D4    D3 D2 D1 D0 X X X X
(LTC2640-10)
Data In  :  C3 C2 C1 C0 X X X X    D9  D8  D7 D6 D5 D4 D3 D2    D1 D0 X  X  X X X X
(LTC2640-8)
Data In  :  C3 C2 C1 C0 X X X X    D7  D6  D5 D4 D3 D2 D1 D0    X  X  X  X  X X X X

Cx   : DAC Command Code
Dx   : Data Bits
X    : Don't care

Example Code:

Set DAC to 1V for 12-bit DAC.


    shift_count = 4; // Set shift count for 12-bit DAC

    dac_voltage = 1.0;  // Set dac voltage variable to 1V;
    dac_code = LTC2640_voltage_to_code(dac_voltage, LTC2640_lsb, LTC2640_offset); // Calculates DAC code from desired voltage

    LTC2640_write(LTC2640_CS, LTC2640_CMD_WRITE_UPDATE, dac_code << shift_count);

@endverbatim

http://www.linear.com/product/LTC2640

http://www.linear.com/product/LTC2640#demoboards

REVISION HISTORY
$Revision: 1807 $
$Date: 2013-07-29 13:06:06 -0700 (Mon, 29 Jul 2013) $

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
    @ingroup LTC2640
    Header for LTC2640 Single 12-/10-/8-Bit Rail-to-Rail DACs with 10ppm/C Reference
*/

#ifndef LTC2640_H
#define LTC2640_H

#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTC2640_CS
#define LTC2640_CS QUIKEVAL_CS //! SPI Chip Select Pin
#endif

//! @name LTC2640 Command Codes (C3-C0)
//! @{
#define  LTC2640_CMD_WRITE               0x00  // Write to input register
#define  LTC2640_CMD_UPDATE              0x10  // Update (power up) DAC register
#define  LTC2640_CMD_WRITE_UPDATE        0x30  // Write to and update (power up) the DAC register
#define  LTC2640_CMD_POWER_DOWN          0x40  // Power down
#define  LTC2640_CMD_INTERNAL_REFERENCE  0x60  // Select internal reference (default at power up)
#define  LTC2640_CMD_EXTERNAL_REFERENCE  0x70  // Select the supply as the reference
//! @}

//! Write the dac_command and 16-bit dac_code to the LTC2640. The dac_code
//! must be left aligned before calling this function.
//! @return Void
void LTC2640_write(uint8_t cs,          //!< Chip Select Pin
                   uint8_t dac_command, //!< Command code nibble
                   uint16_t dac_code    //!< 12-bit DAC code, left justified
                  );

//! Calculate the LTC2640 DAC code given the desired output voltage
//! @return Code to send to DAC
uint16_t LTC2640_voltage_to_code(float dac_voltage,       //!< Voltage to send to DAC
                                 float LTC2640_lsb,       //!< LSB value (volts)
                                 int16_t LTC2640_offset   //!< Offset (volts)
                                );

//! Calculate the LTC2640 DAC output voltage given the DAC code, offset, and LSB value
//! @return Calculated voltage
float LTC2640_code_to_voltage(uint16_t dac_code,          //!< DAC code
                              float LTC2640_lsb,          //!< LSB value (volts)
                              int16_t LTC2640_offset      //!< Offset (volts)
                             );

//! Calculate the LTC2640 offset and LSB voltages given two measured voltages and their corresponding codes
//! @return Void
void LTC2640_calibrate(uint16_t dac_code1,                //!< First DAC code
                       uint16_t dac_code2,                //!< Second DAC code
                       float voltage1,                    //!< First voltage
                       float voltage2,                    //!< Second voltage
                       float *LTC2640_lsb,                //!< Returns resulting LSB (volts)
                       int16_t *LTC2640_offset            //!< Returns resulting Offset (volts)
                      );

#endif  // LTC2640_H
