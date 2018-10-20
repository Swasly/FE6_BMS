/*!
LTC PMBus Support

@verbatim

This PMBus code does not cover the entire PMBus command set. The intention
is to cover the common commands. It was written for LTC PMBus devices,
and may not perform properly with other PMBus devices.

@endverbatim

REVISION HISTORY
$Revision:  $
$Date: $

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

/*! @file
    @ingroup LT_I2CBus
    Library Header File for LT_PMBus
*/

#ifndef PMBUS_H_
#define PMBUS_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "LT_SMBus.h"
#include "LT_SMBusGroup.h"
#include "LT_PMBusMath.h"

#define SUCCESS          		1
#define FAILURE          		0

#define PAGE            		0x00
#define OPERATION				0x01
#define ON_OFF_CONFIG			0x02
#define CLEAR_FAULTS			0x03
#define PAGE_PLUS_WRITE			0x05
#define PAGE_PLUS_READ			0x06
#define WRITE_PROTECT			0x10
#define STORE_USER_ALL			0x15
#define RESTORE_USER_ALL		0x16
#define SMBALERT_MASK			0x1B
#define VOUT_MODE 				0x20
#define VOUT_COMMAND			0x21
#define VOUT_MAX				0x24
#define VOUT_MARGIN_HIGH		0x25
#define VOUT_MARGIN_LOW			0x26
#define VOUT_OV_FAULT_LIMIT		0x40
#define VOUT_OV_WARN_LIMIT		0x42
#define VOUT_UV_WARN_LIMIT		0x43
#define VOUT_UV_FAULT_LIMIT		0x44
#define IOUT_OC_FAULT_LIMIT		0x46
#define IOUT_OC_WARN_LIMIT		0x4A
#define OT_FAULT_LIMIT			0x4F
#define OT_WARN_LIMIT			0x51
#define UT_FAULT_LIMIT			0x53
#define UT_WARN_LIMIT			0x52
#define VIN_OV_FAULT_LIMIT		0x55
#define VIN_OV_WARN_LIMIT		0x57
#define VIN_UV_WARN_LIMIT		0x58
#define VIN_UV_FAULT_LIMIT		0x59
#define TON_DELAY				0x60
#define TON_RISE				0x61
#define TON_MAX_FAULT_LIMIT		0x62
#define TON_MAX_FAULT_RESPONSE	0x63
#define TOFF_DELAY				0x64
#define TOFF_FALL				0x65
#define TOFF_MAX_WARN_LIMIT		0x66
#define IIN_OC_WARN_LIMIT		0x5D
#define STATUS_BYTE				0x78
#define STATUS_WORD				0x79
#define STATUS_VOUT				0x7A
#define STATUS_IOUT				0x7B
#define STATUS_INPUT			0x7C
#define STATUS_TEMP				0X7D
#define STATUS_CML				0x7E
#define STATUS_MFR_SPEC			0x80
#define READ_VIN        		0x88
#define READ_IIN        		0x89
#define READ_VOUT       		0x8B
#define READ_IOUT       		0x8C
#define READ_OTEMP				0x8D	// This is internal for LTC2978
#define READ_ITEMP				0x8E
#define READ_POUT				0x96
#define READ_PIN				0x97
#define MFR_MODEL				0x9A	// LTC38XX only
#define MFR_REVISION            0x9B
#define PMBUS_REVISION			0x98
#define MFR_CONFIG_LTC2974		0xD0
#define MFR_CONFIG_ALL_LTC3880  0xD1
#define MFR_CONFIG_ALL_LTC3882  0xD1
#define MFR_CONFIG_ALL_LTC3883  0xD1
#define MFR_CONFIG_ALL_LTC2974  0xD1
#define MFR_CONFIG_ALL_LTC2977  0xD1
#define MFR_CONFIG_ALL_LTC2978  0xD1
#define MFR_WATCHDOG_T_FIRST	0xE2
#define MFR_WATCHDOG_T			0xE3
#define MFR_PADS				0xE5
#define MFR_SPECIAL_ID			0xE7
#define MFR_FAULT_LOG_STORE     0xEA
#define MFR_FAULT_LOG_RESTORE	0xEB
#define MFR_FAULT_LOG_CLEAR		0xEC
#define MFR_FAULT_LOG_STATUS	0xED	// LTC29XX only
#define MFR_FAULT_LOG           0xEE
#define MFR_COMMON				0xEF
#define MFR_TEMP_1_GAIN			0xF8
#define MFR_RESET				0xFD

#define LTC3880_SW_NONE					0x0001
#define LTC3880_SW_CML					0x0002
#define LTC3880_SW_TEMP					0x0004
#define LTC3880_SW_IOUT_OC				0x0010
#define LTC3880_SW_VOUT_OV				0x0020
#define LTC3880_SW_OFF					0x0040
#define LTC3880_SW_BUSY					0x0080
#define LTC3880_SW_PGOODB				0x0800
#define LTC3880_SW_MFR					0x1000
#define LTC3880_SW_INPUT				0x2000
#define LTC3880_SW_IOUT_POUT			0x4000
#define LTC3880_SW_VOUT					0x8000

#define LTC3880_CFGALL_RUN_CLR  		0x0001
#define LTC3880_CFGALL_CLK_STR  		0x0002
#define LTC3880_CFGALL_MASK_PLL_FLT		0x0004
#define LTC3880_CFGALL_IGNORE_RCFG		0x0040
#define LTC3880_CFGALL_FLAG				0x0080

