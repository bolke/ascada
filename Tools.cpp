//--------------------------------------------------------------------------------------
#include "Tools.h"
#include <EEPROM.h>
//--------------------------------------------------------------------------------------
bool CheckCrc(uint8_t* buf, uint16_t len)
{
  uint16_t crc = GetCrc16(buf,len-2);
  uint16_t oldCrc = buf[len-1];
  oldCrc = (oldCrc << 8) + buf[len-2];
  return crc == oldCrc;
}
//--------------------------------------------------------------------------------------
uint16_t GetCrc16(uint8_t* buf, uint16_t len)
{
  uint16_t crc = 0xFFFF;
  for (uint16_t pos = 0; pos < len; pos++)
  {
    crc ^= (uint16_t)buf[pos];
    for (uint16_t i = 8; i != 0; i--)
    {
      if ((crc & 0x0001) != 0)
      {
        crc >>= 1;
        crc ^= 0xA001;
      }
      else
        crc >>= 1;
    }
  }
  return crc;
}
//--------------------------------------------------------------------------------------
bool ReadEeprom(uint16_t start, uint16_t cnt, uint8_t* buf)
{
  if((start<=E2END) && (cnt>0 && cnt <=E2END) && ((start+cnt)<=E2END))
  {
    for(uint16_t i=0;i<cnt;i++)
    {
      buf[i]=EEPROM.read(start);
      start++;
    }
    return true;
  }
  return false;
}
//--------------------------------------------------------------------------------------
bool WriteEeprom(uint16_t start, uint16_t cnt, uint8_t* buf)
{
  if((start<=E2END) && (cnt>0 && cnt <=E2END) && ((start+cnt)<=E2END))
  {
    for(uint16_t i=0;i<cnt;i++)
    {
      EEPROM.write(start,buf[i]);
      start++;
    }
    return true;
  }
  return false;
}
//--------------------------------------------------------------------------------------
bool CheckEepromForMagic()
{
  union16_t magic;
  magic.buf[0]=EEPROM.read(MAGIC_ADDRESS);
  magic.buf[1]=EEPROM.read(MAGIC_ADDRESS+1);
  return magic.val == MAGIC_NUMBER;
}
//--------------------------------------------------------------------------------------
uint8_t CheckResetRegister()
{
  uint8_t result=(GET_REGISTER(0x54)&0x0F);                                             //get the reset register, only first 4 bits are interesting
  SET_REGISTER(0x54,0x00);                                                              //and clear it
  if(result&0x02)                                                                       //check external reset flag
    result=2;
  else if(result&0x04)                                                                  //check brown out reset flag
    result=3;
  else if(result&0x08)                                                                  //check watchdog reset flag
    result=4;
  return result;                                                                        //and if only bit 1 is set, bit 1 stays set
}
//--------------------------------------------------------------------------------------
uint16_t GetConfigStart()
{
  union16_t configStart;
  configStart.buf[0]=EEPROM.read(MAGIC_ADDRESS+2);
  configStart.buf[1]=EEPROM.read(MAGIC_ADDRESS+3);
  return configStart.val;
}
//--------------------------------------------------------------------------------------
bool ConfigFromEeprom(uint16_t start,uint16_t cnt, uint8_t* buf)
{
  return ReadEeprom(GetConfigStart() + start,cnt,buf);
}
//--------------------------------------------------------------------------------------
bool ConfigToEeprom(uint16_t start,uint16_t cnt, uint8_t* buf)
{
  return WriteEeprom(GetConfigStart() + start,cnt,buf);
}
//--------------------------------------------------------------------------------------

