/******************************************************************************
*
* Copyright (c) 2012-2014 Linear Technology Corporation (LTC)
* All Rights Reserved
*
*******************************************************************************
*
* Subject to Customer’s acceptance of the following terms and conditions, both
* redistribution of this software by Customer in binary form only and internal
* use of this software by Customer in source form, with or without modification,
* are permitted:
*
* - LTC has granted use rights to Customer in writing.
*
* - Any redistributions by Customer in binary form must include the above
*   copyright notice, this list of conditions and the following disclaimer in
*   the documentation and/or other materials provided with the redistribution.
*
* - Permission for internal use of the Software in source form is strictly
*   limited to Customer only and its controlled subsidiaries. Contractors,
*   consultants and all other third parties must seek and obtain separate
*   written use rights from LTC.
*
* - Other than for the above copyright notice and the below disclaimer, neither
*   the name of Linear Technology, Inc., Linear Technology Corporation (LTC)
*   nor the names of any of their contributors may be used by Customer for any
*   purpose, including, without limitation, the endorsement or promotion of any
*   products derived from this software without the fully informed, specific,
*   prior written permission from LTC.
*
*******************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY LTC "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL LTC OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************/

#include "nvm.h"

// static prog_uchar*	icpFile;

// uint16_t flashLocation = 0;


LT_PMBus * pmbus__;
LT_SMBusNoPec * smbusNoPec__;
LT_SMBusPec * smbusPec__;

NVM::NVM(LT_PMBus* pmbus, LT_SMBusNoPec* smbusNP, LT_SMBusPec* smbusP, uint8_t* addrs, uint8_t num)
{
	pmbus__ = pmbus;
	smbusNoPec__ = smbusNP;
	smbusPec__ = smbusP;
	
	numAddrs = num;
	addr = new uint8_t[numAddrs];
	for (int i = 0; i < numAddrs; i++)
	{
		addr[i] = addrs[i];
	}
}

NVM::~NVM()
{
	delete [] addr;
}

// bool NVM::load_hex_file(int settings)
// {
	// smbusNoPec__ = new LT_SMBusNoPec();
	// smbusPec__ = new LT_SMBusPec();
	// pmbus__ = new LT_PMBus(smbusNoPec__);
	
	// uint8_t addr[3] = { 0x30, 0x32, 0x33 };
	// if (1 == settings)
	// {
		// icpFile = icp1;
	// }
	// // else if (2 == settings)
	// // {
		// // icpFile = icp2;
	// // }
	// else
	// {
	   // return 0;
	// }

	// if (0 != i2cbus.writeByte(0x7C, 0x00) && 0 != i2cbus.writeByte(addr[0], 0x00))
	// {
		// //if no device found (bricked or expected), return error
		// return 0;
	// }
	
	// reset_parse_hex();
	// if(processRecordsOnDemand(get_record) == 0)
	// {
		// icpFile = 0;
		// return 0;
	// }
	// reset_parse_hex();
	// icpFile = 0;

	// // Wait for writing to finish because polling for ACK is
	// // failing in the smbus layer.
	// delay(100);
	// pmbus__->immediateOffGlobal();
	// delay(1000);
	// pmbus__->restoreFromNvmAll(addr, 3);
	// //restore_from_nvm_global();
	// // Wait for the restore instead of polling for ACK.
	// delay(100);
	// pmbus__->sequenceOnGlobal();
	
	// delete smbusNoPec__;
	// delete smbusPec__;
	// delete pmbus__;

	// return 1;
// }

bool NVM::load_hex_file(char *file)
{
	if (sdReader.open(file)) // 0 == success, other equals some failure
		return 0;

	reset_parse_hex();
	if(processRecordsOnDemand(get_record) == 0)
	{
		return 0;
	}
	reset_parse_hex();

	// Wait for writing to finish because polling for ACK is
	// failing in the smbus layer.
	delay(100);
	pmbus__->immediateOffGlobal();
	delay(1000);
	pmbus__->restoreFromNvmAll(addr, 3);
	// Wait for the restore instead of polling for ACK.
	delay(100);
	pmbus__->sequenceOnGlobal();

	return 1;
}

bool NVM::program()
{
	reset_parse_hex();
	if(processRecordsOnDemand(get_record) == 0)
	{
		return 0;
	}
	reset_parse_hex();

	// Wait for writing to finish because polling for ACK is
	// failing in the smbus layer.
	delay(100);
	pmbus__->immediateOffGlobal();
	delay(1000);
	pmbus__->restoreFromNvmAll(addr, numAddrs);
	// Wait for the restore instead of polling for ACK.
	delay(100);
	pmbus__->sequenceOnGlobal();
	
	return 1;
}

bool NVM::verify()
{
	reset_parse_hex();
	if(verifyRecordsOnDemand(get_record) == 0)
	{
		return 0;
	}
	reset_parse_hex();
	
	return 1;
}




uint8_t get_hex_data(void)
{
	uint8_t c = '\0';

	c = sdReader.read();
	while (c == '\n' || c == '\r')
		c = sdReader.read();
	if (c != -1)
		return c;
	else
		return 0;
}

// uint8_t get_hex_data(void)
// {
	// uint8_t c = '\0';
	// c = pgm_read_byte_near(icpFile + flashLocation++);
	
	// //count = read (hex_fd, &c, 1);
	// while (c == '\n' || c == '\r')
		// c = pgm_read_byte_near(icpFile + flashLocation++);
		// //count = read (hex_fd, &c, 1);

	// if ('\0' != c)
		// return c;
	// else
		// return 0;
// }

uint8_t get_record_data(void)
{
	return parse_hex(get_hex_data);
}

pRecordHeaderLengthAndType get_record(void)
{
	return parse_record(get_record_data);
}

