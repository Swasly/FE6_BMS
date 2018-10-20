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

#include "main_record_processor.h"

#define _DEBUG_SILENT 0
#define _DEBUG_PROCESSING 0
#define _DEBUG_PRINT 0

/** PRIVATE PROTOTYPES RELATING TO RECORD TYPE PARSING ************/
uint8_t recordProcessor___0x01___processWriteByteOptionalPEC(t_RECORD_PMBUS_WRITE_BYTE*);
uint8_t recordProcessor___0x02___processWriteWordOptionalPEC(t_RECORD_PMBUS_WRITE_WORD*);
uint8_t recordProcessor___0x04___processReadByteExpectOptionalPEC(t_RECORD_PMBUS_READ_BYTE_EXPECT*);
uint8_t recordProcessor___0x05___processReadWordExpectOptionalPEC(t_RECORD_PMBUS_READ_WORD_EXPECT*);
uint8_t recordProcessor___0x09___bufferNVMData(t_RECORD_NVM_DATA*);
uint8_t recordProcessor___0x0A___processReadByteLoopMaskOptionalPEC(t_RECORD_PMBUS_READ_BYTE_LOOP_MASK*);
uint8_t recordProcessor___0x0B___processReadWordLoopMaskOptionalPEC(t_RECORD_PMBUS_READ_WORD_LOOP_MASK*);
uint8_t recordProcessor___0x0C___processPollReadByteUntilAckNoPEC(t_RECORD_PMBUS_POLL_READ_BYTE_UNTIL_ACK*);
uint8_t recordProcessor___0x0D___processDelayMs(t_RECORD_DELAY_MS*);
uint8_t recordProcessor___0x0E___processSendByteOptionalPEC(t_RECORD_PMBUS_SEND_BYTE*);
uint8_t recordProcessor___0x0F___processWriteByteNoPEC(t_RECORD_PMBUS_WRITE_BYTE_NOPEC*);
uint8_t recordProcessor___0x10___processWriteWordNoPEC(t_RECORD_PMBUS_WRITE_WORD_NOPEC*);
uint8_t recordProcessor___0x12___processReadByteExpectNoPEC(t_RECORD_PMBUS_READ_BYTE_EXPECT_NOPEC*);
uint8_t recordProcessor___0x13___processReadWordExpectNoPEC(t_RECORD_PMBUS_READ_WORD_EXPECT_NOPEC*);
uint8_t recordProcessor___0x15___processReadByteLoopMaskNoPEC(t_RECORD_PMBUS_READ_BYTE_LOOP_MASK_NOPEC*);
uint8_t recordProcessor___0x16___processReadWordLoopMaskNoPEC(t_RECORD_PMBUS_READ_WORD_LOOP_MASK_NOPEC*);
uint8_t recordProcessor___0x17___processSendByteNoPEC(t_RECORD_PMBUS_SEND_BYTE_NOPEC*);
uint8_t recordProcessor___0x18___processEvent(t_RECORD_EVENT*);
uint8_t recordProcessor___0x19___processReadByteExpectMaskNoPEC(t_RECORD_PMBUS_READ_BYTE_EXPECT_MASK_NOPEC*);
uint8_t recordProcessor___0x1A___processReadWordExpectMaskNoPEC(t_RECORD_PMBUS_READ_WORD_EXPECT_MASK_NOPEC*);
uint8_t recordProcessor___0x1B___processVariableMetaData(t_RECORD_VARIABLE_META_DATA*);
uint8_t recordProcessor___0x1C___modifyWordNoPEC(t_RECORD_PMBUS_MODIFY_WORD_NO_PEC*);
uint8_t recordProcessor___0x1D___modifyByteNoPEC(t_RECORD_PMBUS_MODIFY_BYTE_NO_PEC*);
uint8_t recordProcessor___0x1E___writeNvmData(t_RECORD_NVM_DATA*);
uint8_t recordProcessor___0x1F___read_then_verifyNvmData(t_RECORD_NVM_DATA*);
uint8_t recordProcessor___0x20___modifyByteOptionalPEC(t_RECORD_PMBUS_MODIFY_BYTE*);
uint8_t recordProcessor___0x21___modifyWordOptionalPEC(t_RECORD_PMBUS_MODIFY_WORD*);

/** VARIABLES ******************************************************/
_InCircuitProgrammingRecordTypeListItem_p ICP_RTLI_Root; 	// The main root node of the In-Circuit Programming Record Type List buffer data
uint8_t ICP_RTLI_Root___empty = 0;							// Number of elements in the ICP Record Type List buffer

uint16_t device_serial_number = 0x0001;						// First device serial number. Note that this is only supported on the LTC2978
t_RECORD_META_OEM_SERIAL_NUMBER* pSerialNumberDetails;		// A pointer to the Serial Number details. The serial number is incremented inside this type. Note that this is only supported on the LTC2978

/********************************************************************
 * Function:        _InCircuitProgrammingRecordTypeListItem_p ICP_RTLI_CreateNew(tRecordHeader* record);
 *
 * PreCondition:    None
 * Input:           tRecordHeader* record = tRecordHeader of the record to add to the list
 * Output:          Returns a pointer to the newly created node on the stack/heap (depending on malloc implementation)
 * Overview:        Makes a new node for the Linked List
 * Note:            Note that depending on your implementation, you will need to closely
 *					monitor your stack or heap to ensure you have sufficient space to
 *					store all record data in this dynamically allocated linked list
 * 					Use of malloc is required so that the node is not destroyed when the function returns
 *******************************************************************/
_InCircuitProgrammingRecordTypeListItem_p ICP_RTLI_CreateNew(pRecordHeaderLengthAndType record)
{
	_InCircuitProgrammingRecordTypeListItem_p newNode = (_InCircuitProgrammingRecordTypeListItem_p)malloc(sizeof(_InCircuitProgrammingRecordTypeListItem_t));
    newNode->record = record;
    newNode->next = NULL;
    return newNode;
}

/********************************************************************
 * Function:        void ICP_RTLI_Add(tRecordHeader* record, _InCircuitProgrammingRecordTypeListItem_t* recordList)
 *
 * PreCondition:    Must have a typeof(nvramNode_t) list available to add to and store NVM data contents into
 * Input:           tRecordHeader* record									= Record Header of new programming record
 *					_InCircuitProgrammingRecordTypeListItem_t* recordList 	= Programming record list item to add new item to
 * Output:          None
 * Overview:        Adds a new data point to the master In Circuit Programming Record Type List
 * Note:            First it navigates to the end node of the linked list, then creates a new node, and links it
 * 					Note that to save processing power, you could save the last linked list address and reference that directly instead of finding it dynamically each time
 *******************************************************************/
void ICP_RTLI_Add(pRecordHeaderLengthAndType record, _InCircuitProgrammingRecordTypeListItem_t* recordList)
{
    _InCircuitProgrammingRecordTypeListItem_p node = recordList;
    _InCircuitProgrammingRecordTypeListItem_p newNode;

    while (node->next != NULL)
    {
        node = node->next;
    }

    newNode = ICP_RTLI_CreateNew(record);
    node->next = newNode;
}

// This function is called to store the block of NVRAM data in the bitstream
// into a linked list to be used later.
void InitializeRecordTypeList(void)
{
	ICP_RTLI_Root___empty = 0;
	ICP_RTLI_Root = ICP_RTLI_CreateNew(0); // Make the first node, always throw away
	return;
}

void ReleaseRecordTypeList(void)
{
    _InCircuitProgrammingRecordTypeListItem_p node = ICP_RTLI_Root;
    _InCircuitProgrammingRecordTypeListItem_p nextNode;

    while (node->next != NULL)
    {
    	nextNode = node->next;
    	free(node);
        node = nextNode;
    }
    free(node);
}

/********************************************************************
 * Function:        uint8_t processRecords(void);
 *
 * PreCondition:    None
 * Input:           None
 * Output:          Returns SUCCESS (1) or FAILURE (0) depending on the status of parsing ALL the record types
 * Overview:        Processes all the records stored in the record type list
 * Note:            None
 *******************************************************************/
