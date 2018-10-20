/*!
LTC SMBus Support: API for a LTC2977 Fault Log

@verbatim

This API is shared with Linduino and RTOS code. End users should code to this
API to enable use of the PMBus code without modifications.

@endverbatim

REVISION HISTORY
$Revision: 3018 $
$Date: 2014-12-01 15:53:20 -0800 (Mon, 01 Dec 2014) $

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
    @ingroup LT_2977FaultLog
    Library Header File for LT_2977FaultLog
*/

#include <Arduino.h>
#include "LT_2977FaultLog.h"
#include "LT_FaultLog.h"


LT_2977FaultLog::LT_2977FaultLog(LT_SMBus *smbus):LT_FaultLog(smbus)
{
}


bool LT_2977FaultLog::FaultLogLtc2977::isValidData(void* pos, uint8_t size)
{
	return (uint8_t*)pos >= (uint8_t*)this + firstValidByte && (uint8_t*)pos + (size - 1) <= (uint8_t*)this + lastValidByte;
}

/*
 * Read LTC2977 fault log
 *
 * address: PMBUS address
 */
struct LT_2977FaultLog::FaultLogLtc2977 * 
LT_2977FaultLog::read(uint8_t address)
{
	uint16_t size = sizeof(struct LT_2977FaultLog::FaultLogLtc2977);
	uint8_t* data = (uint8_t*) malloc(size);
	if(data == 0)
		Serial.print(F("bad malloc."));
	data[0] = 0x00;


	// Copy to RAM
	smbus_->sendByte(address, MFR_FAULT_LOG_RESTORE);
	// Monitor BUSY bit
	while ((smbus_->readByte(address, MFR_COMMON) & (1 << 6)) == 0);
	// Read block data with log
	smbus_->readBlock(address, MFR_FAULT_LOG, data, 255);
	
	struct LT_2977FaultLog::FaultLogLtc2977 *log = (struct LT_2977FaultLog::FaultLogLtc2977 *)data;

	log->firstValidByte = 72;
	
	uint8_t count = log->preamble.cyclic_data_count;
	if (count == 255)
		log->lastValidByte = 237;
	else
		log->lastValidByte = 237 - (166-count);
	
	uint8_t cycle_start = log->preamble.position_last;
	
	log->loops = (LT_2977FaultLog::FaultLogReadLoopLtc2977*) (log->telemetryData - 45 + cycle_start);
	
	
	faultLog2977 = log;
	
	return faultLog2977;
}


void LT_2977FaultLog::release()
{
	free(faultLog2977);
	faultLog2977 = 0;
}


void LT_2977FaultLog::dumpBinary(Print* printer)
{
	dumpBin(printer, (uint8_t*)faultLog2977, 255);
}

void LT_2977FaultLog::print(Print* printer)
{
	if (printer == 0)
		printer = &Serial;
	buffer = new char[FILE_TEXT_LINE_MAX];
	
	printTitle(printer);

	printTime(printer);

	printPeaks(printer);

	printAllLoops(printer);


	delete [] buffer;
}


void LT_2977FaultLog::printTitle(Print* printer)
{
	printer->print(F("LTC2977 Log Data\n"));
}

void LT_2977FaultLog::printTime(Print* printer)
{
	uint8_t *time = (uint8_t*)&faultLog2977->preamble.shared_time;
	snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Time 0x%02x%02x%02x%02x%02x%02x\n"), time[0], time[1], time[2], time[3], time[4], time[5]);
	printer->print(buffer);
	printer->print((long) getSharedTime200us(faultLog2977->preamble.shared_time));
	printer->println(F(" Ticks (200us each)"));
}

