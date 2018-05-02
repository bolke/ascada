#ifndef TOOLS_H
  #define TOOLS_H

  #include <stdint.h>
  #include "Arduino.h"
  #include "ProjectDefines.h"

  #define SET_REGISTER(addr,value) ((*(volatile uint8_t *)addr)=value)
  #define GET_REGISTER(addr) (*(volatile uint8_t *)addr)

  #define MB_MAP_FULL(regStart, regCnt, funcPtr, isRead) {regStart,regCnt,funcPtr,isRead}
  #define MB_READ_RANGE(regStart,regCnt,funcPtr) MB_MAP_FULL(regStart,regCnt,funcPtr,true)
  #define MB_WRITE_RANGE(regStart,regCnt,funcPtr) MB_MAP_FULL(regStart,regCnt,funcPtr,false)
  #define MB_WRITE(regStart,funcPtr) MB_WRITE_RANGE(regStart,1,funcPtr)
  #define MB_READ(regStart,funcPtr) MB_READ_RANGE(regStart,1,funcPtr)

  template <typename T> void PROGRAM_READTYPE (const T * sce, T& dest)
  {
    memcpy_P (&dest, sce, sizeof (T));
  }

  typedef uint8_t (*ModbusFuncPtr)(uint16_t address,uint16_t* value);

  typedef struct
  {
    uint16_t regStart;
    uint16_t regCnt;
    ModbusFuncPtr funcPtr;
    bool isRead;
  } modbusMapping_t;

  typedef union{
    uint8_t buf[2];
    uint16_t val=0;
  } union16_t;

  typedef union
  {
    uint8_t buf[4];
    uint32_t val;
  } union32_t;

  bool CheckCrc(uint8_t* buf, uint16_t len);
  uint16_t GetCrc16(uint8_t* buf, uint16_t len);

  bool ReadEeprom(uint16_t start, uint16_t cnt, uint8_t* buf);
  bool WriteEeprom(uint16_t start, uint16_t cnt, uint8_t* buf);

  bool ConfigFromEeprom(uint16_t start,uint16_t cnt, uint8_t* buf);
  bool ConfigToEeprom(uint16_t start,uint16_t cnt, uint8_t* buf);

  bool CheckEepromForMagic();
  uint8_t CheckResetRegister();

#endif

