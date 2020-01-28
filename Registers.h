// reading and writing registers from the device into a modbus format (and using modbus addresses)

#ifndef REGISTERS_H
  #define REGISTERS_H

  #include "Project.h"
  #include "Tools.h"

  uint8_t ReadDeviceReg(uint16_t address,uint16_t* value);                              //read a device register (avr register), one address at a time (8 bits)            
  uint8_t WriteDeviceReg(uint16_t address,uint16_t* value);                             //write a device register (avr register), one address at a time (8 bits)
  uint8_t ReadEepromReg(uint16_t address,uint16_t* value);                              //read an eeprom register (avr eeprom register), 16 bits at a time
  uint8_t WriteEepromReg(uint16_t address,uint16_t* value);                             //write an eeprom register (avr eeprom register), 16 bits at a time
  uint8_t ReadAlarmReg(uint16_t address,uint16_t* value);                               //read an alarm register, a 16 bit alarm register (16 bits for 16 alarms)
  uint8_t ReadVersionReg(uint16_t address,uint16_t* value);                             //read the version information (2 bytes per read)
  uint8_t ReadStatusReg(uint16_t address,uint16_t* value);                              //read a status register
  uint8_t ReadSettingsReg(uint16_t address,uint16_t* value);                            //read a settings register
  uint8_t ReadUptimeReg(uint16_t address,uint16_t* value);                              //read uptime register
  uint8_t WriteFuncCoil(uint16_t address,uint16_t* value);                              //write a function coil (bit toggling)

  #define MB_CNT 12                                                                     //number of elements in modbusmapping array

  const modbusMapping_t mbMapping[MB_CNT] PROGMEM =                                     //PROGMEM means, store this data into flash memory instead of normal memory.
  {                                                                                     //function with modbus address start and amount of addresses used
    MB_READ_RANGE(0x9C41,0x00C7,ReadDeviceReg),                                         //read from device, 0xC6 (198) registers (device registers)
    MB_WRITE_RANGE(0x9C41,0x00C7,WriteDeviceReg),                                       //write to device, 0xC6 (198) registers (device registers)
    MB_READ_RANGE(0x9D08,0x0200,ReadEepromReg),                                         //read from eeprom, 0x0200 (512) registers (eeprom registers)
    MB_WRITE_RANGE(0x9D08,0x0200,WriteEepromReg),                                       //write to eeprom, 0x0200 (512) registers (eeprom registers)
    MB_READ_RANGE(0x7531,0x0008,ReadVersionReg),                                        //read version information (8 registers) from memory
    MB_READ(0x7539,ReadStatusReg),                                                      //read status register (1 register) from memory
    MB_READ(0x753A,ReadSettingsReg),                                                    //read settings register (1 register) from memory
    MB_READ_RANGE(0x753B,0x0002,ReadUptimeReg),                                         //read uptime register (2 registers) from memory
    MB_READ_RANGE(0x753D,ALARM_WORD_CNT,ReadAlarmReg),                                  //read an alarm register (however many are defined in ProjectDefines
    MB_WRITE_RANGE(0x0001,0x0003,WriteFuncCoil),                                        //write function register 0x50 (80 register) from memory
    MB_READ_RANGE(0x9F08,0x200,prReadRegister),                                         //read project registers 0x200 (512 register) for project
    MB_WRITE_RANGE(0x9F08,0x200,prWriteRegister)                                        //write project registers 0x200 (512 register) for project
  };
  
#endif
