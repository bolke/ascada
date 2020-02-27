#include "Tools.h"                                                                      
#include "Project.h"                                                             
#include <EEPROM.h>                                                                     

bool CheckCrc(uint8_t* buf, uint16_t len)                                               
{                                                                                       
  uint16_t crc = GetCrc16(buf,len-2);                                                   
  uint16_t oldCrc = buf[len-1];                                                         
  oldCrc = (oldCrc << 8) + buf[len-2];                                                  
  return crc == oldCrc;                                                                 
}                                                                                       

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
      {                                                                                 
        crc >>= 1;                                                                      
      }                                                                                 
    }                                                                                   
  }                                                                                     
  return crc;                                                                           
}                                                                                       

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

bool CheckEepromForMagic()                                                              
{                                                                                       
  union16_t magic;                                                                      
  magic.buf[0]=EEPROM.read(MAGIC_ADDRESS);                                              
  magic.buf[1]=EEPROM.read(MAGIC_ADDRESS+1);                                            
  return magic.val == MAGIC_NUMBER;                                                     
}                                                                                       

uint8_t CheckResetRegister()                                                            
{                                                                                       
  uint8_t value=(GET_REGISTER(0x54)&0x0F);                                              
  SET_REGISTER(0x54,0x00);                                                              
  switch(value)                                                                         
  {                                                                                     
    case 0x02:                                                                          
      return EXTERNAL_RESET_FLAG;                                                       
    case 0x04:                                                                          
      return BROWNOUT_RESET_FLAG;                                                       
    case 0x08:                                                                          
      return WATCHDOG_RESET_FLAG;                                                       
  }                                                                                     
  return NO_RESET_FLAG;                                                                 
}                                                                                       

uint16_t GetConfigStartAddress()                                                        
{                                                                                       
  union16_t configStartAddress;                                                         
  configStartAddress.buf[0]=EEPROM.read(MAGIC_ADDRESS+2);                               
  configStartAddress.buf[1]=EEPROM.read(MAGIC_ADDRESS+3);                               
  return configStartAddress.val;                                                        
}                                                                                       

bool ConfigFromEeprom(uint16_t start,uint16_t cnt, uint8_t* buf)                        
{                                                                                       
  return ReadEeprom(GetConfigStartAddress() + start,cnt,buf);                           
}                                                                                       

bool ConfigToEeprom(uint16_t start,uint16_t cnt, uint8_t* buf)                          
{                                                                                       
  return WriteEeprom(GetConfigStartAddress() + start,cnt,buf);                          
}                                                                                       
