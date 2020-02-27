#include "Registers.h"
#include "Project.h"
#include "Modbus.h"
#include "ControlLoop.h"                                                                                                                                                        

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t ReadDeviceReg(uint16_t address,uint16_t* value)                                 //read a register from the device and fill the given value argument
{                                                                                       //
  *value=GET_REGISTER(address)&0xFF;                                                    //get the value from the given register, binairy and with the value.
  return EXCEPTION_NONE;                                                                //return success (or lack of exceptions)
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t WriteDeviceReg(uint16_t address,uint16_t* value)                                //write a register of the device with the least significant byte
{                                                                                       //
  SET_REGISTER(address,(*value)&0XFF);                                                  //write given value to address, values is cut down to least significant bytes
  return EXCEPTION_NONE;                                                                //return success (or lack of exceptions)
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t ReadEepromReg(uint16_t address,uint16_t* value)                                 //read a value from eeprom. eeprom is read in bytes, 2 bytes at a time
{                                                                                       //
  union16_t result;                                                                     //16 bit result
  address=address+address;                                                              //get the actual address, internal 1 byte per adress, so 2 adresses per outside adress
  result.buf[0]=EEPROM.read(address);                                                   //get first byte
  result.buf[1]=EEPROM.read(address+1);                                                 //get second byte
  *value=result.val;                                                                    //push the result value into the referenced variable
  return EXCEPTION_NONE;                                                                //success, no exception
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t WriteEepromReg(uint16_t address,uint16_t* value)                                //write a given value to eeprom, write 16 bytes at a time
{                                                                                       //
  union16_t result;                                                                     //16 bit result value
  result.val = *value;                                                                  //put the given value into the variable
  address=address+address;                                                              //find the actual address
  EEPROM.write(address,result.buf[0]);                                                  //write the first byte to the first address
  EEPROM.write(address+1,result.buf[1]);                                                //write the second byte to the second address
  return EXCEPTION_NONE;                                                                //success
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t ReadAlarmReg(uint16_t address,uint16_t* value)                                  //read an alarm address 
{                                                                                       //
  *value=pr_ds.alarms[address];                                                         //set the value pointer with the requested alarm address
  return EXCEPTION_NONE;                                                                //success
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t ReadVersionReg(uint16_t address,uint16_t* value)                                //read version information from memory
{                                                                                       //
  switch(address)                                                                       //read all data and return the 16 bit value (2 bytes)
  {                                                                                     //
    case 0:                                                                             //
      *value=(VDATE[0]<<8)|(VDATE[1]);                                                  //
      break;                                                                            //
    case 1:                                                                             //
      *value=(VDATE[2]<<8)|(VDATE[4]);                                                  //
      break;                                                                            //
    case 2:                                                                             //
      *value=(VDATE[5]<<8)|(VDATE[7]);                                                  //
      break;                                                                            //
    case 3:                                                                             //
      *value=(VDATE[8]<<8)|(VDATE[9]);                                                  //
      break;                                                                            //
    case 4:                                                                             //
      *value=(VDATE[10]<<8)|(' ');                                                      //
      break;                                                                            //
    case 5:                                                                             //
      *value=(VTIME[0]<<8)|(VTIME[1]);                                                  //
      break;                                                                            //
    case 6:                                                                             //
      *value=(VTIME[3]<<8)|(VTIME[4]);                                                  //
      break;                                                                            //
    case 7:                                                                             //
      *value=(VTIME[6]<<8)|(VTIME[7]);                                                  //
      break;                                                                            //
    default:                                                                            //
      return EXCEPTION_INVALID_ADDRESS;                                                 //failure
  }                                                                                     //
                                                                                        //
  return EXCEPTION_NONE;                                                                //success
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t ReadStatusReg(uint16_t address,uint16_t* value)                                 //return status register
{                                                                                       //
  *value=cl_ds.status;                                                                  //copy to value
  return EXCEPTION_NONE;                                                                //success
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t ReadSettingsReg(uint16_t address,uint16_t* value)                               //return settings register
{                                                                                       //
  *value=cl_ds.settings[address];                                                       //copy to value
  return EXCEPTION_NONE;                                                                //success
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t ReadUptimeReg(uint16_t address,uint16_t* value)                                 //return uptime in milliseconds
{                                                                                       //
  if(address==0)
  {
    *value=(millis()>>16);
  }
  else
  {
    *value=(millis()&0xFFFF);
  }  
  return EXCEPTION_NONE;                                                                //success
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t WriteFuncCoil(uint16_t address,uint16_t* value)                                 //execute a given function (address) with given value.
{                                                                                       //
  uint8_t result=EXCEPTION_INVALID_ADDRESS;                                             //
  if(*value==0xFF00)                                                                    //if the given value is 0xFF00 
  {                                                                                     //
    switch (address)                                                                    //and the address is in range
    {                                                                                   //
      case 0x0000:                                                                      //start function
        if (!cl_ds.isRunning &&                                                         //if we haven't started 
            cl_ds.defaultOffline &&                                                     //and if we are default offline (no boot errors)
            ConfigFromEeprom(0, MB_SETTING_SIZE, mb_ds.settings))                       //load configuration from eeprom
        {                                                                               //
          mbSetup(mb_ds.baudrate, mb_ds.slaveId);                                       //setup modbus serial communication        
          result=clStart();                                                             //start control loop
        }                                                                               //
        break;                                                                          //
      case 0x0001:                                                                      //
        result=clPause();                                                               //pause
        break;                                                                          //
      case 0x0002:                                                                      //
        result=clStop();                                                                //stop
        break;                                                                          //
    }                                                                                   //insert more functions here if necesarry
  }                                                                                     //
  else                                                                                  //
  {                                                                                     //
    result=EXCEPTION_INVALID_VALUE;                                                     //no function coupled to the given address
  }                                                                                     //
  return result;                                                                        //
}                                                                                       //
