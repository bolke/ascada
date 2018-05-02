//--------------------------------------------------------------------------------------
#include "ControlLoop.h"
#include "Modbus.h"
#include "Project.h"
#include "Exceptions.h"
#include "Arduino.h"
#include <EEPROM.h>
//--------------------------------------------------------------------------------------
cl_t cl_ds;
//--------------------------------------------------------------------------------------
uint8_t clSetup()
{
  if(CheckEepromForMagic())
  {
    if(ConfigFromEeprom(0, MB_SETTING_SIZE, mb_ds.settings))
    {
      ConfigFromEeprom(MB_SETTING_SIZE, sizeof(mb_ds.settings), cl_ds.settings);
      cl_ds.configLoaded=true;
    }
  }

  cl_ds.unexpectedShutdown=CheckResetRegister();

  mb_ds.ReadRegister=HandleModbusRead;
  mb_ds.ReadBit=HandleModbusRead;
  mb_ds.WriteRegister=HandleModbusWrite;
  mb_ds.WriteBit=HandleModbusWrite;

  if(cl_ds.configLoaded)
  {
    if((!cl_ds.startRunning)&&cl_ds.defaultOffline)
      mbSetup(DEFAULT_BAUDRATE, DEFAULT_SLAVE_ID);
    else
      mbSetup(mb_ds.baudrate, mb_ds.slaveId);
    if(cl_ds.startRunning)
      clStart();
  }
  else
    mbSetup(DEFAULT_BAUDRATE, DEFAULT_SLAVE_ID);

  if(!cl_ds.isRunning)
  {
    prInitOfflineGpioDef();
    prOfflineGpio();
  }

  return EXCEPTION_NONE;
}
//--------------------------------------------------------------------------------------
bool clDetectAlarms()
{
  for(uint16_t i=0;i<ALARM_WORD_CNT;i++)
  {
    if(pr_ds.alarms[i]!=0)
    {
      return true;
      break;
    }
  }
  return false;
}
//--------------------------------------------------------------------------------------
uint8_t clLoop()
{
  cl_ds.hasAlarms=clDetectAlarms();
  return 0;
}
//--------------------------------------------------------------------------------------
uint8_t clStart()
{
  uint8_t result=EXCEPTION_NEGATIVE_ACKNOWLEDGE;
  if(cl_ds.configLoaded && !cl_ds.isRunning)
  {
    if(cl_ds.halted==HALTED_NONE || cl_ds.halted==HALTED_PAUSED)
    {
      cl_ds.halted=HALTED_NONE;
      cl_ds.isRunning=true;
      prInitOnlineGpioDef();
      result=prOnlineGpio();
      if(result!=EXCEPTION_NONE)
         clStop();
    }
  }
  return result;
}
//--------------------------------------------------------------------------------------
uint8_t clStop()
{
  uint8_t result=EXCEPTION_NEGATIVE_ACKNOWLEDGE;

  if(cl_ds.isRunning)
  {
    cl_ds.isRunning=false;
    cl_ds.halted=HALTED_STOPPED;
    result=EXCEPTION_NONE;
  }
  else if(cl_ds.halted==HALTED_PAUSED)
  {
    cl_ds.halted=HALTED_STOPPED;
    result=EXCEPTION_NONE;
  }

  prInitOfflineGpioDef();
  prOfflineGpio();

  return result;
}
//--------------------------------------------------------------------------------------
uint8_t clPause()
{
  if(cl_ds.isRunning)
  {
    cl_ds.isRunning=false;
    cl_ds.halted=HALTED_PAUSED;
    prInitOfflineGpioDef();
    prOfflineGpio();
    return EXCEPTION_NONE;
  }
  return EXCEPTION_NEGATIVE_ACKNOWLEDGE;
}
//--------------------------------------------------------------------------------------
uint8_t ReadDeviceReg(uint16_t address,uint16_t* value)
{
  *value=GET_REGISTER(address)&0xFF;
  return EXCEPTION_NONE;
}
//--------------------------------------------------------------------------------------
uint8_t WriteDeviceReg(uint16_t address,uint16_t* value)
{
  SET_REGISTER(address,*value);
  return EXCEPTION_NONE;
}
//--------------------------------------------------------------------------------------
uint8_t ReadEepromReg(uint16_t address,uint16_t* value)
{
  *value=EEPROM.read(address)&0xFF;
  return EXCEPTION_NONE;
}
//--------------------------------------------------------------------------------------
uint8_t WriteEepromReg(uint16_t address,uint16_t* value)
{
  EEPROM.write(address,*value);
  return EXCEPTION_NONE;
}
//--------------------------------------------------------------------------------------
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
//--------------------------------------------------------------------------------------
uint8_t ReadStatusReg(uint16_t address,uint16_t* value)
{
  *value=cl_ds.status;
  return EXCEPTION_NONE;
}
//--------------------------------------------------------------------------------------
uint8_t ReadSettingsReg(uint16_t address,uint16_t* value)
{
  *value=cl_ds.settings[address];
  return EXCEPTION_NONE;
}
//--------------------------------------------------------------------------------------
uint8_t ReadUptimeReg(uint16_t address,uint16_t* value)
{
  *value=(millis()>>(address*16))&0xFFFF;
  return EXCEPTION_NONE;
}
//--------------------------------------------------------------------------------------
uint8_t WriteFuncCoil(uint16_t address,uint16_t* value)
{
  uint8_t result=EXCEPTION_INVALID_ADDRESS;
  if(*value==0xFF00)
  {
    switch (address)
    {
      case 0x0000:
        result=clStart();
        if ((result==EXCEPTION_NONE)&&cl_ds.defaultOffline)
          if (ConfigFromEeprom(0, MB_SETTING_SIZE, mb_ds.settings))
            mbSetup(mb_ds.baudrate, mb_ds.slaveId);
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
    result=EXCEPTION_INVALID_VALUE;
  return result;
}
//--------------------------------------------------------------------------------------
uint8_t HandleModbusRead(uint16_t address, uint16_t* value)
{
  modbusMapping_t target;
  for(uint16_t i=0;i<MB_CNT;i++)
  {
    PROGRAM_READTYPE (&mbMapping [i], target);
    if(target.isRead && target.regStart<=address)
      if(address<(target.regStart+target.regCnt))
        return (*(target.funcPtr))(address-target.regStart,value);
  }
  return EXCEPTION_INVALID_ADDRESS;
}
//--------------------------------------------------------------------------------------
uint8_t HandleModbusWrite(uint16_t address, uint16_t* value)
{
  modbusMapping_t target;
  for(uint16_t i=0;i<MB_CNT;i++)
  {
    PROGRAM_READTYPE (&mbMapping [i], target);
    if(!target.isRead && target.regStart<=address)
      if(address<(target.regStart+target.regCnt))
        return (*(target.funcPtr))(address-target.regStart,value);
  }
  return EXCEPTION_INVALID_ADDRESS;
}
//--------------------------------------------------------------------------------------
void serialEvent()
{
  mbSerialEvent();
}
//--------------------------------------------------------------------------------------
ISR(TIMER0_COMPA_vect)
{
  mbTimerEvent();
}
//--------------------------------------------------------------------------------------

