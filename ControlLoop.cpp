//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "ControlLoop.h"                                                                //
#include "Modbus.h"                                                                     //
#include "Registers.h"                                                                  //           
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
cl_t cl_ds;                                                                             //control loop data structure
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t clSetup()                                                                       //setup the control loop, load settings and setup the project
{                                                                                       //
  if(CheckEepromForMagic())                                                             //check if a magic number of on a certain address
  {                                                                                     //
    if(ConfigFromEeprom(0, MB_SETTING_SIZE, mb_ds.settings))                            //load modbus settings from eeprom, into mb_ds.settings
    {                                                                                   //
      ConfigFromEeprom(MB_SETTING_SIZE, CL_SETTING_SIZE, cl_ds.settings);               //read control loop settings from eeprom
      cl_ds.configLoaded=true;                                                          //set the configloaded flag to true
    }                                                                                   //
  }                                                                                     //
                                                                                        //
  cl_ds.unexpectedShutdown=CheckResetRegister();                                        //check the reset register and set the unexpected shutdown flag
                                                                                        //
  mb_ds.ReadRegister=mbHandleModbusRead;                                                //set modbus datastructure register read function
  mb_ds.ReadBit=mbHandleModbusRead;                                                     //set modbus datastructure bit read function
  mb_ds.WriteRegister=mbHandleModbusWrite;                                              //set modbus datastructure register write function
  mb_ds.WriteBit=mbHandleModbusWrite;                                                   //set modbus datastructure bit write function
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
  }                                                                                     //
                                                                                        //
  return EXCEPTION_NONE;                                                                //no exceptions
}                                                                                       //                                                                                     //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t clLoop()                                                                        //
{                                                                                       //
  if(cl_ds.isRunning)                                                                   //while running
  {                                                                                     //
    return prLoop();                                                                    //project loop, returns exception (if any)
  }                                                                                     //
  return EXCEPTION_NOT_RUNNING;                                                         //
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
      result=prSetup();
      if(result==EXCEPTION_NONE)                                                        //if we arent successfull
      {         
        result=prSetup();
      }
      if(result!=EXCEPTION_NONE)
      {
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
