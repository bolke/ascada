//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "ControlLoop.h"                                                                //
#include "Modbus.h"                                                                     //
#include "Tools.h"                                                                      //
#include "Project.h"                                                                    //
#include "ModbusMapping.h"                                                              //           
#include "Project.h"                                                                    //
#include "Exceptions.h"                                                                 //
#include "Arduino.h"                                                                    //
#include <EEPROM.h>                                                                     //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
cl_t cl_ds;                                                                             //control loop data structure
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t clSetup()                                                                       //
{                                                                                       //
  if(CheckEepromForMagic())                                                             //check if a magic number of on a certain address
  {                                                                                     //
    if(ConfigFromEeprom(0, MB_SETTING_SIZE, mb_ds.settings))                            //load modbus settings from eeprom, into mb_ds.settings
    {                                                                                   //
      ConfigFromEeprom(MB_SETTING_SIZE, sizeof(mb_ds.settings), cl_ds.settings);        //read control loop settings from eeprom
      cl_ds.configLoaded=true;                                                          //set the configloaded flag
    }                                                                                   //
  }                                                                                     //
                                                                                        //
  cl_ds.unexpectedShutdown=CheckResetRegister();                                        //check the reset register and set the unexpected shutdown flag
                                                                                        //
  mb_ds.ReadRegister=HandleModbusRead;                                                  //set modbus datastructure register read function
  mb_ds.ReadBit=HandleModbusRead;                                                       //set modbus datastructure bit read function
  mb_ds.WriteRegister=HandleModbusWrite;                                                //set modbus datastructure register write function
  mb_ds.WriteBit=HandleModbusWrite;                                                     //set modbus datastructure bit write function
                                                                                        //
  if(cl_ds.configLoaded)                                                                //if the configuration has been loaded from eeprom
  {                                                                                     //
    if((!cl_ds.startRunning)&&cl_ds.defaultOffline)                                     //check if we start running at boot, or if we use default serial settings when not running (offline)
    {                                                                                   //
      mbSetup(DEFAULT_BAUDRATE, DEFAULT_SLAVE_ID);                                      //load default baudrate and default slave id
    }                                                                                   //
    else                                                                                //
    {                                                                                   //
      mbSetup(mb_ds.baudrate, mb_ds.slaveId);                                           //load baudrate and slave id, out of the loaded configuration
    }                                                                                   //
    if(cl_ds.startRunning)                                                              //start at boot?
    {                                                                                   //
      clStart();                                                                        //start 
    }                                                                                   //
  }                                                                                     //
  else                                                                                  //
  {                                                                                     //
    mbSetup(DEFAULT_BAUDRATE, DEFAULT_SLAVE_ID);                                        //load default baudrate and default slave id
  }                                                                                     //
                                                                                        //
  if(!cl_ds.isRunning)                                                                  //if we arent running
  {                                                                                     //
    prInitOfflineGpioDef();                                                             //initialize the offline gpio settings
    prOfflineGpio();                                                                    //and set to starting settings
  }                                                                                     //
                                                                                        //
  return EXCEPTION_NONE;                                                                //no exceptions
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool clDetectAlarms()                                                                   //check if there are any alarms
{                                                                                       //
  for(uint16_t i=0;i<ALARM_WORD_CNT;i++)                                                //loop through all the alarms
  {                                                                                     //
    if(pr_ds.alarms[i]!=0)                                                              //and check if an alarm has been set
    {                                                                                   //
      return true;                                                                      //if an alarm has been found, return true
    }                                                                                   //
  }                                                                                     //
  return false;                                                                         //no alarms
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t clLoop()                                                                        //
{                                                                                       //
  cl_ds.hasAlarms=clDetectAlarms();                                                     //set the hasAlarms flag
  return 0;                                                                             //
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t clStart()                                                                       //
{                                                                                       //
  uint8_t result=EXCEPTION_NEGATIVE_ACKNOWLEDGE;                                        //no success
  if(cl_ds.configLoaded && !cl_ds.isRunning)                                            //if we have a configuration and aren't started yet
  {                                                                                     //
    if(cl_ds.halted==HALTED_NONE || cl_ds.halted==HALTED_PAUSED)                        //if we aren't halted or if we are paused
    {                                                                                   //
      cl_ds.halted=HALTED_NONE;                                                         //we aren't halted anymore
      cl_ds.isRunning=true;                                                             //and we are running now
      prInitOnlineGpioDef();                                                            //initialize the gpio when online
      result=prOnlineGpio();                                                            //and then set the gpio at their starting values
      if(result!=EXCEPTION_NONE)                                                        //if we arent successfull
      {                                                                                 //
         clStop();                                                                      //stop 
      }                                                                                 //
    }                                                                                   //
  }                                                                                     //
  return result;                                                                        //return the success
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t clStop()                                                                        //stop running
{                                                                                       //
  uint8_t result=EXCEPTION_NEGATIVE_ACKNOWLEDGE;                                        //negative result on failure
                                                                                        //
  if(cl_ds.isRunning ||                                                                 //if we are running
     cl_ds.halted==HALTED_PAUSED)                                                       //or paused
  {                                                                                     //
    cl_ds.isRunning=false;                                                              //we aren't now
    cl_ds.halted=HALTED_STOPPED;                                                        //and we have stopped
    result=EXCEPTION_NONE;                                                              //success
    prInitOfflineGpioDef();                                                             //initialize the gpio for offline running
    prOfflineGpio();                                                                    //and set the values to the correct value
  }                                                                                     //
                                                                                        //
  return result;                                                                        //return the result
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t clPause()                                                                       //pause the runloop
{                                                                                       //
  if(cl_ds.isRunning)                                                                   //only pause if we are running
  {                                                                                     //
    cl_ds.isRunning=false;                                                              //we aren't running anymore
    cl_ds.halted=HALTED_PAUSED;                                                         //and we are paused
    return EXCEPTION_NONE;                                                              //success
  }                                                                                     //
  return EXCEPTION_NEGATIVE_ACKNOWLEDGE;                                                //failure
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t ReadDeviceReg(uint16_t address,uint16_t* value)                                 //read a register from the device and fill the given value argument
{                                                                                       //
  *value=GET_REGISTER(address)&0xFF;                                                    //get the value from the given register, binairy and with the value.
  return EXCEPTION_NONE;                                                                //return success (or lack of exceptions)
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t WriteDeviceReg(uint16_t address,uint16_t* value)                                //
{                                                                                       //
  SET_REGISTER(address,*value);                                                         //write given value to address
  return EXCEPTION_NONE;                                                                //return success (or lack of exceptions)
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t ReadEepromReg(uint16_t address,uint16_t* value)                                 //read a value from eeprom. eeprom is read in bytes
{                                                                                       //
  *value=EEPROM.read(address)&0xFF;                                                     //read a byte an dpop it in the value
  return EXCEPTION_NONE;                                                                //success
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t WriteEepromReg(uint16_t address,uint16_t* value)                                //write a given value to eeprom, only 
{                                                                                       //
  EEPROM.write(address,*value);                                                         //writes a byte value to eeprom, even though a int16 is given (first byte is used)
  return EXCEPTION_NONE;                                                                //success
}            
//--------------------------------------------------------------------------------------
uint8_t ReadAlarmReg(uint16_t address,uint16_t* value)
{
  *value=pr_ds.alarms[address];
  return EXCEPTION_NONE;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t ReadVersionReg(uint16_t address,uint16_t* value)                                //read version information from memory
{                                                                                       //
  switch(address)                                                                       //read all data and return the 16 bit value 
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
  *value=(millis()>>(address*16))&0xFFFF;                                               //uptime read from arduino library millis
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
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t HandleModbusRead(uint16_t address, uint16_t* value)                             //read data from given modbuss address
{                                                                                       //
  modbusMapping_t target;                                                               //the function used to read the given address
  for(uint16_t i=0;i<MB_CNT;i++)                                                        //check all functions in modbus address / function array
  {                                                                                     //
    PROGRAM_READTYPE (&mbMapping[i], target);                                           //get the function from flash into memory
    if(target.isRead &&                                                                 //check if the given function is a reading fucntion
       target.regStart<=address &&                                                      //and address is within the given range stored in the function
       address<(target.regStart+target.regCnt))                                         //
    {                                                                                   //
        return (*(target.funcPtr))(address-target.regStart,value);                      //call the actual function, and return the result
    }                                                                                   //
  }                                                                                     //
  return EXCEPTION_INVALID_ADDRESS;                                                     //given address out of range
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t HandleModbusWrite(uint16_t address, uint16_t* value)                            //
{                                                                                       //
  modbusMapping_t target;                                                               //function to execute
  for(uint16_t i=0;i<MB_CNT;i++)                                                        //loop through all the modbus read write functions
  {                                                                                     //
    PROGRAM_READTYPE (&mbMapping[i], target);                                           //copy from flash to memory
    if(!target.isRead &&                                                                //if it's a read function
       target.regStart<=address &&                                                      //and the address is within range
       address<(target.regStart+target.regCnt))                                         //
    {                                                                                   //
      return (*(target.funcPtr))(address-target.regStart,value);                        //call the registered function
    }                                                                                   //
  }                                                                                     //
  return EXCEPTION_INVALID_ADDRESS;                                                     //failed to write
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void serialEvent()                                                                      //called automaticcaly through serial arduino stuff
{                                                                                       //
  mbSerialEvent();                                                                      //mb serial data parsing
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ISR(TIMER0_COMPA_vect)                                                                  //timer0 callback
{                                                                                       //
  mbTimerEvent();                                                                       //call this though timer, used to reset modbus communication
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
