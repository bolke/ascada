//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Tools.h"                                                                      
#include "Project.h"                                                             
#include <EEPROM.h>                                                                     
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool CheckCrc(uint8_t* buf, uint16_t len)                                               //calc crc16 from given bytearray, exception last 2, and compare with last 2
{                                                                                       //
  uint16_t crc = GetCrc16(buf,len-2);                                                   //calculate crc from array
  uint16_t oldCrc = buf[len-1];                                                         //old crc from last 2 bytes
  oldCrc = (oldCrc << 8) + buf[len-2];                                                  //
  return crc == oldCrc;                                                                 //return old and new compared
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t GetCrc16(uint8_t* buf, uint16_t len)                                           //
{                                                                                       //
  uint16_t crc = 0xFFFF;                                                                //calculate the crc with the given values
  for (uint16_t pos = 0; pos < len; pos++)                                              //loop through all the bytes
  {                                                                                     //
    crc ^= (uint16_t)buf[pos];                                                          //bitwise xor
    for (uint16_t i = 8; i != 0; i--)                                                   //loop from 8 down to 0
    {                                                                                   //
      if ((crc & 0x0001) != 0)                                                          //some magic to generate the crc
      {                                                                                 //
        crc >>= 1;                                                                      //
        crc ^= 0xA001;                                                                  //
      }                                                                                 //
      else                                                                              //
      {                                                                                 //
        crc >>= 1;                                                                      //
      }                                                                                 //
    }                                                                                   //
  }                                                                                     //
  return crc;                                                                           //return crc
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ReadEeprom(uint16_t start, uint16_t cnt, uint8_t* buf)                             //read bytes from eeprom, start at start, cnt amount, buf filled with data
{                                                                                       //
  if((start<=E2END) && (cnt>0 && cnt <=E2END) && ((start+cnt)<=E2END))                  //check eeprom range, 0 until E2END, inclusive
  {                                                                                     //
    for(uint16_t i=0;i<cnt;i++)                                                         //loop through all bytes to read
    {                                                                                   //
      buf[i]=EEPROM.read(start);                                                        //read one byte at a time, filling the buffer
      start++;                                                                          //next address
    }                                                                                   //
    return true;                                                                        //done successfully
  }                                                                                     //
  return false;                                                                         //failure, out of range
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool WriteEeprom(uint16_t start, uint16_t cnt, uint8_t* buf)                            //write bytes into eeprom, start at start, cnt amount, buf filled with data
{                                                                                       //
  if((start<=E2END) && (cnt>0 && cnt <=E2END) && ((start+cnt)<=E2END))                  //check eeprom range, 0 until E2END, inclusive
  {                                                                                     //
    for(uint16_t i=0;i<cnt;i++)                                                         //loop through all bytes to write
    {                                                                                   //
      EEPROM.write(start,buf[i]);                                                       //write a byte, at start, with i to track the byte
      start++;                                                                          //increment start
    }                                                                                   //
    return true;                                                                        //done successfully
  }                                                                                     //
  return false;                                                                         //failure, out of range
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool CheckEepromForMagic()                                                              //read an address, check value, and return success 
{                                                                                       //
  union16_t magic;                                                                      //read into this variable from eeprom
  magic.buf[0]=EEPROM.read(MAGIC_ADDRESS);                                              //read on magic address first byte 
  magic.buf[1]=EEPROM.read(MAGIC_ADDRESS+1);                                            //read on magic address second byte
  return magic.val == MAGIC_NUMBER;                                                     //return compared value 
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t CheckResetRegister()                                                            //read and clear the reset register
{                                                                                       //
  uint8_t value=(GET_REGISTER(0x54)&0x0F);                                              //get the reset register, only first 4 bits are interesting
  SET_REGISTER(0x54,0x00);                                                              //and clear it
  switch(value)                                                                         //
  {                                                                                     //
    case 0x02:                                                                          //external reset flag
      return EXTERNAL_RESET_FLAG;                                                       //
    case 0x04:                                                                          //brownout flag
      return BROWNOUT_RESET_FLAG;                                                       //
    case 0x08:                                                                          //watchdog flag
      return WATCHDOG_RESET_FLAG;                                                       //
  }                                                                                     //
  return NO_RESET_FLAG;                                                                 //no reset has happened, clean startup
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t GetConfigStartAddress()                                                        //get the address for the config from eeprom
{                                                                                       //
  union16_t configStartAddress;                                                         //
  configStartAddress.buf[0]=EEPROM.read(MAGIC_ADDRESS+2);                               //byte 0, read from MAGIC_ADDRESS+2
  configStartAddress.buf[1]=EEPROM.read(MAGIC_ADDRESS+3);                               //byte 1, read from MAGIC_ADDRESS+3
  return configStartAddress.val;                                                        //result
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ConfigFromEeprom(uint16_t start,uint16_t cnt, uint8_t* buf)                        //start address, byte count to read, buffer to fill
{                                                                                       //
  return ReadEeprom(GetConfigStartAddress() + start,cnt,buf);                           //read config from eeprom
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ConfigToEeprom(uint16_t start,uint16_t cnt, uint8_t* buf)                          //start address, byte count to write, buffer to use
{                                                                                       //
  return WriteEeprom(GetConfigStartAddress() + start,cnt,buf);                          //write config to eeprom
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
