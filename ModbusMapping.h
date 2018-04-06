#ifndef MODBUSMAPPING_H
	#define MODBUSMAPPING_H

	#include "Tools.h"

	#define MB_CNT 8
	
	uint8_t ReadDeviceReg(uint16_t address,uint16_t* value);
	uint8_t WriteDeviceReg(uint16_t address,uint16_t* value);
	uint8_t ReadEepromReg(uint16_t address,uint16_t* value);   
	uint8_t WriteEepromReg(uint16_t address,uint16_t* value);  
	uint8_t ReadVersionReg(uint16_t address,uint16_t* value);  
	uint8_t ReadStatusReg(uint16_t address,uint16_t* value);   
	uint8_t ReadSettingsReg(uint16_t address,uint16_t* value); 
	uint8_t ReadUptimeReg(uint16_t address,uint16_t* value); 
   
	const modbusMapping_t mbMapping[MB_CNT] PROGMEM = 
  {		
	  MB_READ_RANGE(0x9C41,0x00C6,ReadDeviceReg),
	  MB_WRITE_RANGE(0x9C41,0x00C6,WriteDeviceReg),
	  MB_READ_RANGE(0x9D07,0x0400,ReadEepromReg),
	  MB_WRITE_RANGE(0x9D07,0x0400,WriteEepromReg),
	  MB_READ_RANGE(0x7531,0x0008,ReadVersionReg),
	  MB_READ(0x7539,ReadStatusReg),
	  MB_READ(0x7540,ReadSettingsReg),
	  MB_READ_RANGE(0x7541,0x0002,ReadUptimeReg)
  };  

   uint8_t HandleModbusCall(uint16_t address,uint16_t* value);

#endif

