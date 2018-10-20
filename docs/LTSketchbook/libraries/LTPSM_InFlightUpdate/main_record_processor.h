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

#ifndef MAIN_RECORD_PROCESSOR_H_
#define MAIN_RECORD_PROCESSOR_H_

#include <stdlib.h>
#include <stdint.h>
#include "../LT_PMBUS/LT_SMBusNoPec.h"
#include "../LT_PMBUS/LT_SMBusPec.h"
#include "record_type_definitions.h"									/* Record Type Definitions */
#include "in_circuit_programming_record_type_list_item_definition.h"	/* Linked List Structure for buffering all records before programming from Intel HEX file */
#include "nvm_data_helpers.h"


extern _InCircuitProgrammingRecordTypeListItem_p ICP_RTLI_Root;

extern LT_SMBusNoPec* smbusNoPec__;
extern LT_SMBusPec* smbusPec__;

extern uint8_t processRecords(void);
extern uint8_t processRecordArray(ppRecordHeaderLengthAndType records, uint16_t length);
extern uint8_t processRecordsOnDemand(pRecordHeaderLengthAndType (*getRecord)(void));
extern uint8_t verifyRecordsOnDemand(pRecordHeaderLengthAndType (*getRecord)(void));
extern void InitializeRecordTypeList(void);
extern void ReleaseRecordTypeList(void);
extern _InCircuitProgrammingRecordTypeListItem_p ICP_RTLI_CreateNew(pRecordHeaderLengthAndType);
extern void ICP_RTLI_Add(pRecordHeaderLengthAndType, _InCircuitProgrammingRecordTypeListItem_t*);

#endif /* MAIN_RECORD_PROCESSOR_H_ */