#define LTC3880_SMFR_EXT_GPIO   		0x0001
#define LTC3880_SMFR_VDD_UV_OV  		0x0004
#define LTC3880_SMFR_FAULT_LOG  		0x0008
#define LTC3880_SMFR_PLL_UNLOCK 		0x0010
#define LTC3880_SMFR_NVM_CRC    		0x0020
#define LTC3880_SMFR_TEMP_WARN  		0x0040
#define LTC3880_SMFR_TEMP_FAULT 		0x0080

#define LTC3882_CFGALL_RUN_CLR_RISE  	0x0001
#define LTC3882_CFGALL_CLK_STR  		0x0002
#define LTC3882_CFGALL_PEC				0x0004
#define LTC3880_CFGALL_255MS_TMO		0x0008
#define LTC3882_CFGALL_DISABLE_SYNC		0x0010
#define LTC3882_CFGALL_IGNORE_RCFG		0x0040
#define LTC3882_CFGALL_FLAG				0x0080

#define LTC3882_SMFR_EXT_GPIO   		0x0001
#define LTC3882_SMFR_SHORT_CYCLE  		0x0002
#define LTC3882_SMFR_FAULT_LOG  		0x0008
#define LTC3882_SMFR_PLL_UNLOCK 		0x0010
#define LTC3882_SMFR_NVM_CRC    		0x0020
#define LTC3882_SMFR_TEMP_WARN  		0x0040
#define LTC3882_SMFR_TEMP_FAULT 		0x0080

#define LTC3883_CFGALL_CLK_STR  		0x0002
#define LTC3883_CFGALL_PEC				0x0004
#define LTC3883_CFGALL_MASK_PLL_ULOCK	0x0008
#define LTC3883_CFGALL_IGNORE_RCFG		0x0040
#define LTC3883_CFGALL_FLAG				0x0080

#define LTC3883_SMFR_EXT_GPIO   		0x0001
#define LTC3883_SMFR_VDD_UV_OV  		0x0004
#define LTC3883_SMFR_FAULT_LOG  		0x0008
#define LTC3883_SMFR_PLL_UNLOCK 		0x0010
#define LTC3883_SMFR_NVM_CRC    		0x0020
#define LTC3883_SMFR_TEMP_WARN  		0x0040
#define LTC3883_SMFR_TEMP_FAULT 		0x0080

#define LTC2974_SW_HIGH_BYTE			0x0001
#define LTC2974_SW_CML					0x0002
#define LTC2974_SW_TEMP					0x0004
#define LTC2974_SW_VIN_UV				0x0008
#define LTC2974_SW_IOUT_OC				0x0010
#define LTC2974_SW_VOUT_OV				0x0020
#define LTC2974_SW_OFF					0x0040
#define LTC2974_SW_BUSY					0x0080
#define LTC2974_SW_FANS					0x0400
#define LTC2974_SW_PGOODB				0x0800
#define LTC2974_SW_MFR					0x1000
#define LTC2974_SW_INPUT				0x2000
#define LTC2974_SW_IOUT					0x4000
#define LTC2974_SW_VOUT					0x8000

#define LTC2974_CFGALL_AUX_FB_WPU		0x0001
#define LTC2974_CFGALL_PMBUS_TIMO		0x0002
#define LTC2974_CFGALL_PEC				0x0004
#define LTC2974_CFGALL_VIN_SHARE		0x0008
#define LTC2974_CFGALL_CRTL0_POL		0x0010
#define LTC2974_CFGALL_CTRL1_POL		0x0020
#define LTC2974_CFGALL_VIN_ON_CLR_FLT	0x0040
#define LTC2974_CFGALL_FLAG				0x0080
#define LTC2974_CFGALL_CNTL2_POL		0x0100
#define LTC2974_CFGALL_CNTL3_POL		0x0200
#define LTC2974_CFGALL_FAST_FLAG		0x0400
#define LTC2974_CFGALL_PGOOD_OFF_UV		0x0800

#define LTC2974_CFG_DAC_POL  			0x0001
#define LTC2974_CFG_DAC_GAIN			0x0002
#define LTC2974_CFG_VO_EN_WPD			0x0004
#define LTC2974_CFG_VO_EN_WPU			0x0008
#define LTC2974_CFG_SRVO_ON_WARN		0x0040
#define LTC2974_CFG_SVRO_CONT			0x0080
#define LTC2974_CFG_SUP_RES				0x0400
#define LTC2974_CFG_FAST_SERVO			0x0800
#define LTC2974_CFG_CASCADE_ON			0x4000

#define LTC2974_SMFR_WATCH_DOG  		0x0001
#define LTC2974_SMFR_AUX_FAULT  		0x0002
#define LTC2974_CFG_ALL_DAC_MODE		0x0030
#define LTC2974_SMFR_DAC_SAT    		0x0004
#define LTC2974_SMFR_DAC_CON    		0x0008
#define LTC2974_SMFR_SERVO_TGT  		0x0010
#define LTC2974_SMFR_FAULT0_IN  		0x0020
#define LTC2974_SMFR_FAULT1_IN  		0x0040
#define LTC2974_SMFR_DISCHARGE  		0x0080

#define LTC2974_SFL_EEPROM      		0x0001
#define LTC2974_SFL_RAM         		0x0002

