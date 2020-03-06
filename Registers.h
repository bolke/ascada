#ifndef REGISTERS_H
  #define REGISTERS_H

  #include "Project.h"
  #include "Tools.h"

  uint8_t ReadDeviceReg(uint16_t address,uint16_t* value);
  uint8_t WriteDeviceReg(uint16_t address,uint16_t* value);
  uint8_t ReadEepromReg(uint16_t address,uint16_t* value);
  uint8_t WriteEepromReg(uint16_t address,uint16_t* value);
  uint8_t ReadAlarmReg(uint16_t address,uint16_t* value);
  uint8_t ReadVersionReg(uint16_t address,uint16_t* value);
  uint8_t ReadStatusReg(uint16_t address,uint16_t* value);
  uint8_t ReadSettingsReg(uint16_t address,uint16_t* value);
  uint8_t ReadUptimeReg(uint16_t address,uint16_t* value);
  uint8_t WriteFuncCoil(uint16_t address,uint16_t* value);

  #define MB_CNT 12

  const modbusMapping_t mbMapping[MB_CNT] PROGMEM =
  {    
    MB_READ_RANGE(0x9C41,0x00C7,ReadDeviceReg),
    MB_WRITE_RANGE(0x9C41,0x00C7,WriteDeviceReg),
    MB_READ_RANGE(0x9D08,0x0200,ReadEepromReg),
    MB_WRITE_RANGE(0x9D08,0x0200,WriteEepromReg),   
    MB_READ_RANGE(0x9F08,0x200,prReadRegister),
    MB_WRITE_RANGE(0x9F08,0x200,prWriteRegister),
     
    MB_READ_RANGE(0x7531,0x0009,ReadVersionReg),
    MB_READ(0x753A,ReadStatusReg),
    MB_READ_RANGE(0x753B,0x0002,ReadSettingsReg),
    MB_READ_RANGE(0x753D,0x0002,ReadUptimeReg),
    MB_READ_RANGE(0x753F,ALARM_WORD_CNT,ReadAlarmReg),    
    
    MB_WRITE_RANGE(0x0001,0x0003,WriteFuncCoil)       
  };

#endif