uint8_t processRecords(void)
{
	// Not implemented yet...
	_InCircuitProgrammingRecordTypeListItem_p node = ICP_RTLI_Root;
	struct tRecordHeader record_to_process;
	uint16_t recordType_of_record_to_process;
	uint8_t successful_parse_of_record_type = SUCCESS;

	while (node->next != NULL && successful_parse_of_record_type == SUCCESS)
    {
        node = node->next;
		record_to_process = *(node->record);
		recordType_of_record_to_process = record_to_process.RecordType;

	    switch(recordType_of_record_to_process)
		{
			case RECORD_TYPE_PMBUS_WRITE_BYTE: // 0x01
				successful_parse_of_record_type = recordProcessor___0x01___processWriteByteOptionalPEC( (t_RECORD_PMBUS_WRITE_BYTE *) node->record);
				break;
			case RECORD_TYPE_PMBUS_WRITE_WORD: // 0x02
				successful_parse_of_record_type = recordProcessor___0x02___processWriteWordOptionalPEC( (t_RECORD_PMBUS_WRITE_WORD *) node->record);
				break;
			case RECORD_TYPE_PMBUS_WRITE_BLOCK: // 0x03
				successful_parse_of_record_type = FAILURE; // Unsupported Record Type
				break;
			case RECORD_TYPE_PMBUS_READ_BYTE_EXPECT: // 0x04
				successful_parse_of_record_type = recordProcessor___0x04___processReadByteExpectOptionalPEC( (t_RECORD_PMBUS_READ_BYTE_EXPECT *) node->record);
				break;
			case RECORD_TYPE_PMBUS_READ_WORD_EXPECT: // 0x05
				successful_parse_of_record_type = recordProcessor___0x05___processReadWordExpectOptionalPEC( (t_RECORD_PMBUS_READ_WORD_EXPECT *) node->record);
				break;
			case RECORD_TYPE_PMBUS_READ_BLOCK_EXPECT: // 0x06
				successful_parse_of_record_type = FAILURE; // Unsupported Record Type
				break;
			case RECORD_TYPE_DEVICE_ADDRESS: // 0x07 -- OBSOLETED
				successful_parse_of_record_type = SUCCESS; // Do nothing for this record type, but do not fail
				break;
			case RECORD_TYPE_PACKING_CODE: // 0x08 -- OBSOLETED
				successful_parse_of_record_type = SUCCESS; // Do nothing for this record type, but do not fail
				break;
			case RECORD_TYPE_NVM_DATA: // 0x09 -- FUNCTIONALITY CHANGED 25/01/2011
				successful_parse_of_record_type = recordProcessor___0x09___bufferNVMData( (t_RECORD_NVM_DATA *) node->record);
				break;
			case RECORD_TYPE_PMBUS_READ_BYTE_LOOP_MASK: // 0x0A
				successful_parse_of_record_type = recordProcessor___0x0A___processReadByteLoopMaskOptionalPEC( (t_RECORD_PMBUS_READ_BYTE_LOOP_MASK *) node->record);
				break;
			case RECORD_TYPE_PMBUS_READ_WORD_LOOP_MASK: //0x0B
				successful_parse_of_record_type = recordProcessor___0x0B___processReadWordLoopMaskOptionalPEC( (t_RECORD_PMBUS_READ_WORD_LOOP_MASK *) node->record);
				break;
			case RECORD_TYPE_PMBUS_POLL_UNTIL_ACK_NOPEC: // 0x0C
				successful_parse_of_record_type = recordProcessor___0x0C___processPollReadByteUntilAckNoPEC( (t_RECORD_PMBUS_POLL_READ_BYTE_UNTIL_ACK *) node->record);
				break;
			case RECORD_TYPE_DELAY_MS: // 0x0D
				successful_parse_of_record_type = recordProcessor___0x0D___processDelayMs( (t_RECORD_DELAY_MS *) node->record);
				break;
			case RECORD_TYPE_PMBUS_SEND_BYTE: //0x0E
				successful_parse_of_record_type = recordProcessor___0x0E___processSendByteOptionalPEC( (t_RECORD_PMBUS_SEND_BYTE *) node->record);
				break;
			case RECORD_TYPE_PMBUS_WRITE_BYTE_NOPEC: // 0x0F
				successful_parse_of_record_type = recordProcessor___0x0F___processWriteByteNoPEC( (t_RECORD_PMBUS_WRITE_BYTE_NOPEC *) node->record);
				break;
			case RECORD_TYPE_PMBUS_WRITE_WORD_NOPEC: // 0x10
				successful_parse_of_record_type = recordProcessor___0x10___processWriteWordNoPEC( (t_RECORD_PMBUS_WRITE_WORD_NOPEC *) node->record);
				break;
			case RECORD_TYPE_PMBUS_WRITE_BLOCK_NOPEC: // 0x11
				successful_parse_of_record_type = FAILURE; // Unsupported Record Type
				break;
			case RECORD_TYPE_PMBUS_READ_BYTE_EXPECT_NOPEC: // 0x12
				successful_parse_of_record_type = recordProcessor___0x12___processReadByteExpectNoPEC( (t_RECORD_PMBUS_READ_BYTE_EXPECT_NOPEC *) node->record);
				break;
			case RECORD_TYPE_PMBUS_READ_WORD_EXPECT_NOPEC: // 0x13
				successful_parse_of_record_type = recordProcessor___0x13___processReadWordExpectNoPEC( (t_RECORD_PMBUS_READ_WORD_EXPECT_NOPEC *) node->record);
				break;
			case RECORD_TYPE_PMBUS_READ_BLOCK_EXPECT_NOPEC: // 0x14
				successful_parse_of_record_type = FAILURE; // Unsupported Record Type
				break;
			case RECORD_TYPE_PMBUS_READ_BYTE_LOOP_MASK_NOPEC: // 0x15
				successful_parse_of_record_type = recordProcessor___0x15___processReadByteLoopMaskNoPEC( (t_RECORD_PMBUS_READ_BYTE_LOOP_MASK_NOPEC *) node->record);
				break;
			case RECORD_TYPE_PMBUS_READ_WORD_LOOP_MASK_NOPEC: // 0x16
				successful_parse_of_record_type = recordProcessor___0x16___processReadWordLoopMaskNoPEC( (t_RECORD_PMBUS_READ_WORD_LOOP_MASK_NOPEC *) node->record);
				break;
			case RECORD_TYPE_PMBUS_SEND_BYTE_NOPEC: // 0x17
				successful_parse_of_record_type = recordProcessor___0x17___processSendByteNoPEC( (t_RECORD_PMBUS_SEND_BYTE_NOPEC *) node->record);
				break;
			case RECORD_TYPE_EVENT: // 0x18
				successful_parse_of_record_type = recordProcessor___0x18___processEvent( (t_RECORD_EVENT*) node->record);
				break;
			case RECORD_TYPE_PMBUS_READ_BYTE_EXPECT_MASK_NOPEC: // 0x19
				successful_parse_of_record_type = recordProcessor___0x19___processReadByteExpectMaskNoPEC( (t_RECORD_PMBUS_READ_BYTE_EXPECT_MASK_NOPEC *) node->record);
				break;
			case RECORD_TYPE_PMBUS_READ_WORD_EXPECT_MASK_NOPEC: //0x1A
				successful_parse_of_record_type = recordProcessor___0x1A___processReadWordExpectMaskNoPEC( (t_RECORD_PMBUS_READ_WORD_EXPECT_MASK_NOPEC *) node->record);
				break;
			case RECORD_TYPE_VARIABLE_META_DATA: // 0x1B
				successful_parse_of_record_type = recordProcessor___0x1B___processVariableMetaData( (t_RECORD_VARIABLE_META_DATA*) node->record);
				break;
			case RECORD_TYPE_MODIFY_WORD_NOPEC: // 0x1C
				successful_parse_of_record_type = recordProcessor___0x1C___modifyWordNoPEC( (t_RECORD_PMBUS_MODIFY_WORD_NO_PEC *) node->record);
				break;
			case RECORD_TYPE_MODIFY_BYTE_NOPEC: // 0x1D
				successful_parse_of_record_type = recordProcessor___0x1D___modifyByteNoPEC( (t_RECORD_PMBUS_MODIFY_BYTE_NO_PEC *) node->record);
				break;
			case RECORD_TYPE_PMBUS_WRITE_EE_DATA: // 0x1E
				successful_parse_of_record_type = recordProcessor___0x1E___writeNvmData( (t_RECORD_NVM_DATA *) node->record);
				break;
			case RECORD_TYPE_PMBUS_READ_AND_VERIFY_EE_DATA: // 0x1F
				successful_parse_of_record_type = recordProcessor___0x1F___read_then_verifyNvmData( (t_RECORD_NVM_DATA *) node->record);
				break;
			case RECORD_TYPE_PMBUS_MODIFY_BYTE: // 0x20
				successful_parse_of_record_type = recordProcessor___0x20___modifyByteOptionalPEC( (t_RECORD_PMBUS_MODIFY_BYTE *) node->record);
				break;
			case RECORD_TYPE_PMBUS_MODIFY_WORD: // 0x21
				successful_parse_of_record_type = recordProcessor___0x21___modifyWordOptionalPEC( (t_RECORD_PMBUS_MODIFY_WORD *) node->record);
				break;
			default:
				successful_parse_of_record_type = FAILURE; // Unknown Instruction, report a failure
				break;
		}
	}

	return successful_parse_of_record_type;
}

/********************************************************************
 * Function:        uint8_t processRecordArray(_InCircuitProgrammingRecordTypeListItem_p node, uint16_t length);
 *
 * PreCondition:    None
 * Input:           Array of nodes and length
 * Output:          Returns SUCCESS (1) or FAILURE (0) depending on the status of parsing ALL the record types
 * Overview:        Processes all the records stored in the record type list
 * Note:            None
 *******************************************************************/