#define LTC2978_SW_HIGH_BYTE			0x0001
#define LTC2978_SW_CML					0x0002
#define LTC2978_SW_TEMP					0x0004
#define LTC2978_SW_VIN_UV				0x0008
#define LTC2978_SW_IOUT_OC				0x0010
#define LTC2978_SW_VOUT_OV				0x0020
#define LTC2978_SW_OFF					0x0040
#define LTC2978_SW_BUSY					0x0080
#define LTC2978_SW_FANS					0x0400
#define LTC2978_SW_PGOODB				0x0800
#define LTC2978_SW_MFR					0x1000
#define LTC2978_SW_INPUT				0x2000
#define LTC2978_SW_IOUT					0x4000
#define LTC2978_SW_VOUT					0x8000

#define LTC2978_CFGALL_AUX_FB_WPU		0x0001
#define LTC2978_CFGALL_PMBUS_TIMO		0x0002
#define LTC2978_CFGALL_PEC				0x0004
#define LTC2978_CFGALL_VIN_SHARE		0x0008
#define LTC2978_CFGALL_CRTL0_POL		0x0010
#define LTC2978_CFGALL_CTRL1_POL		0x0020
#define LTC2978_CFGALL_VIN_ON_CLR_FLT	0x0040
#define LTC2978_CFGALL_FLAG				0x0080
#define LTC2978_CFGALL_FAST_FLAG		0x0400
#define LTC2978_CFGALL_PGOOD_OFF_UV		0x0800
#define LTC2978_CFGALL_SHORT_CYC_FAULT	0x1000

#define LTC2978_CFG_DAC_POL  			0x0001
#define LTC2978_CFG_DAC_GAIN			0x0002
#define LTC2978_CFG_VO_EN_WPD			0x0004
#define LTC2978_CFG_VO_EN_WPU			0x0008
#define LTC2978_CFG_DAC_MODE			0x0030
#define LTC2978_CFG_SRVO_ON_WARN		0x0040
#define LTC2978_CFG_SVRO_CONT			0x0080
#define LTC2978_CFG_CNTL_IN_SEL			0x0100
#define LTC2978_CFG_ADC_RES				0x0200
#define LTC2978_CFG_SUP_RES				0x0400
#define LTC2978_CFG_FAST_SERVO			0x0800
#define LTC2978_CFG_CHAN_MODE			0xC000

#define LTC2978_SMFR_WATCH_DOG  		0x0001
#define LTC2978_SMFR_VIN_EN     		0x0002
#define LTC2978_SMFR_DAC_SAT    		0x0004
#define LTC2978_SMFR_DAC_CON    		0x0008
#define LTC2978_SMFR_SERVO_TGT  		0x0010
#define LTC2978_SMFR_FAULT0_IN  		0x0020
#define LTC2978_SMFR_FAULT1_IN  		0x0040
#define LTC2978_SMFR_DISCHARGE  		0x0080

#define LTC2978_SFL_EEPROM      		0x0001
#define LTC2978_SFL_RAM         		0x0002

#define LTC2977_SFL_EEPROM      		0x0001
#define LTC2977_SFL_RAM         		0x0002

//! PMBus communication. Do not use polled commands with LTC2978 or LTC2977.
//! Commands that end in WithPage use PAGE_PLUS. This is reserved for future
//! products.
class LT_PMBus
{
private:

	/*
		consider changing this smbus_ variable to public.  At the moment, only commands in this pmbus library 
		can be used in group protocol.  If this variable is made public, users can use it to add arbitrary commands 
		to the set they're sending in the group protocol.
	*/
	LT_SMBusGroup smbus_;
	
	uint8_t waitForNotBusy(uint8_t address);
	void pmbusWriteByteWithPolling(uint8_t address, uint8_t command, uint8_t data);
	uint8_t pmbusReadByteWithPolling(uint8_t address, uint8_t command);
	uint16_t pmbusReadWordWithPolling(uint8_t address, uint8_t command);
	
	float L11_to_Float(uint16_t input_val);
	float L16_to_Float_with_polling(uint8_t address, uint16_t input_val);
	float L16_to_Float(uint8_t address, uint16_t input_val);
	float L16_to_FloatExp(int8_t exponent, uint16_t input_val);
	uint16_t Float_to_L16(uint8_t address,  float input_val);
	uint16_t Float_to_L16Exp(int8_t exponent, float input_val);
	uint16_t Float_to_L11(float input_val);
	
public:

	//! Construct a LT_PMBus.
	LT_PMBus(LT_SMBus *pmbus	//!< SMBus for communication. Use the PEC or non-PEC version.
			);
			
	//! Set output voltage
	//! @return void
	void setVout(uint8_t address, 	//!< Slave address 
				float voltage
				);

	//! Set output voltage
	//! @return void
	void setVoutWithPage(uint8_t address, 	//!< Slave address
						float voltage,  	//!< Voltage 
						uint8_t page 		//!< PAGE
						);
	
	//! Set output voltage
	//! @return void
	void setVoutWithPagePlus(uint8_t address, 	//!< Slave address
							float voltage,  	//!< Voltage 
							uint8_t page 		//!< PAGE PLUS PAGE
							);
										
	//! Set output voltage and supervisors
	//! @return void
	void setVoutWithSupervision(uint8_t address, 	//!< Slave address 
								float voltage,  	//!< Voltage 
								float margin_percent,  	//!< Amount to margin
								float warn_percent,  	//!< Amount of warning limit
								float fault_percent 	//!< Amount of fault limit
								);

