#ifndef TOOLS_H                                                                         //tools header check
  #define TOOLS_H                                                                       //and definition
                                                                                        //
  #include <stdint.h>                                                                   //default definition of integers
  #include "Arduino.h"                                                                  //arduino definitions
  #include <EEPROM.h>
  #include "Exceptions.h"                                                               //header containing exception codes

  #define NO_RESET_FLAG 0x00                                                            //device was cleanly started
  #define EXTERNAL_RESET_FLAG 0x02                                                      //device was reset externally (reset input)
  #define BROWNOUT_RESET_FLAG 0x03                                                      //device was reset through brownout
  #define WATCHDOG_RESET_FLAG 0x04                                                      //device was reset through the wachdog
  
  #define SET_REGISTER(addr,value) ((*(volatile uint8_t *)addr)=value)                  //set an actual register address with given value
  #define GET_REGISTER(addr) (*(volatile uint8_t *)addr)                                //return actual register from given address
                                                                                        //
  #define MB_MAP_FULL(start, cnt, funcPtr, isRead) {start,cnt,funcPtr,isRead}           //all the parameters to map a function into memory
  #define MB_READ_RANGE(start,cnt,funcPtr) MB_MAP_FULL(start,cnt,funcPtr,true)          //read multiple value, so read flag is true
  #define MB_WRITE_RANGE(start,cnt,funcPtr) MB_MAP_FULL(start,cnt,funcPtr,false)        //write multiple values, so read flag is false
  #define MB_WRITE(start,funcPtr) MB_WRITE_RANGE(start,1,funcPtr)                       //write a single value, so length is 1
  #define MB_READ(start,funcPtr) MB_READ_RANGE(start,1,funcPtr)                         //read a single value, so length is 1
                                                                                        //
  template <typename T> void PROGRAM_READTYPE (const T * src, T& dst)                   //read from flash and copy to memory
  {                                                                                     //
    memcpy_P (&dst, src, sizeof (T));                                                   //use memcpy_P to read from Program memory (Flash)
  }                                                                                     //
                                                                                        //
  typedef uint8_t (*ModbusFuncPtr)(uint16_t address,uint16_t* value);                   //function definition, used in modbusmapping function array 
                                                                                        //which is put into flash memory
  typedef struct                                                                        //
  {                                                                                     //
    uint16_t regStart;                                                                  //starting address of function call
    uint16_t regCnt;                                                                    //number of registers
    ModbusFuncPtr funcPtr;                                                              //function pointer
    bool isRead;                                                                        //is it a reading function, used when executing the function
  } modbusMapping_t;                                                                    //name of struct type
                                                                                        //
  typedef union{                                                                        //
    uint8_t buf[2];                                                                     //2 bytes make a 16 bit short
    uint16_t val=0;                                                                     //short value
  } union16_t;                                                                          //mapping bytes onto short
                                                                                        //
  typedef union                                                                         //
  {                                                                                     //
    uint8_t buf[4];                                                                     //4 bytes make a int
    uint32_t val;                                                                       //int value
  } union32_t;                                                                          //mapping bytes onto int
                                                                                        //
  bool CheckCrc(uint8_t* buf, uint16_t len);                                            //check given crc by calculating new it on your own
  uint16_t GetCrc16(uint8_t* buf, uint16_t len);                                        //get the crc16 value of the given data
                                                                                        //
  bool ReadEeprom(uint16_t start, uint16_t cnt, uint8_t* buf);                          //read from eeprom into buffer
  bool WriteEeprom(uint16_t start, uint16_t cnt, uint8_t* buf);                         //write to eeprom from buffer
                                                                                        //
  bool ConfigFromEeprom(uint16_t start,uint16_t cnt, uint8_t* buf);                     //load from eeprom based upon config start address
  bool ConfigToEeprom(uint16_t start,uint16_t cnt, uint8_t* buf);                       //write to eeprom based upon config start address
                                                                                        //
  bool CheckEepromForMagic();                                                           //read at a given address a certain magic value, return result
  uint8_t CheckResetRegister();                                                         //read and clear the reset register, returning the result
                                                                                        //
#endif                                                                                  //
