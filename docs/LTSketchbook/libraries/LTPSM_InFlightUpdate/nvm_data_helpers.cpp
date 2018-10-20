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

#include "nvm_data_helpers.h"

/********************************************************************
 * Function:        nvramNode_p nvramListNew(uint16_t dataIn, uint8_t pecIn, uint8_t addIn, uint8_t cmdIn);
 *
 * PreCondition:    None
 * Input:           uint16_t dataIn 	= WORD of data from NVM block
 *					uint8_t pecIn		= Optionally use PEC or not, same as from t_RECORD_NVM_DATA->detailedRecordHeader->UsePec;
 *					uint8_t addIn		= Address of device this block belongs to, same as from t_RECORD_NVM_DATA->detailedRecordHeader->DeviceAddress
 *					uint8_t cmdIn		= Command for NVM block store of device, same as from t_RECORD_NVM_DATA->detailedRecordHeader->CommandCode
 * Output:          Returns a pointer to the newly created node on the stack/heap (depending on malloc implementation)
 * Overview:        Makes a new node for the Linked List
 * Note:            Note that depending on your implementation, you will need to closely
 *					monitor your stack or heap to ensure you have sufficient space to
 *					store all NVRAM data in this dynamically allocated linked list
 * 					Use of malloc is required so that the node is not destroyed when the function returns
 *******************************************************************/
nvramNode_p nvramListNew(uint16_t dataIn, uint8_t pecIn, uint8_t addIn, uint8_t cmdIn)
{
    nvramNode_p newNode = (nvramNode_p)malloc(sizeof(nvramNode_t));
    newNode->data_store = dataIn;
	newNode->nvram_usePEC = pecIn;
	newNode->nvram_pmbusAddress = addIn;
	newNode->nvram_pmbusCommand = cmdIn;
    newNode->next = NULL;
    return newNode;
}

/********************************************************************
 * Function:        void nvramListAdd(uint16_t dataIn, uint8_t pecIn, uint8_t addIn, uint8_t cmdIn, nvramNode_t* nvramList);
 *
 * PreCondition:    Must have a typeof(nvramNode_t) list available to add to and store NVM data contents into
 * Input:           uint16_t dataIn 			= WORD of data from NVM block
 *					uint8_t pecIn				= Optionally use PEC or not, same as from t_RECORD_NVM_DATA->detailedRecordHeader->UsePec;
 *					uint8_t addIn				= Address of device this block belongs to, same as from t_RECORD_NVM_DATA->detailedRecordHeader->DeviceAddress
 *					uint8_t cmdIn				= Command for NVM block store of device, same as from t_RECORD_NVM_DATA->detailedRecordHeader->CommandCode
 * 					nvramNode_t* nvramList 	= Base address for the linked list to store to (Making this a parameter allows for multiple NVM lists in the future)
 * Output:          None
 * Overview:        Adds a new data point to the master NVRAM buffer list
 * Note:            First it navigates to the end node of the linked list, then creates a new node, and links it
 * 					Note that to save processing power, you could save the last linked list address and reference that directly instead of finding it dynamically each time
 *******************************************************************/
void nvramListAdd(uint16_t dataIn, uint8_t pecIn, uint8_t addIn, uint8_t cmdIn, nvramNode_t* nvramList)
{
	nvramNode_p newNode;
    nvramNode_p node = nvramList;

    while (node->next != NULL)
    {
        node = node->next;
    }

    newNode = nvramListNew(dataIn, pecIn, addIn, cmdIn);
    node->next = newNode;
}

//nvramNode_p nvramListTop; // The main root node of the NVRAM buffer data
//nvramNode_p nvramListTopVerify; // The main root node of the NVRAM buffer data to verify

static uint8_t nvram_empty = 0; // Simple flag to make sure you are not writing something you have not buffered
static uint8_t nvram_somethingToVerify = 0; // Simple flag to make sure you are not verifying something you have not buffered or written