	//! Set output voltage and supervisors
	//! @return void
	void setVoutWithSupervisionWithPage(uint8_t address, 	//!< Slave address 
										float voltage,  	//!< Voltage
										float margin_percent,  	//!< Amount to margin
										float warn_percent,  	//!< Amount of warning limit
										float fault_percent,  	//!< Amount of fault limit
										uint8_t page			//!< PAGE
										);

	//! Set output voltage and supervisors
	//! @return void
	void setVoutWithSupervisionWithPagePlus(uint8_t address, 	//!< Slave address 
											float voltage,  	//!< Voltage
											float margin_percent,  	//!< Amount to margin
											float warn_percent,  	//!< Amount of warning limit
											float fault_percent,  	//!< Amount of fault limit
											uint8_t page			//!< PAGE PLUS PAGE
											);
																		
	//! Set the maximum output voltage
	//! @return void
	void setVoutMaxWithPage(uint8_t address, 	//!< Slave address 
							float voltage,  	//!< Voltage 
							uint8_t page 		//!< PAGE
							);

	//! Set the maximum output voltage
	//! @return void
	void setVoutMaxWithPagePlus(uint8_t address, 	//!< Slave address 
								float voltage,  	//!< Voltage 
								uint8_t page 		//!< PAGE PLUS PAGE
								);
							
	//! Set the maximum output voltage
	//! @return void
	void setVoutMax(uint8_t address, 	//!< Slave address 
					float voltage 		//!< Voltage 
					);
					
	//! Set delay before rail turns on
	//! @return void
	void setTonDelay(uint8_t address, 	//!< Slave address 
					float delay		 	//!< Delay time
					);
					
	//! Set the turn on rise time
	//! @return void
	void setTonRise(uint8_t address, 	//!< Slave address 
					float rise		 	//!< Rise time
					);
					
	//! Set the maximum time allow for turn on
	//! @return void
	void setTonMaxFaultLimit(uint8_t address, 	//!< Slave address 
							float max			//!< Max time
							);
							
	//! Set the delay before the rail turns off
	//! @return void
	void setToffDelay(uint8_t address, 	//!< Slave address 
						float delay		//!< Max time
						);
						
	//! Set the turn off fall time
	//! @return void
	void setToffFall(uint8_t address, 	//!< Slave address 
						float fall		//!< Fall time
						);
						
	//! Set the maximum time allow to turn off
	//! @return void
	void setToffMaxWarnLimit(uint8_t address, 	//!< Slave address 
							float max			//!< Max time
							);

	//! Set the minimum output voltage
	//! @return void
	void setVoutMin(uint8_t address, 	//!< Slave address 
					float voltage		//!< Voltage
					);
							
	//! Set the minimum output voltage
	//! @return void
	void setVoutMinWithPage(uint8_t address, 	//!< Slave address 
							float voltage,	 	//!< Voltage 
							uint8_t page 		//!< PAGE
							);

	//! Set the minimum output voltage
	//! @return void
	void setVoutMinWithPagePlus(uint8_t address, 	//!< Slave address 
								float voltage,	 	//!< Voltage 
								uint8_t page);		//!< PAGE PLUS PAGE
												
	//! Set the over voltage supervisor fault limit
	//! @return void
	void setVoutOvFaultLimit(uint8_t address, 	//!< Slave address 
							float voltage	 	//!< Voltage 
							);

	//! Set the over voltage supervisor fault limit
	//! @return void
	void setVoutOvFaultLimitWithPage(uint8_t address, 	//!< Slave address 
									float voltage,  	//!< Voltage 
									uint8_t page		//!< PAGE
									);

	//! Set the over voltage supervisor fault limit
	//! @return void
	void setVoutOvFaultLimitWithPagePlus(uint8_t address, 	//!< Slave address 
										float voltage,  	//!< Voltage 
										uint8_t page		//!< PAGE PLUS PAGE
										);
																
		//! Set the over voltage supervisor warning limit
	//! @return void
	void setVoutOvWarnLimit(uint8_t address, 	//!< Slave address 
							float voltage	 	//!< Voltage 
							);

	//! Set the over voltage supervisor warning limit
	//! @return void
	void setVoutOvWarnLimitWithPage(uint8_t address, 	//!< Slave address 
									float voltage,  	//!< Voltage 
									uint8_t page		//!< PAGE
									);

	//! Set the over voltage supervisor warning limit
	//! @return void
	void setVoutOvWarnLimitWithPagePlus(uint8_t address, 	//!< Slave address 
										float voltage,  	//!< Voltage 
										uint8_t page		//!< PAGE PLUS PAGE
										);
							
	//! Set the under voltage supervisor warning limit
	//! @return void
	void setVoutUvWarnLimit(uint8_t address, 	//!< Slave address 
							float voltage	 	//!< Voltage 
							);
							
	//! Set the under voltage supervisor warning limit
	//! @return void
	void setVoutUvWarnLimitWithPage(uint8_t address, 	//!< Slave address 
									float voltage,  	//!< Voltage 
									uint8_t page		//!< PAGE
									);

	//! Set the under voltage supervisor warning limit
	//! @return void
	void setVoutUvWarnLimitWithPagePlus(uint8_t address, 	//!< Slave address 
										float voltage,  	//!< Voltage 
										uint8_t page		//!< PAGE PLUS PAGE
										);
										
