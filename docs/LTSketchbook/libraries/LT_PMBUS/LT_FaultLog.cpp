/*!
LTC SMBus Support: API for a shared LTC Fault Log

@verbatim

This API is shared with Linduino and RTOS code. End users should code to this
API to enable use of the PMBus code without modifications.

@endverbatim

REVISION HISTORY
$Revision: 3240 $
$Date: 2015-03-09 16:13:55 -0700 (Mon, 09 Mar 2015) $

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

//! @defgroup DoxygenExampleGroupName Doxygen Documentation Example

/*! @file
    @ingroup LT_I2CBus
    Library Header File for LT_FaultLog
*/

#include <Arduino.h>
#include "LT_FaultLog.h"

LT_FaultLog::LT_FaultLog(LT_SMBus *smbus)
{
	smbus_ = smbus;
}



uint8_t 
LT_FaultLog::readMfrFaultLogStatusByte(uint8_t address)
{
	uint8_t status_byte;

	status_byte = smbus_->readByte(address, MFR_FAULT_LOG_STATUS);

	return status_byte;
}
  
  /*
   * Enable fault log
   *
   * address: PMBUS address
   */
void 
LT_FaultLog::enableFaultLog(uint8_t address)
	{
	uint16_t 	id;
	uint8_t 	config8;
	uint16_t	config16;

	id = smbus_->readWord(address, MFR_SPECIAL_ID);
	if ((id & 0xFF00) == 0x4000) // LTC3880
	{
		config8 = smbus_->readByte(address, MFR_CONFIG_ALL_LTC3880);
		smbus_->writeByte(address, MFR_CONFIG_ALL_LTC3880, config8 | LTC3880_CFGALL_FLAG);
	}
	else if ((id & 0xFF00) == 0x4200) // LTC3882
	{
		config8 = smbus_->readByte(address, MFR_CONFIG_ALL_LTC3882);
		smbus_->writeByte(address, MFR_CONFIG_ALL_LTC3882, config8 | LTC3882_CFGALL_FLAG);
	}
	else if ((id & 0xFF00) == 0x4300) // LTC3883
	{
		config8 = smbus_->readByte(address, MFR_CONFIG_ALL_LTC3883);
		smbus_->writeByte(address, MFR_CONFIG_ALL_LTC3883, config8 | LTC3883_CFGALL_FLAG);
	}
	else if (id == 0x0212) // LTC2974
	{
		config16 = smbus_->readWord(address, MFR_CONFIG_ALL_LTC2974);
		smbus_->writeWord(address, MFR_CONFIG_ALL_LTC2974, config16 | LTC2974_CFGALL_FLAG);
	}
	else if (id == 0x0130 || id == 0x0121) // LTC2977 or LTC2978
	{
		config16 = smbus_->readWord(address, MFR_CONFIG_ALL_LTC2978);
		smbus_->writeWord(address, MFR_CONFIG_ALL_LTC2978, config16 | LTC2978_CFGALL_FLAG);
	}
}
  
  /*
   * Disable fault log
   *
   * address: PMBUS address
   */
void 
LT_FaultLog::disableFaultLog(uint8_t address)
{
	uint16_t 	id;
	uint8_t 	config8;
	uint16_t	config16;

	id = smbus_->readWord(address, MFR_SPECIAL_ID);
	if ((id & 0xFF00) == 0x4000) // LTC3880
	{
		config8 = smbus_->readByte(address, MFR_CONFIG_ALL_LTC3880);
		smbus_->writeByte(address, MFR_CONFIG_ALL_LTC3880, config8 & ~LTC3880_CFGALL_FLAG);
	}
	else if ((id & 0xFF00) == 0x4200) // LTC3882
	{
		config8 = smbus_->readByte(address, MFR_CONFIG_ALL_LTC3882);
		smbus_->writeByte(address, MFR_CONFIG_ALL_LTC3882, config8 & ~LTC3882_CFGALL_FLAG);
	}
	else if ((id & 0xFF00) == 0x4300) // LTC3883
	{
		config8 = smbus_->readByte(address, MFR_CONFIG_ALL_LTC3883);
		smbus_->writeByte(address, MFR_CONFIG_ALL_LTC3883, config8 & ~LTC3883_CFGALL_FLAG);
	}
	else if (id == 0x0212) // LTC2974
	{
		config16 = smbus_->readWord(address, MFR_CONFIG_ALL_LTC2974);
		smbus_->writeWord(address, MFR_CONFIG_ALL_LTC2974, config16 & ~LTC2974_CFGALL_FLAG);
	}
	else if (id == 0x0130 || id == 0x0121) // LTC2977 or LTC2978
	{
		config16 = smbus_->readWord(address, MFR_CONFIG_ALL_LTC2978);
		smbus_->writeWord(address, MFR_CONFIG_ALL_LTC2978, config16 & ~LTC2978_CFGALL_FLAG);
	}
}

void LT_FaultLog::dumpBin(Print* printer, uint8_t* log, uint8_t size)
{
	if (printer == 0)
		printer = &Serial;
	uint8_t* temp = log;
	for (uint8_t i = 0; i < size; i++)
	{
		if (!(i % 16))
			printer->println();
		if (temp[i] < 0x10)
			printer->write('0');
		printer->print(temp[i], HEX);
	}
	printer->println();
}

  
  /*
   * Clear fault log
   *
   * address: PMBUS address
   */
void 
LT_FaultLog::clearFaultLog(uint8_t address)
{
	smbus_->sendByte(address, MFR_FAULT_LOG_CLEAR);
}


uint64_t 
LT_FaultLog::getSharedTime200us(FaultLogTimeStamp time_stamp)
{
	uint64_t num200us = ((uint64_t) time_stamp.shared_time_byte5 << 40);
	num200us = num200us | ((uint64_t) time_stamp.shared_time_byte4 << 32);
	num200us = num200us | ((uint32_t) time_stamp.shared_time_byte3 << 24);
	num200us = num200us | ((uint32_t) time_stamp.shared_time_byte2 << 16);
	num200us = num200us | ((uint32_t) time_stamp.shared_time_byte1 << 8);
	num200us = num200us | (time_stamp.shared_time_byte0);
	return num200us;

}

float 
LT_FaultLog::getTimeInMs(FaultLogTimeStamp time_stamp)
{
	double ms = getSharedTime200us(time_stamp)/5.0;
	return ms;
}

uint8_t 
LT_FaultLog::getRawByteVal(RawByte value)
{
	return (uint8_t) value.uint8_tValue;
}

uint16_t 
LT_FaultLog::getRawWordVal(RawWord value)
{
	return (uint16_t) (value.lo_byte | (value.hi_byte << 8));
}

uint16_t 
LT_FaultLog::getRawWordReverseVal(RawWordReverse value)
{
	return (uint16_t) (value.lo_byte | (value.hi_byte << 8));
}

uint16_t 
LT_FaultLog::getLin5_11WordVal(Lin5_11Word value)
{
	return (uint16_t) (value.lo_byte | (value.hi_byte << 8));
}

uint16_t 
LT_FaultLog::getLin5_11WordReverseVal(Lin5_11WordReverse value)
{
	return (uint16_t) (value.lo_byte | (value.hi_byte << 8));
}

uint16_t 
LT_FaultLog::getLin16WordVal(Lin16Word value)
{
	return (uint16_t) (value.lo_byte | (value.hi_byte << 8));
}

uint16_t 
LT_FaultLog::getLin16WordReverseVal(Lin16WordReverse value)
{
	return (uint16_t) (value.lo_byte | (value.hi_byte << 8));
}