static uint16_t* words;
static uint16_t nWords;
static t_RECORD_NVM_DATA *record_pointer;
static uint8_t busy;

// This function replaced the older one above. It reads through the whole linked list
// and writes the nodes word by word. It also sets the 'nvramListTopVerify' pointer to
// the root node of the linked list it just wrote. This allows the verify function
// to know what to verify.
uint8_t writeNvmData(t_RECORD_NVM_DATA *pRecord)
{
	//nvramNode_p node = nvramListTop;
	uint8_t allGood = 1;
	nvram_empty = 0;

	//nvramListTopVerify = nvramListTop;
	nvram_somethingToVerify = 1;

    //while (node->next != NULL)
	for (uint16_t i = 0; i < nWords; i++)
    {
        //node = node->next;

		if(allGood)
		{
			if (pRecord->detailedRecordHeader.UsePec)
				smbusPec__->writeWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
					          pRecord->detailedRecordHeader.CommandCode,
						  	  words[i]);
			else
				smbusNoPec__->writeWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
					          pRecord->detailedRecordHeader.CommandCode,
						  	  words[i]);
		}

		if(allGood)
		{
			do
			{
				if (pRecord->detailedRecordHeader.UsePec)
					busy = smbusPec__->readByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress, 0xef);
				else
					busy = smbusNoPec__->readByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress, 0xef);

				busy = (busy & 0x40)==0;
			} while (busy);
		}
    }

	return allGood ? 1 : 0;
}

// This function is called to store the block of NVRAM data in the bitstream
// into a linked list to be used later.
uint8_t bufferNvmData(t_RECORD_NVM_DATA *pRecord)
{/*
	uint16_t i;
	words = (uint16_t*) ((uint16_t)pRecord+8); // Change (UINT16) to the size of an address on the target machine.
	nWords = (uint16_t)(pRecord->baseRecordHeader.Length-8)/2;

	if(nvram_empty == 0)
	{
		nvramListTop = nvramListNew(0,0,0,0); // Make the first node, always throw away
	}

	for(i=0; i < nWords; i++)
	{
		nvramListAdd(words[i], pRecord->detailedRecordHeader.UsePec, (uint8_t) pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, nvramListTop);
	}

	return 1;*/
	
	nvram_somethingToVerify = 1;
	
	uint16_t i;
	record_pointer = pRecord;
	nWords = (uint16_t)(pRecord->baseRecordHeader.Length-8)/2;
	words = (uint16_t*) ((uint16_t)pRecord+8); // Change (UINT16) to the size of an address on the target machine.
	
	
	return 1;
}

uint8_t allGood = 1;
uint16_t actual_value;
//uint16_t* actual_value_p = &actual_value;
uint16_t expected_value;

// This function reads the NVRAM back from the device and compares it against
// what was buffered. If any word does not match, it returns a fail flag
uint8_t read_then_verifyNvmData(t_RECORD_NVM_DATA *pRecord)
{
	//nvramNode_p node;
	//nvramNode_p lastNode;

	if(nvram_somethingToVerify == 0) { return 1; }

	nvram_somethingToVerify = 0;

	//node = nvramListTopVerify;
	allGood = 0;

    // while (node->next != NULL)
	for (uint16_t i = 0; i < nWords; i++)
    {
    	//lastNode = node;
        //node = node->next;
        //free(lastNode);
		expected_value = words[i];
		actual_value = 0;

		do
		{
			if (pRecord->detailedRecordHeader.UsePec)
				busy = smbusPec__->readByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress, 0xef);
			else
				busy = smbusNoPec__->readByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress, 0xef);

			busy = (busy & 0x40)==0;
		} while (busy);

		if (pRecord->detailedRecordHeader.UsePec)
			actual_value = smbusPec__->readWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode);
		else
			actual_value = smbusNoPec__->readWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode);

		if(actual_value != expected_value)
		{
			allGood = 1;
			break;
			//return 1;
		}
	}
    free(record_pointer);

	return allGood;
}