	//! Set the margin high
	//! @return void
	void setVoutMarginHigh(uint8_t address, 	//!< Slave address 
							float voltage	 	//!< Voltage 
							);

	//! Set the margin high
	//! @return void
	void setVoutMarginHighWithPage(uint8_t address, 	//!< Slave address 
									float voltage,  	//!< Voltage 
									uint8_t page 		//!< PAGE
									);
									
	//! Set the margin high
	//! @return void
	void setVoutMarginHighWithPagePlus(uint8_t address, 	//!< Slave address 
										float voltage,  	//!< Voltage 
										uint8_t page 		//!< PAGE PLUS PAGE
										);

	//! Set the margin low
	//! @return void
	void setVoutMarginLow(uint8_t address, 	//!< Slave address 
							float voltage 	//!< Voltage 
							);
														
	//! Set the margin low
	//! @return void
	void setVoutMarginLowWithPage(uint8_t address, 		//!< Slave address 
									float voltage,  	//!< Voltage 
									uint8_t page 		//!< PAGE
									);

	//! Set the margin low
	//! @return void
	void setVoutMarginLowWithPagePlus(uint8_t address, 		//!< Slave address 
										float voltage,  	//!< Voltage 
										uint8_t page 		//!< PAGE PLUS PAGE
										);
									
	//! Set the input over voltage warning limit
	//! @return void
	void setVinOvWarningLimit(uint8_t address, 	//!< Slave address 
								float voltage 	//!< Voltage 
								);
																	
	//! Set the under voltage fault limit
	//! @return void
	void setVoutUvFaultLimit(uint8_t address, 	//!< Slave address 
							float voltage	 	//!< Voltage 
							);

	//! Set the under voltage fault limit
	//! @return void
	void setVoutUvFaultLimitWithPage(uint8_t address, 	//!< Slave address 
									float voltage,  	//!< Voltage 
									uint8_t page 		//!< PAGE
									);

	//! Set the under voltage fault limit
	//! @return void
	void setVoutUvFaultLimitWithPagePlus(uint8_t address, 	//!< Slave address 
										float voltage,  	//!< Voltage 
										uint8_t page 		//!< PAGE PLUS PAGE
										);
																
	//! Set the over current fault limit
	//! @return void
	void setIoutOcFaultLimit(uint8_t address, 	//!< Slave address 
								float current	//!< Current
								);
									
	//! Set the over current fault limit
	//! @return void
	void setIoutOcFaultLimitWithPage(uint8_t address, 	//!< Slave address 
									float current, 
									uint8_t page 		//!< PAGE
									);
									
	//! Set the over current fault limit
	//! @return void
	void setIoutOcFaultLimitWithPagePlus(uint8_t address, 	//!< Slave address 
										float current, 
										uint8_t page 		//!< PAGE PLUS PAGE
										);
									
	//! Set the over current warning limit
	//! @return void
	void setIoutOcWarnLimit(uint8_t address, 	//!< Slave address 
								float current	//!< Current
								);

	//! Set the over current warning limit
	//! @return void
	void setIoutOcWarnLimitWithPage(uint8_t address, 	//!< Slave address 
									float current, 		//!< Current
									uint8_t page		//!< PAGE
									);

	//! Set the over current warning limit
	//! @return void
	void setIoutOcWarnLimitWithPagePlus(uint8_t address, 	//!< Slave address 
										float current, 		//!< Current
										uint8_t page		//!< PAGE PLUS PAGE
										);

	//! Set the input over voltage fault limit
	//! @return void
	void setVinOvFaultLimit(uint8_t address, 	//!< Slave address 
							float voltage	 	//!< Voltage 
							);
							
	//! Set the input over voltage warnign limit
	//! @return void
	void setVinOvWarnLimit(uint8_t address, 	//!< Slave address 
							float voltage	 	//!< Voltage 
							);
							
	//! Set the input under voltage fault limit
	//! @return void
	void setVinUvFaultLimit(uint8_t address, 	//!< Slave address 
							float voltage	 	//!< Voltage 
							);
							
	//! Set the input under voltage warning limit
	//! @return void
	void setVinUvWarnLimit(uint8_t address, 	//!< Slave address 
							float voltage	 	//!< Voltage 
							);
							
	//! Set the input over current warning limit
	//! @return void
	void setIinOcWarnLimit(uint8_t address, 	//!< Slave address 
							float current		//!< DCurrent
							);

	//! Set the over temperature fault limit
	//! @return void
	void setOtFaultLimit(uint8_t address, 	//!< Slave address 
						float temperature 	//!< Temperature
						);

	//! Set the over temperature fault limit
	//! @return void
	void setOtFaultLimitWithPage(uint8_t address, 	//!< Slave address 
								float temperature, 	//!< Temperature
								uint8_t page 		//!< PAGE
								);
								
	//! Set the over temperature fault limit
	//! @return void
	void setOtFaultLimitWithPagePlus(uint8_t address, 	//!< Slave address 
									float temperature, 	//!< Temperature
									uint8_t page 		//!< PAGE PLUS PAGE
									);
						
	//! Set the over temperature warning limit
	//! @return void
	void setOtWarnLimit(uint8_t address, 	//!< Slave address 
						float temperature 	//!< Temperature
						);

	//! Set the over temperature warning limit
	//! @return void
	void setOtWarnLimitWithPage(uint8_t address, 	//!< Slave address 
								float temperature,  //!< Temperature
								uint8_t page 		//!< PAGE
								);
								