uint8_t processRecordArray(ppRecordHeaderLengthAndType records, uint16_t length)
{
	pRecordHeaderLengthAndType record_to_process;
	uint16_t recordType_of_record_to_process;
	uint8_t successful_parse_of_record_type = SUCCESS;
	uint16_t position = 0;

	while (position < length && successful_parse_of_record_type == SUCCESS)
    {
 		record_to_process = *(records+position);
		recordType_of_record_to_process = record_to_process->RecordType;

	    switch(recordType_of_record_to_process)
		{
			case RECORD_TYPE_PMBUS_WRITE_BYTE: // 0x01
				successful_parse_of_record_type = recordProcessor___0x01___processWriteByteOptionalPEC( (t_RECORD_PMBUS_WRITE_BYTE *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_WRITE_WORD: // 0x02
				successful_parse_of_record_type = recordProcessor___0x02___processWriteWordOptionalPEC( (t_RECORD_PMBUS_WRITE_WORD *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_WRITE_BLOCK: // 0x03
				successful_parse_of_record_type = FAILURE; // Unsupported Record Type
				break;
			case RECORD_TYPE_PMBUS_READ_BYTE_EXPECT: // 0x04
				successful_parse_of_record_type = recordProcessor___0x04___processReadByteExpectOptionalPEC( (t_RECORD_PMBUS_READ_BYTE_EXPECT *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_WORD_EXPECT: // 0x05
				successful_parse_of_record_type = recordProcessor___0x05___processReadWordExpectOptionalPEC( (t_RECORD_PMBUS_READ_WORD_EXPECT *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_BLOCK_EXPECT: // 0x06
				successful_parse_of_record_type = FAILURE; // Unsupported Record Type
				break;
			case RECORD_TYPE_DEVICE_ADDRESS: // 0x07 -- OBSOLETED
				successful_parse_of_record_type = SUCCESS; // Do nothing for this record type, but do not fail
				break;
			case RECORD_TYPE_PACKING_CODE: // 0x08 -- OBSOLETED
				successful_parse_of_record_type = SUCCESS; // Do nothing for this record type, but do not fail
				break;
			case RECORD_TYPE_NVM_DATA: // 0x09 -- FUNCTIONALITY CHANGED 25/01/2011
				successful_parse_of_record_type = recordProcessor___0x09___bufferNVMData( (t_RECORD_NVM_DATA *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_BYTE_LOOP_MASK: // 0x0A
				successful_parse_of_record_type = recordProcessor___0x0A___processReadByteLoopMaskOptionalPEC( (t_RECORD_PMBUS_READ_BYTE_LOOP_MASK *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_WORD_LOOP_MASK: //0x0B
				successful_parse_of_record_type = recordProcessor___0x0B___processReadWordLoopMaskOptionalPEC( (t_RECORD_PMBUS_READ_WORD_LOOP_MASK *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_POLL_UNTIL_ACK_NOPEC: // 0x0C
				successful_parse_of_record_type = recordProcessor___0x0C___processPollReadByteUntilAckNoPEC( (t_RECORD_PMBUS_POLL_READ_BYTE_UNTIL_ACK *) record_to_process);
				break;
			case RECORD_TYPE_DELAY_MS: // 0x0D
				successful_parse_of_record_type = recordProcessor___0x0D___processDelayMs( (t_RECORD_DELAY_MS *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_SEND_BYTE: //0x0E
				successful_parse_of_record_type = recordProcessor___0x0E___processSendByteOptionalPEC( (t_RECORD_PMBUS_SEND_BYTE *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_WRITE_BYTE_NOPEC: // 0x0F
				successful_parse_of_record_type = recordProcessor___0x0F___processWriteByteNoPEC( (t_RECORD_PMBUS_WRITE_BYTE_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_WRITE_WORD_NOPEC: // 0x10
				successful_parse_of_record_type = recordProcessor___0x10___processWriteWordNoPEC( (t_RECORD_PMBUS_WRITE_WORD_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_WRITE_BLOCK_NOPEC: // 0x11
				successful_parse_of_record_type = FAILURE; // Unsupported Record Type
				break;
			case RECORD_TYPE_PMBUS_READ_BYTE_EXPECT_NOPEC: // 0x12
				successful_parse_of_record_type = recordProcessor___0x12___processReadByteExpectNoPEC( (t_RECORD_PMBUS_READ_BYTE_EXPECT_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_WORD_EXPECT_NOPEC: // 0x13
				successful_parse_of_record_type = recordProcessor___0x13___processReadWordExpectNoPEC( (t_RECORD_PMBUS_READ_WORD_EXPECT_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_BLOCK_EXPECT_NOPEC: // 0x14
				successful_parse_of_record_type = FAILURE; // Unsupported Record Type
				break;
			case RECORD_TYPE_PMBUS_READ_BYTE_LOOP_MASK_NOPEC: // 0x15
				successful_parse_of_record_type = recordProcessor___0x15___processReadByteLoopMaskNoPEC( (t_RECORD_PMBUS_READ_BYTE_LOOP_MASK_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_WORD_LOOP_MASK_NOPEC: // 0x16
				successful_parse_of_record_type = recordProcessor___0x16___processReadWordLoopMaskNoPEC( (t_RECORD_PMBUS_READ_WORD_LOOP_MASK_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_SEND_BYTE_NOPEC: // 0x17
				successful_parse_of_record_type = recordProcessor___0x17___processSendByteNoPEC( (t_RECORD_PMBUS_SEND_BYTE_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_EVENT: // 0x18
				successful_parse_of_record_type = recordProcessor___0x18___processEvent( (t_RECORD_EVENT*) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_BYTE_EXPECT_MASK_NOPEC: // 0x19
				successful_parse_of_record_type = recordProcessor___0x19___processReadByteExpectMaskNoPEC( (t_RECORD_PMBUS_READ_BYTE_EXPECT_MASK_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_WORD_EXPECT_MASK_NOPEC: //0x1A
				successful_parse_of_record_type = recordProcessor___0x1A___processReadWordExpectMaskNoPEC( (t_RECORD_PMBUS_READ_WORD_EXPECT_MASK_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_VARIABLE_META_DATA: // 0x1B
				successful_parse_of_record_type = recordProcessor___0x1B___processVariableMetaData( (t_RECORD_VARIABLE_META_DATA*) record_to_process);
				break;
			case RECORD_TYPE_MODIFY_WORD_NOPEC: // 0x1C
				successful_parse_of_record_type = recordProcessor___0x1C___modifyWordNoPEC( (t_RECORD_PMBUS_MODIFY_WORD_NO_PEC *) record_to_process);
				break;
			case RECORD_TYPE_MODIFY_BYTE_NOPEC: // 0x1D
				successful_parse_of_record_type = recordProcessor___0x1D___modifyByteNoPEC( (t_RECORD_PMBUS_MODIFY_BYTE_NO_PEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_WRITE_EE_DATA: // 0x1E
				successful_parse_of_record_type = recordProcessor___0x1E___writeNvmData( (t_RECORD_NVM_DATA *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_AND_VERIFY_EE_DATA: // 0x1F
				successful_parse_of_record_type = recordProcessor___0x1F___read_then_verifyNvmData( (t_RECORD_NVM_DATA *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_MODIFY_BYTE: // 0x20
				successful_parse_of_record_type = recordProcessor___0x20___modifyByteOptionalPEC( (t_RECORD_PMBUS_MODIFY_BYTE *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_MODIFY_WORD: // 0x21
				successful_parse_of_record_type = recordProcessor___0x21___modifyWordOptionalPEC( (t_RECORD_PMBUS_MODIFY_WORD *) record_to_process);
				break;
			default:
				successful_parse_of_record_type = FAILURE; // Unknown Instruction, report a failure
				break;
		}
	    position++;
	}

	return successful_parse_of_record_type;
}

/********************************************************************
 * Function:        uint8_t processRecordsOnDemand(_InCircuitProgrammingRecordTypeListItem_p node, uint16_t length);
 *
 * PreCondition:    None
 * Input:           Function to get records one by one
 * Output:          Returns SUCCESS (1) or FAILURE (0) depending on the status of parsing ALL the record types
 * Overview:        Processes all the records until the function to get records returns null
 * 					Records are freed as processed.
 * Note:            None
 *******************************************************************/
uint8_t processRecordsOnDemand(pRecordHeaderLengthAndType (*getRecord)(void))
{
	pRecordHeaderLengthAndType record_to_process;
	uint16_t recordType_of_record_to_process;
	uint8_t successful_parse_of_record_type = SUCCESS;

	while ((record_to_process = getRecord()) != NULL && successful_parse_of_record_type == SUCCESS)
    {
		recordType_of_record_to_process = record_to_process->RecordType;
		
	    switch(recordType_of_record_to_process)
		{
			case RECORD_TYPE_PMBUS_WRITE_BYTE: // 0x01
				successful_parse_of_record_type = recordProcessor___0x01___processWriteByteOptionalPEC( (t_RECORD_PMBUS_WRITE_BYTE *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_WRITE_WORD: // 0x02
				successful_parse_of_record_type = recordProcessor___0x02___processWriteWordOptionalPEC( (t_RECORD_PMBUS_WRITE_WORD *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_WRITE_BLOCK: // 0x03
				successful_parse_of_record_type = FAILURE; // Unsupported Record Type
				break;
			case RECORD_TYPE_PMBUS_READ_BYTE_EXPECT: // 0x04
				successful_parse_of_record_type = recordProcessor___0x04___processReadByteExpectOptionalPEC( (t_RECORD_PMBUS_READ_BYTE_EXPECT *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_WORD_EXPECT: // 0x05
				successful_parse_of_record_type = recordProcessor___0x05___processReadWordExpectOptionalPEC( (t_RECORD_PMBUS_READ_WORD_EXPECT *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_BLOCK_EXPECT: // 0x06
				successful_parse_of_record_type = FAILURE; // Unsupported Record Type
				break;
			case RECORD_TYPE_DEVICE_ADDRESS: // 0x07 -- OBSOLETED
				successful_parse_of_record_type = SUCCESS; // Do nothing for this record type, but do not fail
				break;
			case RECORD_TYPE_PACKING_CODE: // 0x08 -- OBSOLETED
				successful_parse_of_record_type = SUCCESS; // Do nothing for this record type, but do not fail
				break;
			case RECORD_TYPE_NVM_DATA: // 0x09 -- FUNCTIONALITY CHANGED 25/01/2011
				successful_parse_of_record_type = recordProcessor___0x09___bufferNVMData( (t_RECORD_NVM_DATA *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_BYTE_LOOP_MASK: // 0x0A
				successful_parse_of_record_type = recordProcessor___0x0A___processReadByteLoopMaskOptionalPEC( (t_RECORD_PMBUS_READ_BYTE_LOOP_MASK *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_WORD_LOOP_MASK: //0x0B
				successful_parse_of_record_type = recordProcessor___0x0B___processReadWordLoopMaskOptionalPEC( (t_RECORD_PMBUS_READ_WORD_LOOP_MASK *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_POLL_UNTIL_ACK_NOPEC: // 0x0C
				successful_parse_of_record_type = recordProcessor___0x0C___processPollReadByteUntilAckNoPEC( (t_RECORD_PMBUS_POLL_READ_BYTE_UNTIL_ACK *) record_to_process);
				break;
			case RECORD_TYPE_DELAY_MS: // 0x0D
				successful_parse_of_record_type = recordProcessor___0x0D___processDelayMs( (t_RECORD_DELAY_MS *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_SEND_BYTE: //0x0E
				successful_parse_of_record_type = recordProcessor___0x0E___processSendByteOptionalPEC( (t_RECORD_PMBUS_SEND_BYTE *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_WRITE_BYTE_NOPEC: // 0x0F
				successful_parse_of_record_type = recordProcessor___0x0F___processWriteByteNoPEC( (t_RECORD_PMBUS_WRITE_BYTE_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_WRITE_WORD_NOPEC: // 0x10
				successful_parse_of_record_type = recordProcessor___0x10___processWriteWordNoPEC( (t_RECORD_PMBUS_WRITE_WORD_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_WRITE_BLOCK_NOPEC: // 0x11
				successful_parse_of_record_type = FAILURE; // Unsupported Record Type
				break;
			case RECORD_TYPE_PMBUS_READ_BYTE_EXPECT_NOPEC: // 0x12
				successful_parse_of_record_type = recordProcessor___0x12___processReadByteExpectNoPEC( (t_RECORD_PMBUS_READ_BYTE_EXPECT_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_WORD_EXPECT_NOPEC: // 0x13
				successful_parse_of_record_type = recordProcessor___0x13___processReadWordExpectNoPEC( (t_RECORD_PMBUS_READ_WORD_EXPECT_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_BLOCK_EXPECT_NOPEC: // 0x14
				successful_parse_of_record_type = FAILURE; // Unsupported Record Type
				break;
			case RECORD_TYPE_PMBUS_READ_BYTE_LOOP_MASK_NOPEC: // 0x15
				successful_parse_of_record_type = recordProcessor___0x15___processReadByteLoopMaskNoPEC( (t_RECORD_PMBUS_READ_BYTE_LOOP_MASK_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_WORD_LOOP_MASK_NOPEC: // 0x16
				successful_parse_of_record_type = recordProcessor___0x16___processReadWordLoopMaskNoPEC( (t_RECORD_PMBUS_READ_WORD_LOOP_MASK_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_SEND_BYTE_NOPEC: // 0x17
				successful_parse_of_record_type = recordProcessor___0x17___processSendByteNoPEC( (t_RECORD_PMBUS_SEND_BYTE_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_EVENT: // 0x18
				successful_parse_of_record_type = recordProcessor___0x18___processEvent( (t_RECORD_EVENT*) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_BYTE_EXPECT_MASK_NOPEC: // 0x19
				successful_parse_of_record_type = recordProcessor___0x19___processReadByteExpectMaskNoPEC( (t_RECORD_PMBUS_READ_BYTE_EXPECT_MASK_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_WORD_EXPECT_MASK_NOPEC: //0x1A
				successful_parse_of_record_type = recordProcessor___0x1A___processReadWordExpectMaskNoPEC( (t_RECORD_PMBUS_READ_WORD_EXPECT_MASK_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_VARIABLE_META_DATA: // 0x1B
				successful_parse_of_record_type = recordProcessor___0x1B___processVariableMetaData( (t_RECORD_VARIABLE_META_DATA*) record_to_process);
				break;
			case RECORD_TYPE_MODIFY_WORD_NOPEC: // 0x1C
				successful_parse_of_record_type = recordProcessor___0x1C___modifyWordNoPEC( (t_RECORD_PMBUS_MODIFY_WORD_NO_PEC *) record_to_process);
				break;
			case RECORD_TYPE_MODIFY_BYTE_NOPEC: // 0x1D
				successful_parse_of_record_type = recordProcessor___0x1D___modifyByteNoPEC( (t_RECORD_PMBUS_MODIFY_BYTE_NO_PEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_WRITE_EE_DATA: // 0x1E
				successful_parse_of_record_type = recordProcessor___0x1E___writeNvmData( (t_RECORD_NVM_DATA *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_AND_VERIFY_EE_DATA: // 0x1F
				successful_parse_of_record_type = recordProcessor___0x1F___read_then_verifyNvmData( (t_RECORD_NVM_DATA *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_MODIFY_BYTE: // 0x20
				successful_parse_of_record_type = recordProcessor___0x20___modifyByteOptionalPEC( (t_RECORD_PMBUS_MODIFY_BYTE *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_MODIFY_WORD: // 0x21
				successful_parse_of_record_type = recordProcessor___0x21___modifyWordOptionalPEC( (t_RECORD_PMBUS_MODIFY_WORD *) record_to_process);
				break;
			case RECORD_TYPE_END_OF_RECORDS: // 0x22
				free(record_to_process);
				return SUCCESS;
				break;
			default:
				successful_parse_of_record_type = FAILURE; // Unknown Instruction, report a failure
				break;
		}
		
		if(recordType_of_record_to_process == RECORD_TYPE_NVM_DATA)
		{
		}
	    else if(successful_parse_of_record_type == 0)
		    free(record_to_process);
	    else
	    	free(record_to_process);
	}

	return successful_parse_of_record_type;
}

/********************************************************************
 * Function:        uint8_t verifyRecordsOnDemand(_InCircuitProgrammingRecordTypeListItem_p node, uint16_t length);
 *
 * PreCondition:    None
 * Input:           Function to get records one by one, skips writes
 * Output:          Returns SUCCESS (1) or FAILURE (0) depending on the status of parsing ALL the record types
 * Overview:        Processes all the records until the function to get records returns null
 * 					Records are freed as processed.
 * Note:            None
 *******************************************************************/
uint8_t verifyRecordsOnDemand(pRecordHeaderLengthAndType (*getRecord)(void))
{
	pRecordHeaderLengthAndType record_to_process;
	uint16_t recordType_of_record_to_process;
	uint8_t successful_parse_of_record_type = SUCCESS;

	while ((record_to_process = getRecord()) != NULL && successful_parse_of_record_type == SUCCESS)
    {
		recordType_of_record_to_process = record_to_process->RecordType;
		
	    switch(recordType_of_record_to_process)
		{
			case RECORD_TYPE_PMBUS_WRITE_BYTE: // 0x01
				if (((t_RECORD_PMBUS_WRITE_BYTE *) record_to_process)->detailedRecordHeader.CommandCode != 0xBE)
					successful_parse_of_record_type = recordProcessor___0x01___processWriteByteOptionalPEC( (t_RECORD_PMBUS_WRITE_BYTE *) record_to_process);
				else
					smbusPec__->writeByte((uint8_t) ((t_RECORD_PMBUS_WRITE_BYTE *)record_to_process)->detailedRecordHeader.DeviceAddress, 0xBD, 0);
				break;
			case RECORD_TYPE_PMBUS_WRITE_WORD: // 0x02
				successful_parse_of_record_type = recordProcessor___0x02___processWriteWordOptionalPEC( (t_RECORD_PMBUS_WRITE_WORD *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_WRITE_BLOCK: // 0x03
				successful_parse_of_record_type = FAILURE; // Unsupported Record Type
				break;
			case RECORD_TYPE_PMBUS_READ_BYTE_EXPECT: // 0x04
				successful_parse_of_record_type = recordProcessor___0x04___processReadByteExpectOptionalPEC( (t_RECORD_PMBUS_READ_BYTE_EXPECT *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_WORD_EXPECT: // 0x05
				successful_parse_of_record_type = recordProcessor___0x05___processReadWordExpectOptionalPEC( (t_RECORD_PMBUS_READ_WORD_EXPECT *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_BLOCK_EXPECT: // 0x06
				successful_parse_of_record_type = FAILURE; // Unsupported Record Type
				break;
			case RECORD_TYPE_DEVICE_ADDRESS: // 0x07 -- OBSOLETED
				successful_parse_of_record_type = SUCCESS; // Do nothing for this record type, but do not fail
				break;
			case RECORD_TYPE_PACKING_CODE: // 0x08 -- OBSOLETED
				successful_parse_of_record_type = SUCCESS; // Do nothing for this record type, but do not fail
				break;
			case RECORD_TYPE_NVM_DATA: // 0x09 -- FUNCTIONALITY CHANGED 25/01/2011
				successful_parse_of_record_type = recordProcessor___0x09___bufferNVMData( (t_RECORD_NVM_DATA *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_BYTE_LOOP_MASK: // 0x0A
				successful_parse_of_record_type = recordProcessor___0x0A___processReadByteLoopMaskOptionalPEC( (t_RECORD_PMBUS_READ_BYTE_LOOP_MASK *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_WORD_LOOP_MASK: //0x0B
				successful_parse_of_record_type = recordProcessor___0x0B___processReadWordLoopMaskOptionalPEC( (t_RECORD_PMBUS_READ_WORD_LOOP_MASK *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_POLL_UNTIL_ACK_NOPEC: // 0x0C
				successful_parse_of_record_type = recordProcessor___0x0C___processPollReadByteUntilAckNoPEC( (t_RECORD_PMBUS_POLL_READ_BYTE_UNTIL_ACK *) record_to_process);
				break;
			case RECORD_TYPE_DELAY_MS: // 0x0D
				successful_parse_of_record_type = recordProcessor___0x0D___processDelayMs( (t_RECORD_DELAY_MS *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_SEND_BYTE: //0x0E
				successful_parse_of_record_type = recordProcessor___0x0E___processSendByteOptionalPEC( (t_RECORD_PMBUS_SEND_BYTE *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_WRITE_BYTE_NOPEC: // 0x0F
				successful_parse_of_record_type = recordProcessor___0x0F___processWriteByteNoPEC( (t_RECORD_PMBUS_WRITE_BYTE_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_WRITE_WORD_NOPEC: // 0x10
				successful_parse_of_record_type = recordProcessor___0x10___processWriteWordNoPEC( (t_RECORD_PMBUS_WRITE_WORD_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_WRITE_BLOCK_NOPEC: // 0x11
				successful_parse_of_record_type = FAILURE; // Unsupported Record Type
				break;
			case RECORD_TYPE_PMBUS_READ_BYTE_EXPECT_NOPEC: // 0x12
				successful_parse_of_record_type = recordProcessor___0x12___processReadByteExpectNoPEC( (t_RECORD_PMBUS_READ_BYTE_EXPECT_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_WORD_EXPECT_NOPEC: // 0x13
				successful_parse_of_record_type = recordProcessor___0x13___processReadWordExpectNoPEC( (t_RECORD_PMBUS_READ_WORD_EXPECT_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_BLOCK_EXPECT_NOPEC: // 0x14
				successful_parse_of_record_type = FAILURE; // Unsupported Record Type
				break;
			case RECORD_TYPE_PMBUS_READ_BYTE_LOOP_MASK_NOPEC: // 0x15
				successful_parse_of_record_type = recordProcessor___0x15___processReadByteLoopMaskNoPEC( (t_RECORD_PMBUS_READ_BYTE_LOOP_MASK_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_WORD_LOOP_MASK_NOPEC: // 0x16
				successful_parse_of_record_type = recordProcessor___0x16___processReadWordLoopMaskNoPEC( (t_RECORD_PMBUS_READ_WORD_LOOP_MASK_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_SEND_BYTE_NOPEC: // 0x17
				successful_parse_of_record_type = recordProcessor___0x17___processSendByteNoPEC( (t_RECORD_PMBUS_SEND_BYTE_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_EVENT: // 0x18
				successful_parse_of_record_type = recordProcessor___0x18___processEvent( (t_RECORD_EVENT*) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_BYTE_EXPECT_MASK_NOPEC: // 0x19
				successful_parse_of_record_type = recordProcessor___0x19___processReadByteExpectMaskNoPEC( (t_RECORD_PMBUS_READ_BYTE_EXPECT_MASK_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_READ_WORD_EXPECT_MASK_NOPEC: //0x1A
				successful_parse_of_record_type = recordProcessor___0x1A___processReadWordExpectMaskNoPEC( (t_RECORD_PMBUS_READ_WORD_EXPECT_MASK_NOPEC *) record_to_process);
				break;
			case RECORD_TYPE_VARIABLE_META_DATA: // 0x1B
				successful_parse_of_record_type = recordProcessor___0x1B___processVariableMetaData( (t_RECORD_VARIABLE_META_DATA*) record_to_process);
				break;
			case RECORD_TYPE_MODIFY_WORD_NOPEC: // 0x1C
				successful_parse_of_record_type = recordProcessor___0x1C___modifyWordNoPEC( (t_RECORD_PMBUS_MODIFY_WORD_NO_PEC *) record_to_process);
				break;
			case RECORD_TYPE_MODIFY_BYTE_NOPEC: // 0x1D
				successful_parse_of_record_type = recordProcessor___0x1D___modifyByteNoPEC( (t_RECORD_PMBUS_MODIFY_BYTE_NO_PEC *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_WRITE_EE_DATA: // 0x1E
				//successful_parse_of_record_type = recordProcessor___0x1E___writeNvmData( (t_RECORD_NVM_DATA *) record_to_process);
				smbusPec__->writeByte((uint8_t) ((t_RECORD_NVM_DATA*)record_to_process)->detailedRecordHeader.DeviceAddress, 0xBD, 0);
				break;
			case RECORD_TYPE_PMBUS_READ_AND_VERIFY_EE_DATA: // 0x1F
				successful_parse_of_record_type = recordProcessor___0x1F___read_then_verifyNvmData( (t_RECORD_NVM_DATA *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_MODIFY_BYTE: // 0x20
				successful_parse_of_record_type = recordProcessor___0x20___modifyByteOptionalPEC( (t_RECORD_PMBUS_MODIFY_BYTE *) record_to_process);
				break;
			case RECORD_TYPE_PMBUS_MODIFY_WORD: // 0x21
				successful_parse_of_record_type = recordProcessor___0x21___modifyWordOptionalPEC( (t_RECORD_PMBUS_MODIFY_WORD *) record_to_process);
				break;
			case RECORD_TYPE_END_OF_RECORDS: // 0x22
				free(record_to_process);
				return SUCCESS;
				break;
			default:
				successful_parse_of_record_type = FAILURE; // Unknown Instruction, report a failure
				break;
		}
		
		if(recordType_of_record_to_process == RECORD_TYPE_NVM_DATA)
		{
		}
	    else if(successful_parse_of_record_type == 0)
		    free(record_to_process);
	    else
	    	free(record_to_process);
	}

	return successful_parse_of_record_type;
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x01___processWriteByteOptionalPEC(t_RECORD_PMBUS_WRITE_BYTE*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_PMBUS_WRITE_BYTE pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_PMBUS_WRITE_BYTE record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x01___processWriteByteOptionalPEC(t_RECORD_PMBUS_WRITE_BYTE* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("WriteByteOptionalPEC %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->dataByte);
#else
#ifdef DEBUG_PRINT
	printf("WriteByteOptionalPEC %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->dataByte);
#endif
			
			
	if (pRecord->detailedRecordHeader.UsePec)
		smbusPec__->writeByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode,
						  				pRecord->dataByte);
	else
		smbusNoPec__->writeByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode,
						  				pRecord->dataByte);
#endif
#endif
return SUCCESS;
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x02___processWriteWordOptionalPEC(t_RECORD_PMBUS_WRITE_WORD*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_PMBUS_WRITE_WORD pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_PMBUS_WRITE_WORD record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x02___processWriteWordOptionalPEC(t_RECORD_PMBUS_WRITE_WORD* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("WriteWordOptionalPEC %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->dataWord);
#else
#ifdef DEBUG_PRINT
	printf("WriteWordOptionalPEC %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->dataWord);
#endif
	if (pRecord->detailedRecordHeader.UsePec)
		smbusPec__->writeWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode,
						  				pRecord->dataWord);
	else
		smbusNoPec__->writeWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode,
						  				pRecord->dataWord);
#endif
#endif
return SUCCESS;
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x04___processReadByteExpectOptionalPEC(t_RECORD_smbus_read_byte_EXPECT*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_smbus_read_byte_EXPECT pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_smbus_read_byte_EXPECT record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x04___processReadByteExpectOptionalPEC(t_RECORD_PMBUS_READ_BYTE_EXPECT* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("ReadByteExpectOptionalPEC %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode);
	return SUCCESS;
#else
	uint8_t actualByteValue;
	if (pRecord->detailedRecordHeader.UsePec)
		actualByteValue = smbusPec__->readByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);
	else
		actualByteValue = smbusNoPec__->readByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);

#ifdef DEBUG_PRINT
	printf("ReadByteExpectOptionalPEC %x %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, actualByteValue, pRecord->expectedDataByte);
#endif

	return (actualByteValue != pRecord->expectedDataByte) ? FAILURE : SUCCESS;
#endif
#endif
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x05___processReadWordExpectOptionalPEC(t_RECORD_PMBUS_READ_WORD_EXPECT*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_PMBUS_READ_WORD_EXPECT pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_PMBUS_READ_WORD_EXPECT record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x05___processReadWordExpectOptionalPEC(t_RECORD_PMBUS_READ_WORD_EXPECT* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("ReadWordExpectOptionalPEC %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode);
	return SUCCESS;
#else
	uint16_t actualWordValue;
	if(pRecord->detailedRecordHeader.UsePec)
		actualWordValue = smbusPec__->readWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);
	else
		actualWordValue = smbusNoPec__->readWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);

#ifdef DEBUG_PRINT
	printf("ReadWordExpectOptionalPEC %x %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, actualWordValue, pRecord->expectedDataWord);
#endif

	return (actualWordValue != pRecord->expectedDataWord) ? FAILURE : SUCCESS;
#endif
#endif
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x09___bufferNVMData(t_RECORD_NVM_DATA*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_NVM_DATA pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_NVM_DATA record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x09___bufferNVMData(t_RECORD_NVM_DATA* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("NVMData %x %x %d\n",pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->baseRecordHeader.Length);
	return SUCCESS;
#endif
#ifdef DEBUG_PRINT
	printf("NVMData %x %x %d\n",pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->baseRecordHeader.Length);
#endif
#endif

	return bufferNvmData(pRecord);
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x0A___processReadByteLoopMaskOptionalPEC(t_RECORD_smbus_read_byte_LOOP_MASK*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_smbus_read_byte_LOOP_MASK pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_smbus_read_byte_LOOP_MASK record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x0A___processReadByteLoopMaskOptionalPEC(t_RECORD_PMBUS_READ_BYTE_LOOP_MASK* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("ReadByteLoopMaskOptionalPEC %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->byteMask);
#else
	uint8_t actualByteValue;
	uint8_t actualByteValueWithMask;
	uint8_t expectedByteValueWithMask;
	uint8_t success = FAILURE;
	do
	{
		if (pRecord->detailedRecordHeader.UsePec)
			actualByteValue = smbusPec__->readByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);
		else
			actualByteValue = smbusNoPec__->readByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);

		actualByteValueWithMask = (actualByteValue & pRecord->byteMask);
		expectedByteValueWithMask = (pRecord->expectedDataByte & pRecord->byteMask);
		success = (actualByteValueWithMask == expectedByteValueWithMask);
#ifdef DEBUG_PRINT
	printf("ReadByteLoopMaskOptionalPEC %x %x %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->byteMask, actualByteValueWithMask, expectedByteValueWithMask);Serial.println(buffer);
#endif
	} while(success == FAILURE);
#endif
#endif

	return SUCCESS;

}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x0B___processReadWordLoopMaskOptionalPEC(t_RECORD_PMBUS_READ_WORD_LOOP_MASK*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_PMBUS_READ_WORD_LOOP_MASK pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_PMBUS_READ_WORD_LOOP_MASK record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x0B___processReadWordLoopMaskOptionalPEC(t_RECORD_PMBUS_READ_WORD_LOOP_MASK* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("WriteWordLoopMaskOptionalPEC %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->wordMask);
#else
	uint16_t actualWordValue;
	uint16_t actualWordValueWithMask;
	uint16_t expectedWordValueWithMask;
	uint8_t success = FAILURE;
	do
	{
		if (pRecord->detailedRecordHeader.UsePec)
			actualWordValue = smbusPec__->readWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);
		else
			actualWordValue = smbusNoPec__->readWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);

		actualWordValueWithMask = (actualWordValue & pRecord->wordMask);
		expectedWordValueWithMask = (pRecord->expectedDataWord & pRecord->wordMask);
		success = (actualWordValueWithMask == expectedWordValueWithMask);
#ifdef DEBUG_PRINT
	printf("WriteWordLoopMaskOptionalPEC %x %x %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->wordMask, actualWordValueWithMask, expectedWordValueWithMask);
#endif
	} while(success == FAILURE);
#endif
#endif
	return SUCCESS;
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x0C___processPollReadByteUntilAckNoPEC(t_RECORD_PMBUS_POLL_READ_BYTE_UNTIL_ACK*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_PMBUS_POLL_READ_BYTE_UNTIL_ACK pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_PMBUS_POLL_READ_BYTE_UNTIL_ACK record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x0C___processPollReadByteUntilAckNoPEC(t_RECORD_PMBUS_POLL_READ_BYTE_UNTIL_ACK* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("PollReadByteUntilAckNoPEC %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode);
#else
#ifdef DEBUG_PRINT
	printf("PollReadByteUntilAckNoPEC %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode);
#endif
	smbusNoPec__->waitForAck((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);
#endif
#endif
	return SUCCESS;
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x0D___processDelayMs(t_RECORD_DELAY_MS*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_DELAY_MS pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_DELAY_MS record type
 * Note:
 * 					Clock Frequency	= 80MHZ
 * 					Clock Period		= 12.5 nano-seconds
 * 					Disregarding the ~12 cycles to enter function, this delay is approximate and weighted to be longer with uncertainty
 * 					So 1ms of delay is 80,000 clocks
 * 					Each inner for loop iteration is 3 cycles + inside for loop, so 4 clocks per Nop() for-loop
 *					20K iterations of a 4 clock cycle loop at 80MHz is 1ms
 * 					Do that inner 1ms delay loop as many times as specified in the t_RECORD_DELAY_MS Record Header
 *******************************************************************/
uint8_t recordProcessor___0x0D___processDelayMs(t_RECORD_DELAY_MS* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("DelayMs %x\n",
			pRecord->numMs);
#else
	uint16_t number_of_milliseconds_to_delay = pRecord->numMs;
	uint16_t ms_count_i;
	uint16_t delay_i;
#ifdef DEBUG_PRINT
	printf("DelayMs %x\n",
			pRecord->numMs);
#endif

	// Do n number of millisecond delays
	for(ms_count_i=0; ms_count_i < number_of_milliseconds_to_delay; ms_count_i++)
	{
		for(delay_i=0; delay_i < 20000; delay_i++) { asm("nop"); } // This takes 4 clock cycles * 20,000 at 80MHz which is 1 millisecond
	}
#endif
#endif
	return SUCCESS;
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x0E___processSendByteOptionalPEC(t_RECORD_PMBUS_SEND_BYTE*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_PMBUS_SEND_BYTE pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_PMBUS_SEND_BYTE record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x0E___processSendByteOptionalPEC(t_RECORD_PMBUS_SEND_BYTE* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("SendByteOptionalPEC %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode);
#else
#ifdef DEBUG_PRINT
	printf("SendByteOptionalPEC %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode);
#endif
	if (pRecord->detailedRecordHeader.UsePec)
		smbusPec__->sendByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);
	else
		smbusNoPec__->sendByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);
#endif
#endif
return SUCCESS;
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x0F___processWriteByteNoPEC(t_RECORD_PMBUS_WRITE_BYTE_NOPEC*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_PMBUS_WRITE_BYTE_NOPEC pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_PMBUS_WRITE_BYTE_NOPEC record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x0F___processWriteByteNoPEC(t_RECORD_PMBUS_WRITE_BYTE_NOPEC* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("WriteByteNoPec %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->dataByte);
#else
#ifdef DEBUG_PRINT
	printf("WriteByteNoPec %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->dataByte);
#endif
	smbusNoPec__->writeByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode,
						  				pRecord->dataByte);
#endif
#endif
	return SUCCESS;
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x10___processWriteWordNoPEC(t_RECORD_PMBUS_WRITE_WORD_NOPEC*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_PMBUS_WRITE_WORD_NOPEC pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_PMBUS_WRITE_WORD_NOPEC record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x10___processWriteWordNoPEC(t_RECORD_PMBUS_WRITE_WORD_NOPEC* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("WriteWordNoPec %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->dataWord);
#else
#ifdef DEBUG_PRINT
	printf("WriteWordNoPec %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->dataWord);
#endif
	smbusNoPec__->writeWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode,
						  				pRecord->dataWord);
#endif
#endif
return SUCCESS;
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x12___processReadByteExpectNoPEC(t_RECORD_smbus_read_byte_EXPECT_NOPEC*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_smbus_read_byte_EXPECT_NOPEC pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_smbus_read_byte_EXPECT_NOPEC record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x12___processReadByteExpectNoPEC(t_RECORD_PMBUS_READ_BYTE_EXPECT_NOPEC* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("ReadByteExpectNoPEC %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode);
	return SUCCESS;
#else
	uint8_t actualByteValue;
	actualByteValue = smbusNoPec__->readByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);
#ifdef DEBUG_PRINT
	printf("ReadByteExpectNoPEC %x %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, actualByteValue, pRecord->expectedDataByte);
#endif
 	return (actualByteValue != pRecord->expectedDataByte) ? FAILURE : SUCCESS;
#endif
#endif
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x13___processReadWordExpectNoPEC(t_RECORD_PMBUS_READ_WORD_EXPECT_NOPEC*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_PMBUS_READ_WORD_EXPECT_NOPEC pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_PMBUS_READ_WORD_EXPECT_NOPEC record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x13___processReadWordExpectNoPEC(t_RECORD_PMBUS_READ_WORD_EXPECT_NOPEC* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("ReadWordEx[ectNoPEC %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode);
	return SUCCESS;
#else
	uint16_t actualWordValue;
	actualWordValue = smbusNoPec__->readWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);

#ifdef DEBUG_PRINT
	printf("ReadWordExpectNoPEC %x %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, actualWordValue, pRecord->expectedDataWord);
#endif
	return (actualWordValue != pRecord->expectedDataWord) ? FAILURE : SUCCESS;
#endif
#endif
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x15___processReadByteLoopMaskNoPEC(t_RECORD_smbus_read_byte_LOOP_MASK_NOPEC*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_smbus_read_byte_LOOP_MASK_NOPEC pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_smbus_read_byte_LOOP_MASK_NOPEC record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x15___processReadByteLoopMaskNoPEC(t_RECORD_PMBUS_READ_BYTE_LOOP_MASK_NOPEC* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("ReadByteLoopMaskNoPEC %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->byteMask);
#else
	uint8_t actualByteValue;
	uint8_t actualByteValueWithMask;
	uint8_t expectedByteValueWithMask;
	uint8_t success = FAILURE;
	do
	{
		actualByteValue = smbusNoPec__->readByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);

		actualByteValueWithMask = (actualByteValue & pRecord->byteMask);
		expectedByteValueWithMask = (pRecord->expectedDataByte & pRecord->byteMask);
		success = (actualByteValueWithMask == expectedByteValueWithMask);
#ifdef DEBUG_PRINT
	printf("ReadByteLoopMaskNoPEC %x %x %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->byteMask, actualByteValueWithMask, expectedByteValueWithMask);
#endif
	} while(success == FAILURE);
#endif
#endif
	return SUCCESS;
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x16___processReadWordLoopMaskNoPEC(t_RECORD_PMBUS_READ_WORD_LOOP_MASK_NOPEC*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_PMBUS_READ_WORD_LOOP_MASK_NOPEC pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_PMBUS_READ_WORD_LOOP_MASK_NOPEC record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x16___processReadWordLoopMaskNoPEC(t_RECORD_PMBUS_READ_WORD_LOOP_MASK_NOPEC* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("ReadWordLoopMaskNoPEC %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->wordMask);
#else
	uint16_t actualWordValue;
	uint16_t actualWordValueWithMask;
	uint16_t expectedWordValueWithMask;
	uint8_t success = FAILURE;
	do
	{
		actualWordValue = smbusNoPec__->readWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);


		actualWordValueWithMask = (actualWordValue & pRecord->wordMask);
		expectedWordValueWithMask = (pRecord->expectedDataWord & pRecord->wordMask);
		success = (actualWordValueWithMask == expectedWordValueWithMask);
#ifdef DEBUG_PRINT
	printf("ReadWordLoopMaskNoPEC %x %x %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->wordMask, actualWordValueWithMask, expectedWordValueWithMask);
#endif
	} while(success == FAILURE);
#endif
#endif
	return SUCCESS;
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x17___processSendByteNoPEC(t_RECORD_PMBUS_SEND_BYTE_NOPEC*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_PMBUS_SEND_BYTE_NOPEC pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_PMBUS_SEND_BYTE_NOPEC record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x17___processSendByteNoPEC(t_RECORD_PMBUS_SEND_BYTE_NOPEC* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("SendByteNoPEC %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode);
#else
#ifdef DEBUG_PRINT
	printf("SendByteNoPEC %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode);
#endif
	smbusNoPec__->sendByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);
#endif
#endif
	return SUCCESS;
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x18___processEvent(t_RECORD_EVENT*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_EVENT pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_EVENT record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x18___processEvent(t_RECORD_EVENT* pRecord)
{
	switch(pRecord->eventId)
	{
		case BEFORE_BEGIN:
			// This event is fired before any commands are issued to program the NVM
			// Potentially do something in your system that needs doing before anything has started
			// Turn on power to the DUT? Confirm user wants to program? Release Write Protect pins?
#ifdef DEBUG_PRINT
			diag_printf("META DATA EVENT: The system is about to begin executing the programming recipe\n");
#endif
			return SUCCESS;
		case BEFORE_INSYSTEM_PROGRAMMING_BEGIN :
#ifdef DEBUG_PRINT
			diag_printf("META DATA EVENT: The system is about to begin in system programming\n");
#endif
			return SUCCESS;
		case SYSTEM_BEFORE_PROGRAM:
			// This event is fired before any commands are issued to program the NVM
			// Potentially do something in your system that needs doing before any programming has started
			// Confirm user wants to program?
#ifdef DEBUG_PRINT
			diag_printf("META DATA EVENT: The system is about to begin writing to the DUT Non-Volatile Memory\n");
#endif
			return SUCCESS;
		case INSYSTEM_CHIP_BEFORE_PROGRAM :
#ifdef DEBUG_PRINT
			diag_printf("META DATA EVENT: The system is about to begin programming a chip\n");
#endif
			return SUCCESS;
		case SYSTEM_BEFORE_VERIFY:
			// This event is fired after programming the system before any commands are issued to verify the NVM
			// Potentially do something in your system that needs doing before any programming has started
			// Tell the user the chip was programmed without error and now it needs verification?
#ifdef DEBUG_PRINT
			diag_printf("META DATA EVENT: The system is about to begin reading from the DUT's Non-Volatile Memory and verifying the contents match what is expected\n");
#endif
			return SUCCESS;
		case INSYSTEM_CHIP_BEFORE_VERIFY :
#ifdef DEBUG_PRINT
			diag_printf("META DATA EVENT: The system is about to verify a chip\n");
#endif
			return SUCCESS;
		case INSYSTEM_CHIP_AFTER_VERIFY:
#ifdef DEBUG_PRINT
			diag_printf("META DATA EVENT: The system has finished verifying a chip\n");
#endif
			return SUCCESS;
		case SYSTEM_AFTER_VERIFY:
#ifdef DEBUG_PRINT
			diag_printf("META DATA EVENT: The system has finished verification\n");
#endif
			return SUCCESS;
		case AFTER_DONE:
			// This event is fired after the end of the entire programming and verification sequence is complete
			// Potentially do something in your system that needs doing before any programming has started
			// Turn off power to the DUT? Tell the user the outcome of the verification?
#ifdef DEBUG_PRINT
			diag_printf("META DATA EVENT: The system is done programming. If you reach this point without error, your device has been programmed and verified successfully\n");
#endif
			return SUCCESS;
		default:
			// This point should never be reached
#ifdef DEBUG_PRINT
			diag_printf("The META DATA EVENT type is not supported in your firmware. You will need to update your programming algorithms\n");
#endif
			return FAILURE;
	}
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x19___processReadByteExpectMaskNoPEC(t_RECORD_smbus_read_byte_EXPECT_MASK_NOPEC*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_smbus_read_byte_EXPECT_MASK_NOPEC pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_smbus_read_byte_EXPECT_MASK_NOPEC record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x19___processReadByteExpectMaskNoPEC(t_RECORD_PMBUS_READ_BYTE_EXPECT_MASK_NOPEC* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("ReadByteExpectMaskNoPEC %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->byteMask);
	return SUCCESS;
#else
	uint8_t actualByteValue;
	uint8_t actualByteValueWithMask;
	uint8_t expectedByteValueWithMask;
	actualByteValue = smbusNoPec__->readByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);
	actualByteValueWithMask = (actualByteValue & pRecord->byteMask);
	expectedByteValueWithMask = (pRecord->expectedDataByte & pRecord->byteMask);
#ifdef DEBUG_PRINT
	printf("ReadByteExpectMaskNoPEC %x %x %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->byteMask, actualByteValueWithMask, expectedByteValueWithMask);
#endif
	return (actualByteValueWithMask != expectedByteValueWithMask) ? FAILURE : SUCCESS;
#endif
#endif
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x1A___processReadWordExpectMaskNoPEC(t_RECORD_PMBUS_READ_WORD_EXPECT_MASK_NOPEC*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_PMBUS_READ_WORD_EXPECT_MASK_NOPEC pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_PMBUS_READ_WORD_EXPECT_MASK_NOPEC record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x1A___processReadWordExpectMaskNoPEC(t_RECORD_PMBUS_READ_WORD_EXPECT_MASK_NOPEC* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("ReadWordExpectMaskNoPEC %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->wordMask);
	return SUCCESS;
#else
	uint16_t actualWordValue;
	uint16_t actualWordValueWithMask;
	uint16_t expectedWordValueWithMask;
	actualWordValue = smbusNoPec__->readWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);

	actualWordValueWithMask = (actualWordValue & pRecord->wordMask);
	expectedWordValueWithMask = (pRecord->expectedDataWord & pRecord->wordMask);
#ifdef DEBUG_PRINT
	printf("ReadWordExpectMaskNoPEC %x %x %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->wordMask, actualWordValueWithMask, expectedWordValueWithMask);
#endif
	return (actualWordValueWithMask != expectedWordValueWithMask) ? FAILURE : SUCCESS;
#endif
#endif
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x1B___processVariableMetaData(t_RECORD_VARIABLE_META_DATA*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_VARIABLE_META_DATA pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_VARIABLE_META_DATA record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x1B___processVariableMetaData(t_RECORD_VARIABLE_META_DATA* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("WriteByteOptionalPEC\n");
	return SUCCESS;
#else
	uint16_t newGlobalBaseAddress;
	uint16_t serialNumberToWrite;
	uint16_t readBackValueOfSerialNumber;
#ifdef DEBUG_PRINT
	printf("WriteByteOptionalPEC\n");
#endif

	switch(pRecord->metaDataType)
	{
		case META_SET_GLOBAL_BASE_ADDRESS:
			// Set the base address that will be used to talk to the system
			//print_debug_string("Setting the Global Base Address");

			// Ensure that the device is responding
			smbusNoPec__->waitForAck(0x5B, 0x00);

			// Un Write-Protect the device
			// This is a shorthand way of writing 0x00 [Write Protect Disabled] to register 0x10 [PMBus WRITE_PROTECT command register] of the device at address 0x5B with a PEC byte of 0xC0
			// This will work even if the device at 0x5B has PEC_REQUIRED enabled
			smbusNoPec__->writeWord(0x5B, 0x10, 0xC000);

			// Write MFR_I2C_BASE_ADDRESS with supplied 16-bit word new global base address
			newGlobalBaseAddress = ((t_RECORD_META_SET_GLOBAL_BASE_ADDRESS*)pRecord)->globalBaseAddressInWordFormat;
			smbusNoPec__->writeWord(0x5B, 0xE6, newGlobalBaseAddress);
			return SUCCESS;

  		case META_OEM_SERIAL_NUMBER:
			// Serialization is not supported by any device other than the LTC2978 at this time
			// You should not execute this code unless your device supports serialization
			//print_debug_string("Setting the Serial Number for the Device [Only supported for the LTC2978");

//			pSerialNumberDetails = (t_RECORD_META_OEM_SERIAL_NUMBER*) pRecord;

			// Ensure the device is responding;
//			smbus_wait_for_ack(0x5B, 0x00);

			// Un Write-Protect the device
			// This is a shorthand way of writing 0x00 [Write Protect Disabled] to register 0x10 [PMBus WRITE_PROTECT command register] of the device at address 0x5B with a PEC byte of 0xC0
			// This will work even if the device at 0x5B has PEC_REQUIRED enabled
//			smbus_write_word(0x5B, 0x10, 0xC000);

			// Set the serial number details to the current device serial number, and increment the variable for the next device to be programmed
//			pSerialNumberDetails->serialNumber = device_serial_number++;

			// Write serial number value (16bit word) to command code 0xF8 with PEC
//			serialNumberToWrite = pSerialNumberDetails->serialNumber;
//			smbus_write_word_with_pec(0x5B, 0xF8, serialNumberToWrite);

			// Read back the serial number value (16bit word) from register 0xF8. PEC is not required for the read
//			readBackValueOfSerialNumber = smbus_read_word(0x5B, 0xF8);

//			if(serialNumberToWrite != readBackValueOfSerialNumber)
//			{
//				return FAILURE; // What was written does not match what was read back
//			}

			// If the code execution reaches this point, then the serial number has been written and verified successfully
			return SUCCESS;
		default:
			// This point should never be reached
			//print_debug_string("The META DATA type is not supported in your firmware. You will need to update your programming algorithms");
			return FAILURE;
	}
#endif
#endif
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x1C___modifyWordNoPEC(t_RECORD_PMBUS_MODIFY_BYTE_NO_PEC*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_PMBUS_MODIFY_BYTE_NO_PEC pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_PMBUS_MODIFY_BYTE_NO_PEC record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x1C___modifyWordNoPEC(t_RECORD_PMBUS_MODIFY_WORD_NO_PEC* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("modifyWordNoPec %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->desiredDataWord);
#else
	uint16_t actualWordValue;
	uint16_t modifiedWordValue;
#ifdef DEBUG_PRINT
	printf("modifyWordNoPec %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->desiredDataWord);
#endif
	actualWordValue = smbusNoPec__->readWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);

	modifiedWordValue = (actualWordValue & (~pRecord->wordMask));
	smbusNoPec__->writeWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode,
				          				modifiedWordValue | (pRecord->desiredDataWord & pRecord->wordMask));
#endif
#endif
	return SUCCESS;
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x1D___modifyByteNoPEC(t_RECORD_PMBUS_MODIFY_BYTE*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_PMBUS_MODIFY_BYTE pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_PMBUS_MODIFY_BYTE record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x1D___modifyByteNoPEC(t_RECORD_PMBUS_MODIFY_BYTE_NO_PEC* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("modifyByteNoPEC %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->desiredDataByte);
#else
	uint8_t actualByteValue;
	uint8_t modifiedByteValue;
#ifdef DEBUG_PRINT
	printf("modifyByteNoPEC %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->desiredDataByte);
#endif
	actualByteValue = smbusNoPec__->readByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);

	modifiedByteValue = (actualByteValue & (~pRecord->byteMask));
	smbusNoPec__->writeByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode,
				          				modifiedByteValue | (pRecord->desiredDataByte & pRecord->byteMask));
#endif
#endif
	return SUCCESS;
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x1E___writeNvmData(t_RECORD_NVM_DATA*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_NVM_DATA pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_NVM_DATA record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x1E___writeNvmData(t_RECORD_NVM_DATA* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("WriteNvmData\n");
	return SUCCESS;
#else
#ifdef DEBUG_PRINT
	printf("WriteNvmData\n");
#endif
	return writeNvmData(pRecord);
#endif
#endif
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x1F___read_then_verifyNvmData(t_RECORD_NVM_DATA*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_NVM_DATA pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_NVM_DATA record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x1F___read_then_verifyNvmData(t_RECORD_NVM_DATA* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("read_then_verifyNvmData \n");
	return SUCCESS;
#else
#ifdef DEBUG_PRINT
	printf("read_then_verifyNvmData \n");
#endif
	return !read_then_verifyNvmData(pRecord);
#endif
#endif
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x20___modifyByteOptionalPEC(t_RECORD_PMBUS_MODIFY_BYTE*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_PMBUS_MODIFY_BYTE pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_PMBUS_MODIFY_BYTE record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x20___modifyByteOptionalPEC(t_RECORD_PMBUS_MODIFY_BYTE* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("modifyByteOptionalPEC %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->desiredDataByte);
#else
	uint8_t actualByteValue;
	uint8_t modifiedByteValue;
#ifdef DEBUG_PRINT
	printf("modifyByteOptionalPEC %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->desiredDataByte);
#endif
	if(pRecord->detailedRecordHeader.UsePec)
		actualByteValue = smbusPec__->readByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);
	else
		actualByteValue = smbusNoPec__->readByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);

	modifiedByteValue = (actualByteValue & (~pRecord->byteMask));
	if (pRecord->detailedRecordHeader.UsePec)
		smbusPec__->writeByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode,
				          				modifiedByteValue | (pRecord->desiredDataByte & pRecord->byteMask));
	else
		smbusNoPec__->writeByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode,
				          				modifiedByteValue | (pRecord->desiredDataByte & modifiedByteValue));
#endif
#endif
	return SUCCESS;
}

/********************************************************************
 * Function:        uint8_t recordProcessor___0x21___modifyWordOptionalPEC(t_RECORD_PMBUS_MODIFY_WORD*);
 *
 * PreCondition:    None
 * Input:           A t_RECORD_PMBUS_MODIFY_WORD pointer reference
 * Output:          A 1 is returned on success and a 0 is returned on failure
 * Overview:        Processes the t_RECORD_PMBUS_MODIFY_WORD record type
 * Note:            More detailed information may be available in the PDF
 *******************************************************************/
uint8_t recordProcessor___0x21___modifyWordOptionalPEC(t_RECORD_PMBUS_MODIFY_WORD* pRecord)
{
#ifdef DEBUG_SILENT
	return SUCCESS;
#else
#ifdef DEBUG_PROCESSING
	printf("modifyWordOptionalPEC %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->desiredDataWord);
#else
	uint16_t actualWordValue;
	uint16_t modifiedWordValue;
#ifdef DEBUG_PRINT
	printf("modifyWordOptionalPEC %x %x %x\n",
			pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode, pRecord->desiredDataWord);
#endif
	if (pRecord->detailedRecordHeader.UsePec)
		actualWordValue = smbusPec__->readWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);
	else
		actualWordValue = smbusNoPec__->readWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode);

	modifiedWordValue = (actualWordValue & (~pRecord->wordMask));
	if (pRecord->detailedRecordHeader.UsePec)
		smbusPec__->writeWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode,
				          				modifiedWordValue | (pRecord->desiredDataWord & pRecord->wordMask));
	else
		smbusNoPec__->writeWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
				          				pRecord->detailedRecordHeader.CommandCode,
				          				modifiedWordValue | (pRecord->desiredDataWord & pRecord->wordMask));
#endif
#endif
return SUCCESS;
}