void LT_2977FaultLog::printPeaks(Print* printer)
{
	voutPeaks = new Peak16Words*[8];
	chanStatuses = new ChanStatus*[8];
	
	voutPeaks[0] = &faultLog2977->preamble.peaks.vout0_peaks;
	voutPeaks[1] = &faultLog2977->preamble.peaks.vout1_peaks;
	voutPeaks[2] = &faultLog2977->preamble.peaks.vout2_peaks;
	voutPeaks[3] = &faultLog2977->preamble.peaks.vout3_peaks;
	voutPeaks[4] = &faultLog2977->preamble.peaks.vout4_peaks;
	voutPeaks[5] = &faultLog2977->preamble.peaks.vout5_peaks;
	voutPeaks[6] = &faultLog2977->preamble.peaks.vout6_peaks;
	voutPeaks[7] = &faultLog2977->preamble.peaks.vout7_peaks;
	
	chanStatuses[0] = &faultLog2977->preamble.fault_log_status.chan_status0;
	chanStatuses[1] = &faultLog2977->preamble.fault_log_status.chan_status1;
	chanStatuses[2] = &faultLog2977->preamble.fault_log_status.chan_status2;
	chanStatuses[3] = &faultLog2977->preamble.fault_log_status.chan_status3;
	chanStatuses[4] = &faultLog2977->preamble.fault_log_status.chan_status4;
	chanStatuses[5] = &faultLog2977->preamble.fault_log_status.chan_status5;
	chanStatuses[6] = &faultLog2977->preamble.fault_log_status.chan_status6;
	chanStatuses[7] = &faultLog2977->preamble.fault_log_status.chan_status7;
	
	
	printer->println(F("\nPeak Values and Fast Status:"));
	printer->println(F("--------"));
	
	printFastChannel(0, printer);printFastChannel(1, printer);
	
	float vin_max, vin_min, temp_max, temp_min;
	vin_max = math_.lin11_to_float(getLin5_11WordVal(faultLog2977->preamble.peaks.vin_peaks.peak));
	vin_min = math_.lin11_to_float(getLin5_11WordVal(faultLog2977->preamble.peaks.vin_peaks.min));
	printer->print(F("Vin: Min: "));printer->print(vin_min, 6);printer->print(F(", Peak: "));printer->println(vin_max, 6);
	printer->println();
	
	printFastChannel(2, printer);printFastChannel(3, printer);

	temp_max = math_.lin11_to_float(getLin5_11WordVal(faultLog2977->preamble.peaks.temp_peaks.peak));
	temp_min = math_.lin11_to_float(getLin5_11WordVal(faultLog2977->preamble.peaks.temp_peaks.min));
	printer->print(F("Temp: Min: "));printer->print(temp_min, 6);printer->print(F(", Peak: "));printer->println(temp_max, 6);
	printer->println();
	
	printFastChannel(4, printer);printFastChannel(5, printer);printFastChannel(6, printer);printFastChannel(7, printer);
	
	delete [] voutPeaks;
	delete [] chanStatuses;
}

void LT_2977FaultLog::printFastChannel(uint8_t index, Print* printer)
{
	float vout_peak, vout_min;
	uint8_t status;
	vout_peak = math_.lin16_to_float(getLin16WordVal(voutPeaks[index]->peak), 0x13);
	vout_min = math_.lin16_to_float(getLin16WordVal(voutPeaks[index]->min), 0x13);
	printer->print(F("Vout"));printer->print(index);printer->print(F(": Min: "));
		printer->print(vout_min, 6);printer->print(F(", Peak: "));printer->println(vout_peak, 6);
	printer->print(F("Fast Status"));printer->println(index);
	status = getRawByteVal(chanStatuses[index]->status_vout);
	snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_VOUT%u: 0x%02x\n"), index, status);printer->print(buffer);
	status = getRawByteVal(chanStatuses[index]->status_mfr);
	snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_MFR%u: 0x%02x\n"), index, status);printer->print(buffer);
	status = getRawByteVal(chanStatuses[index]->mfr_status2);
	snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  MFR_STATUS_2%u: 0x%02x\n"), index, status);printer->println(buffer);
}

void LT_2977FaultLog::printAllLoops(Print* printer)
{
	voutDatas = new VoutData*[8];
	
	printer->println(F("Fault Log Loops Follow:"));
	printer->println(F("(most recent data first)"));
	
	for (int index = 0; index <= 4 && (index < 4 || faultLog2977->isValidData(&faultLog2977->loops[index])); index++)
	{
		printLoop(index, printer);
	}
	
	delete [] voutDatas;
}