	//! Set the over temperature warning limit
	//! @return void
	void setOtWarnLimitWithPagePlus(uint8_t address, 	//!< Slave address 
									float temperature,  //!< Temperature
									uint8_t page 		//!< PAGE PLUS PAGE
									);
														
	//! Set the under temperature fault limit
	//! @return void
	void setUtFaultLimit(uint8_t address, 	//!< Slave address 
						float temperature 	//!< Temperature
						);

	//! Set the under temperature fault limit
	//! @return void
	void setUtFaultLimitWithPage(uint8_t address, 	//!< Slave address 
								float temperature,  //!< Temperature
								uint8_t page 		//!< PAGE
								);
								
	//! Set the under temperature fault limit
	//! @return void
	void setUtFaultLimitWithPagePlus(uint8_t address, 	//!< Slave address 
									float temperature,  //!< Temperature
									uint8_t page 		//!< PAGE PLUS PAGE
									);
						
	//! Set the under temperature warning limit
	//! @return void
	void setUtWarnLimit(uint8_t address, 	//!< Slave address 
						float temperature 	//!< Temperature
						);

	//! Set the under temperature warning limit
	//! @return void
	void setUtWarnLimitWithPage(uint8_t address, 	//!< Slave address 
								float temperature,  //!< Temperature
								uint8_t page		//!< PAGE
								);
								
	//! Set the under temperature warning limit
	//! @return void
	void setUtWarnLimitWithPagePlus(uint8_t address, 	//!< Slave address 
									float temperature,  //!< Temperature
									uint8_t page		//!< PAGE PLUS PAGE
									);

	//! Get the over temperature warning limit
	//! @return limit
	float getOtWarnLimit(uint8_t address 	//!< Slave address
							);

	//! Get the over temperature warning limit
	//! @return limit
	float getOtWarnLimitWithPage(uint8_t address, 	//!< Slave address 
									uint8_t page	//!< PAGE
									);
									
	//! Get the over temperature warning limit
	//! @return limit
	float getOtWarnLimitWithPagePlus(uint8_t address, 	//!< Slave address 
									uint8_t page		//!< PAGE PLUS PAGE
									);
							
	//! Set the temperature 1 gain
	//! @return void
	void setTemp1Gain(uint8_t address, 		//!< Slave address 
						uint16_t temperature 	//!< Temperature
						);

	//! Get the temperature 1 gain
	//! @return limit
	uint16_t getTemp1Gain(uint8_t address 		//!< Slave address
							);
						
	//! Get the input voltage
	//! @return voltage
	float readVin(uint8_t address, 	//!< Slave address 
					bool polling 	//!< true for polling
					);
					
	//! Get the output over voltage limit
	//! @return limit
	float readVoutOv(uint8_t address, 	//!< Slave address 
						bool polling	//!< true for polling
						);

	//! Get the output over voltage limit
	//! @return limit
	float readVoutOvWithPage(uint8_t address, 	//!< Slave address 
							uint8_t page		//!< PAGE
							);
							
	//! Get the output over voltage limit
	//! @return limit
	float readVoutOvWithPagePlus(uint8_t address, 	//!< Slave address 
								uint8_t page		//!< PAGE PLUS PAGE
								);
													
	//! Get the measured output voltage
	//! @return voltage
	float readVout(uint8_t address, 	//!< Slave address 
						bool polling	//!< true for polling
						);

	//! Get the measured output voltage
	//! @return voltage
	float readVoutWithPage(uint8_t address, 	//!< Slave address 
							uint8_t page		//!< PAGE
							);
							
	//! Get the measured output voltage
	//! @return voltage
	float readVoutWithPagePlus(uint8_t address, 	//!< Slave address 
								uint8_t page		//!< PAGE PLUS PAGE
								);
						
	//! Get the under voltage limit
	//! @return limit
	float readVoutUv(uint8_t address, 		//!< Slave address 
							bool polling	//!< true for polling
							);

	//! Get the under voltage limit
	//! @return limit
	float readVoutUvWithPage(uint8_t address, 	//!< Slave address 
							uint8_t page		//!< PAGE
							);
							
	//! Get the under voltage limit
	//! @return limit
	float readVoutUvWithPagePlus(uint8_t address, 	//!< Slave address 
								uint8_t page		//!< PAGE PLUS PAGE
								);
							
	//! Get the input current
	//! @return current
	float readIin(uint8_t address, 		//!< Slave address 
						bool polling	//!< true for polling
						);
						
	//! Get the over current limit
	//! @return limit
	float readIoutOc(uint8_t address, 	//!< Slave address 
					bool polling		//!< true for polling
					);

	//! Get the over current limit
	//! @return limit
	float readIoutOcWithPage(uint8_t address, 	//!< Slave address 
							uint8_t page 		//!< PAGE
							);
							
	//! Get the over current limit
	//! @return limit
	float readIoutOcWithPagePlus(uint8_t address, 	//!< Slave address 
								uint8_t page 		//!< PAGE PLUS PAGE
								);
					
	//! Get the measured output current
	//! @return current
	float readIout(uint8_t address, 	//!< Slave address 
					bool polling		//!< true for polling
					);

	//! Get the measured output current
	//! @return current
	float readIoutWithPage(uint8_t address, 	//!< Slave address 
							uint8_t page		//!< PAGE
							);
							
