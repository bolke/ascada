#ifndef MODBUSMAPPING_H                                                                 //
  #define MODBUSMAPPING_H                                                               //
                                                                                        //
  #include "Tools.h"                                                                    //
                                                                                        //
  #define MB_CNT 9                                                                      //number of elements in modbusmapping array
                                                                                        //  
  uint8_t ReadDeviceReg(uint16_t address,uint16_t* value);                              //all function definitions use the same format
  uint8_t WriteDeviceReg(uint16_t address,uint16_t* value);                             //and number the same as the amount of elements
  uint8_t ReadEepromReg(uint16_t address,uint16_t* value);                              //in the array. 
  uint8_t WriteEepromReg(uint16_t address,uint16_t* value);                             //
  uint8_t ReadVersionReg(uint16_t address,uint16_t* value);                             //functions are transformed into program memory, 
  uint8_t ReadStatusReg(uint16_t address,uint16_t* value);                              //freeing up some normal memory.
  uint8_t ReadSettingsReg(uint16_t address,uint16_t* value);                            //
  uint8_t ReadUptimeReg(uint16_t address,uint16_t* value);                              //
  uint8_t WriteFuncCoil(uint16_t address,uint16_t* value);                              //
                                                                                        //
  const modbusMapping_t mbMapping[MB_CNT] PROGMEM =                                     //PROGMEM means, store this data into flash memory instead of normal memory.
  {                                                                                     //function with modbus address start and amount of addresses used
    MB_READ_RANGE(0x9C41,0x00C6,ReadDeviceReg),                                         //read from device, 0xC6 (198) registers (device registers), modbus address starts at 0x9C41 (40001)
    MB_WRITE_RANGE(0x9C41,0x00C6,WriteDeviceReg),                                       //write to device, 0xC6 (198) registers (device registers), modbus address starts at 0x9C41 (40001)
    MB_READ_RANGE(0x9D07,0x0400,ReadEepromReg),                                         //read from eeprom, 0x0400 (1024) registers (eeprom registers), modbus address 0x9D07 (40199)
    MB_WRITE_RANGE(0x9D07,0x0400,WriteEepromReg),                                       //write to eeprom, 0x0400 (1024) registers (eeprom registers), modbus address 0x9D07 (40199)
    MB_READ_RANGE(0x7531,0x0008,ReadVersionReg),                                        //read version information (8 registers) from memory, modbus address 0x7531 (30001)
    MB_READ(0x7539,ReadStatusReg),                                                      //read status register (1 register) from memory, modbus address 0x7539 (30009)
    MB_READ(0x753A,ReadSettingsReg),                                                    //read settings register (1 register) from memory, modbus address 0x753A (30010)
    MB_READ_RANGE(0x753B,0x0002,ReadUptimeReg),                                         //read uptime register (2 registers) from memory, modbus address 0x753B (30011)
    MB_WRITE_RANGE(0x0001,0x0050,WriteFuncCoil)                                         //write function register 0x50 (80 register) from memory, modbus address 0x0001 (1)
  };                                                                                    //
                                                                                        //
  uint8_t HandleModbusRead(uint16_t address,uint16_t* value);                           //
  uint8_t HandleModbusWrite(uint16_t address,uint16_t* value);                          //
                                                                                        //
#endif                                                                                  //
