/*!
LTC2642: 16-Bit DAC with SPI Interface

@verbatim

The LTC2641/LTC2642 are families of 16-,14 and 12-bit unbuffered voltage output
DACs. These DACs operate from 2.7V to 5.5V supply and are monotonic over
temperature.

They communicate via SPI interface. 16 bits of data are loaded through DIN
after a high-to-low transition of CS (MSB first). After 16 data bits have
been loaded into the serial input register, a low-to-high transition on CS
transfers the data to the 16-bit DAC latch, updating the DAC output.

For the 14-bit DACs, (LTC2641-14/LTC2642-14), the MSB remains in the same
(left-justified) position in the input 16-bit data word. Therefore, two
“don’t-care” bits must be loaded after the LSB, to make up the required
16 data bits. Similarly, for the 12-bit family members(LTC2641-12/LTC2642-12)
four “don’t-care” bits must follow the LSB.

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

//! @defgroup LTC2642 LTC2642: 16-Bit DAC with SPI Interface

/*! @file
    @ingroup LTC2642
    Library for LTC2642 16-Bit DAC with SPI Interface
*/

#include <SPI.h>
#include "LT_SPI.h"
#include "Linduino.h"

// Writes 16 bit data into serial input register
void LTC2642_write(uint8_t cs, uint16_t data)
{
  output_low(cs);
  delay(100);
  spi_write((uint8_t)(data >> 8));  // D15:D8
  spi_write((uint8_t)data);     // D7:D0
  delay(100);
  output_high(cs);          // CS high transfers data to DAC latch
}

// Calculates the 16 bit data code from voltage
uint16_t LTC2642_voltage_to_code(float voltage, float VREF)
{
  float code;
  code = voltage * 0xFFFF / VREF;
  return code;
}

// Calculates the output voltage from the given digital code and reference voltage
float LTC2642_code_to_voltage(uint16_t data, float VREF)
{
  float voltage;
  voltage = (float)data / 0xFFFF * VREF;
  return voltage;
}