	//! Get the measured output current
	//! @return current
	float readIoutWithPagePlus(uint8_t address, 	//!< Slave address 
								uint8_t page		//!< PAGE PLUS PAGE
							)	;
					
	//! Get the measured input power
	//! @return current
	float readPin(uint8_t address, 	//!< Slave address 
					bool polling		//!< true for polling
					);
	
	//! Get the measured output power
	//! @return current
	float readPout(uint8_t address, 	//!< Slave address 
					bool polling		//!< true for polling
					);

	//! Get the measured output power
	//! @return current
	float readPoutWithPage(uint8_t address, 	//!< Slave address 
							uint8_t page 		//!< PAGE
							);
				
	//! Get the measured output power
	//! @return current
	float readPoutWithPagePlus(uint8_t address, 	//!< Slave address 
								uint8_t page 		//!< PAGE PLUS PAGE
								);

	//! Get the internal measured temperature
	//! @return temperature
	float readItemp(uint8_t address, 	//!< Slave address 
					bool polling		//!< true for polling
					);

	//! Get the internal measured temperature
	//! @return temperature
	float readItempWithPage(uint8_t address, 	//!< Slave address 
							uint8_t page		//!< The PAGE
							);

	//! Get the internal measured temperature
	//! @return temperature
	float readItempWithPagePlus(uint8_t address, 	//!< Slave address 
								uint8_t page		//!< The PAGE PLUS PAGE
								);
												
	//! Get the external measured temperature
	//! @return temperature
	float readOtemp(uint8_t address, 	//!< Slave address 
					bool polling		//!< true for polling
					);

	//! Get the external measured temperature
	//! @return temperature
	float readOtempWithPage(uint8_t address, 	//!< Slave address 
							uint8_t page		//!< PAGE
							);
							
	//! Get the external measured temperature
	//! @return temperature
	float readOtempWithPagePlus(uint8_t address, 	//!< Slave address 
								uint8_t page		//!< PAGE PLUS PAGE
								);
					
	//! Get the output voltage status mask
	//! @return mask
	void setVoutStatusMask(uint8_t address, 	//!< Slave address 
						uint8_t mask			//!< The status mask
						);

	//! Get the status output status mask
	//! @return mask
	void setSoutStatusMask(uint8_t address, 	//!< Slave address 
							uint8_t mask		//!< The status mask
							);
							
	//! Get the output current status mask
	//! @return mask
	void setIoutStatusMask(uint8_t address, 	//!< Slave address 
							uint8_t mask		//!< The status mask
							);
							
	//! Get the input status mask
	//! @return mask
	void setInputStatusMask(uint8_t address, 	//!< Slave address 
							uint8_t mask		//!< The status mask
							);
							
	//! Get the temperature status mask
	//! @return mask
	void setTemperatureStatusMask(uint8_t address, 	//!< Slave address 
								uint8_t mask		//!< The status mask
								);
								
	//! Get the CML status mask
	//! @return mask
	void setCmlStatusMask(uint8_t address, 	//!< Slave address 
						uint8_t mask		//!< The status mask
						);
						
	//! Get the MFR status mask
	//! @return mask
	void setMfrStatusMask(uint8_t address, 	//!< Slave address 
							uint8_t mask		//!< The status mask
							);

	//! Get the output voltage status mask
	//! @return mask
	uint8_t readVoutStatusMask(uint8_t address		//!< Slave address
							);
							
	//! Get the output current status mask
	//! @return mask
	uint8_t readIoutStatusMask(uint8_t address		//!< Slave address
							);
							
	//! Get the input status mask
	//! @return mask
	uint8_t readInputStatusMask(uint8_t address		//!< Slave address
							);
							
	//! Get the temperature status mask
	//! @return mask
	uint8_t readTemperatureStatusMask(uint8_t address		//!< Slave address
							);
							
	//! Get the CML status mask
	//! @return mask
	uint8_t readCmlStatusMask(uint8_t address		//!< Slave address
							);
							
	//! Get the MFR status mask
	//! @return mask
	uint8_t readMfrStatusMask(uint8_t address		//!< Slave address
							);
							

	//! Get the output voltage status byte
	//! @return byte
	uint8_t readVoutStatusByte(uint8_t address		//!< Slave address
							);
							
	//! Get the output current status byte
	//! @return byte
	uint8_t readIoutStatusByte(uint8_t address		//!< Slave address
							);
							
	//! Get the input status byte
	//! @return byte
	uint8_t readInputStatusByte(uint8_t address		//!< Slave address
							);
							
	//! Get the temperature status byte
	//! @return byte
	uint8_t readTemperatureStatusByte(uint8_t address		//!< Slave address
							);
							
	//! Get the CML status byte
	//! @return byte
	uint8_t readCmlStatusByte(uint8_t address		//!< Slave address
							);
							
	//! Get the MFR status byte
	//! @return byte
	uint8_t readMfrStatusByte(uint8_t address		//!< Slave address
							);
							
	//! Get the fault log status byte
	//! @return byte
	uint8_t readMfrFaultLogStatusByte(uint8_t address		//!< Slave address
							);
							
	//! Get the state of the pads
	//! @return state
	uint16_t readMfrPads(uint8_t address		//!< Slave address
							);
							

	//! Get the pmbus  revision
	//! @return revision
	uint8_t readPmbusRevision(uint8_t address		//!< Slave address
							);
	