void LT_2977FaultLog::printLoop(uint8_t index, Print* printer)
{	
	printer->println(F("-------"));
	printer->print(F("Loop: "));
	printer->println(index);
	printer->println(F("-------"));
	
	voutDatas[0] = &faultLog2977->loops[index].vout_data0;
	voutDatas[1] = &faultLog2977->loops[index].vout_data1;
	voutDatas[2] = &faultLog2977->loops[index].vout_data2;
	voutDatas[3] = &faultLog2977->loops[index].vout_data3;
	voutDatas[4] = &faultLog2977->loops[index].vout_data4;
	voutDatas[5] = &faultLog2977->loops[index].vout_data5;
	voutDatas[6] = &faultLog2977->loops[index].vout_data6;
	voutDatas[7] = &faultLog2977->loops[index].vout_data7;
	
	uint8_t stat;
	float val;
	
	printLoopChannel(7, printer);printLoopChannel(6, printer);printLoopChannel(5, printer);printLoopChannel(4, printer);
	
	if (faultLog2977->isValidData(&faultLog2977->loops[index].temp_data.status_temp, 1))
	{
		printer->println(F("TEMPERATURE:"));
		stat = getRawByteVal(faultLog2977->loops[index].temp_data.status_temp);
		snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_TEMP: 0x%02x\n"), stat);printer->print(buffer);
	}
	if (faultLog2977->isValidData(&faultLog2977->loops[index].temp_data.temp))
	{
		val = math_.lin11_to_float(getLin5_11WordReverseVal(faultLog2977->loops[index].temp_data.temp));
		printer->print(F("  READ_TEMP: "));
		printer->print(val, 6);
		printer->println(F(" C"));
	}
		
	printLoopChannel(3, printer);printLoopChannel(2, printer);
	
	if (faultLog2977->isValidData(&faultLog2977->loops[index].vin_data.status_vin, 1))
	{
		printer->println(F("VIN:"));
		stat = getRawByteVal(faultLog2977->loops[index].vin_data.status_vin);
		snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_INPUT: 0x%02x\n"), stat);printer->print(buffer);
	}
	if (faultLog2977->isValidData(&faultLog2977->loops[index].vin_data.vin))
	{
		val = math_.lin11_to_float(getLin5_11WordReverseVal(faultLog2977->loops[index].vin_data.vin));
		printer->print(F("  READ_VIN: "));
		printer->print(val, 6);
		printer->println(F(" V"));
	}
	
	printLoopChannel(1, printer);printLoopChannel(0, printer);
}

void LT_2977FaultLog::printLoopChannel(uint8_t index, Print* printer)
{
	uint8_t stat;
	float val;

	if (faultLog2977->isValidData(&voutDatas[index]->mfr_status2, 1))
	{
	  printer->print(F("CHAN"));printer->print(index);printer->println(F(":"));
	  stat = getRawByteVal(voutDatas[index]->mfr_status2);
	  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  MFR_STATUS_2: 0x%02x\n"), stat);printer->print(buffer);
	}
	if (faultLog2977->isValidData(&voutDatas[index]->status_mfr, 1))
	{
	  stat = getRawByteVal(voutDatas[index]->status_mfr);
	  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_MFR: 0x%02x\n"), stat);printer->print(buffer);
	}
	if (faultLog2977->isValidData(&voutDatas[index]->status_vout, 1))
	{
	  stat = getRawByteVal(voutDatas[index]->status_vout);
	  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_VOUT: 0x%02x\n"), stat);printer->print(buffer);
	}
	if (faultLog2977->isValidData(&voutDatas[index]->read_vout))
	{
	  val = math_.lin16_to_float(getLin16WordReverseVal(voutDatas[index]->read_vout), 0x13);
	  printer->print(F("  READ_VOUT: "));
	  printer->print(val, 6);
	  printer->println(F(" V"));
	}
}


