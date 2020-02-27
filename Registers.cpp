#include "Registers.h"
#include "Project.h"
#include "Modbus.h"
#include "ControlLoop.h"                                                                

uint8_t ReadDeviceReg(uint16_t address,uint16_t* value)                                 
{                                                                                       
  *value=GET_REGISTER(address)&0xFF;                                                    
  return EXCEPTION_NONE;                                                                
}                                                                                       

uint8_t WriteDeviceReg(uint16_t address,uint16_t* value)                                
{                                                                                       
  SET_REGISTER(address,(*value)&0XFF);                                                  
  return EXCEPTION_NONE;                                                                
}                                                                                       

uint8_t ReadEepromReg(uint16_t address,uint16_t* value)                                 
{                                                                                       
  union16_t result;                                                                     
  address=address+address;                                                              
  result.buf[0]=EEPROM.read(address);                                                   
  result.buf[1]=EEPROM.read(address+1);                                                 
  *value=result.val;                                                                    
  return EXCEPTION_NONE;                                                                
}                                                                                       

uint8_t WriteEepromReg(uint16_t address,uint16_t* value)                                
{                                                                                       
  union16_t result;                                                                     
  result.val = *value;                                                                  
  address=address+address;                                                              
  EEPROM.write(address,result.buf[0]);                                                  
  EEPROM.write(address+1,result.buf[1]);                                                
  return EXCEPTION_NONE;                                                                
}

uint8_t ReadAlarmReg(uint16_t address,uint16_t* value)                                  
{                                                                                       
  *value=pr_ds.alarms[address];                                                         
  return EXCEPTION_NONE;                                                                
}                                                                                       

uint8_t ReadVersionReg(uint16_t address,uint16_t* value)                                
{                                                                                       
  switch(address)                                                                       
  {                                                                                     
    case 0:                                                                             
      *value=(VDATE[0]<<8)|(VDATE[1]);                                                  
      break;                                                                            
    case 1:                                                                             
      *value=(VDATE[2]<<8)|(VDATE[4]);                                                  
      break;                                                                            
    case 2:                                                                             
      *value=(VDATE[5]<<8)|(VDATE[7]);                                                  
      break;                                                                            
    case 3:                                                                             
      *value=(VDATE[8]<<8)|(VDATE[9]);                                                  
      break;                                                                            
    case 4:                                                                             
      *value=(VDATE[10]<<8)|(' ');                                                      
      break;                                                                            
    case 5:                                                                             
      *value=(VTIME[0]<<8)|(VTIME[1]);                                                  
      break;                                                                            
    case 6:                                                                             
      *value=(VTIME[3]<<8)|(VTIME[4]);                                                  
      break;                                                                            
    case 7:                                                                             
      *value=(VTIME[6]<<8)|(VTIME[7]);                                                  
      break;                                                                            
    default:                                                                            
      return EXCEPTION_INVALID_ADDRESS;                                                 
  }                                                                                     
                                                                                        
  return EXCEPTION_NONE;                                                                
}                                                                                       

uint8_t ReadStatusReg(uint16_t address,uint16_t* value)                                 
{                                                                                       
  *value=cl_ds.status;                                                                  
  return EXCEPTION_NONE;                                                                
}                                                                                       

uint8_t ReadSettingsReg(uint16_t address,uint16_t* value)                               
{                                                                                       
  *value=cl_ds.settings[address];                                                       
  return EXCEPTION_NONE;                                                                
}                                                                                       

uint8_t ReadUptimeReg(uint16_t address,uint16_t* value)                                 
{                                                                                       
  if(address==0)
  {
    *value=(millis()>>16);
  }
  else
  {
    *value=(millis()&0xFFFF);
  }  
  return EXCEPTION_NONE;                                                                
}                                                                                       

uint8_t WriteFuncCoil(uint16_t address,uint16_t* value)                                 
{                                                                                       
  uint8_t result=EXCEPTION_INVALID_ADDRESS;                                             
  if(*value==0xFF00)                                                                    
  {                                                                                     
    switch (address)                                                                    
    {                                                                                   
      case 0x0000:                                                                      
        if (!cl_ds.isRunning &&                                                         
            cl_ds.defaultOffline &&                                                     
            ConfigFromEeprom(0, MB_SETTING_SIZE, mb_ds.settings))                       
        {                                                                               
          mbSetup(mb_ds.baudrate, mb_ds.slaveId);                                       
          result=clStart();                                                             
        }                                                                               
        break;                                                                          
      case 0x0001:                                                                      
        result=clPause();                                                               
        break;                                                                          
      case 0x0002:                                                                      
        result=clStop();                                                                
        break;                                                                          
    }                                                                                   
  }                                                                                     
  else                                                                                  
  {                                                                                     
    result=EXCEPTION_INVALID_VALUE;                                                     
  }                                                                                     
  return result;                                                                        
}                                                                                       