	//! Get the slave revision
	//! @return model
	void readRevision(uint8_t address, 		//!< Slave address 
						uint8_t *revision	//!< Place to store the revision
						);						
	//! Get the model
	//! @return model
	void readModel(uint8_t address, 	//!< Slave address 
						uint8_t *model	//!< Place to store the model string
						);

	//! Get the status byte
	//! @return byte
	uint8_t readStatusByte(uint8_t address		//!< Slave address
							);
							
	//! Get the status byte
	//! @return byte
	uint8_t readStatusByteWithPage(uint8_t address, 	//!< Slave address 
								uint8_t page			//!< PAGE
								);
								
	//! Get the status byte
	//! @return byte
	uint8_t readStatusByteWithPagePlus(uint8_t address, 	//!< Slave address 
									uint8_t page			//!< PAGE PLUS PAGE
									);

	//! Get the status word
	//! @return byte
	uint16_t readStatusWord(uint8_t address		//!< Slave address
							);
							
	//! Get the status word
	//! @return byte
	uint16_t readStatusWordWithPage(uint8_t address, 	//!< Slave address 
									uint8_t page		//!< PAGE
									);
							
	//! Get the status word
	//! @return byte
	uint16_t readStatusWordWithPagePlus(uint8_t address, 	//!< Slave address 
										uint8_t page		//!< PAGE PLUS PAGE
										);

	//! Clear the faults
	//! @return void
	void clearFaults(uint8_t address		//!< Slave address
							);
							
	//! Clear all the faults
	//! @return void
	void clearFaultsGlobal(void);
	
	//! Set the first watchdog timer
	//! @return void
	void setMfrWatchdogFirst(uint8_t address,	//!< Slave address
							float delay			//!< First delay
							);
	
	//! Set watchdog timer
	//! @return void
	void setMfrWatchdog(uint8_t address,	//!< Slave address
						float delay			//!< Normal delay
						);

	//! Restore device from NVM
	//! @return void
	void restoreFromNvm(uint8_t address		//!< Slave address
							);
							
	//! Restore list of devices from NVM
	//! @return void
	void restoreFromNvmAll(uint8_t* addresses, 		//!< Slave addresses
							uint8_t no_addresses	//!< Number of slave addresses
							);
							
	//! Restore all devices from NVM
	//! @return void
	void restoreFromNvmGlobal(void);
	
	//! Store RAM to NVM
	//! @return void
	void storeToNvm(uint8_t address		//!< Slave address
						);
						
	//! Store RAM to NVM for list of devices
	//! @return void
	void storeToNvmAll(uint8_t* addresses, 		//!< Slave addresses
						uint8_t no_addresses	//!< Number of slave addresses
						);
						
	//! Store RAM to NVM for all devices
	//! @return void
	void storeToNvmGlobal(void);

	//! Set the page
	//! @return void
	void page(uint8_t* addresses, 			//!< Slave addresses
					uint8_t no_addresses	//!< Number of slave addresses
					);

	//! Sequence on a list of devices
	//! @return void
	void sequenceOn(uint8_t* addresses, 		//!< Slave addresses
					uint8_t no_addresses		//!< Number of slave addresses
					);
					
	//! Turn off all devices immediately
	//! @return void
	void immediateOff(uint8_t* addresses, 		//!< Slave addresses
					uint8_t no_addresses		//!< Number of slave addresses
					);
					
	//! Sequence off a list of addresses
	//! @return void
	void sequenceOff(uint8_t* addresses, 		//!< Slave addresses
					uint8_t no_addresses		//!< Number of slave addresses
					);
					
	//! Sequence off all rails
	//! @return void
	void sequenceOffGlobal(void);
	
	//! CSequence on all rails
	//! @return void
	void sequenceOnGlobal(void);
	
	//! Turn off all rails immediately
	//! @return void
	void immediateOffGlobal(void);

	//! Issue reset to all devices
	//! @return void
	void resetGlobal(void);
	
	//! Issue reset to one device
	//! @return void
	void reset(uint8_t address);

	//! Margin a list of rails high
	//! @return void
	void marginHighAll(uint8_t* addresses, 		//!< Slave addresses
						uint8_t* pages, 		//!< The page used for setPage
						uint8_t no_addresses	//!< Number of slave addresses
						);
						
	//! CMargin a list of rails low
	//! @return void
	void marginLowAll(uint8_t* addresses, 		//!< Slave addresses
						uint8_t* pages,  		//!< The page used for setPage
						uint8_t no_addresses	//!< Number of slave addresses
						);
						
	//! Margin all rails high
	//! @return void
	void marginHighGlobal(void);
	
	//! CMargin all rails low
	//! @return void
	void marginLowGlobal(void);

	//! Set the page and poll the busy bit
	//! @return void
	void setPageWithPolling(uint8_t address, 	//!< Slave address 
						uint8_t page 			//!< PAGE PLUS PAGE
						);
						
	//! Set the page
	//! @return void
	void setPage(uint8_t address, 		//!< Slave address 
					uint8_t page 		//!< PAGE PLUS PAGE
					);
					
	//! Enable pec for all transactions
	//! @return void
	void enablePec(uint8_t address 		//!< Slave address 
					);
	
	//! Disable pec for all transactions
	//! @return void
	void disablePec(uint8_t address 		//!< Slave address 
					);
					
	//! starts group protocol
	void startGroupProtocol(void);
	
	//! ends group protocol
	void executeGroupProtocol(void);
};

#endif /* PMBUS_H_ */

