#include "Ascada.h"

cl_t cl_ds;

void setup()
{
  if(CheckEepromForMagic())
  {
    if(ConfigFromEeprom(0, MB_SETTING_SIZE, mb_ds.settings))
    {
      ConfigFromEeprom(MB_SETTING_SIZE, CL_SETTING_SIZE, cl_ds.settings);
      cl_ds.configLoaded=true;
    }
  }

  cl_ds.unexpectedShutdown=CheckResetRegister();

  mb_ds.ReadRegister=mbHandleModbusRead;
  mb_ds.ReadBit=mbHandleModbusRead;
  mb_ds.WriteRegister=mbHandleModbusWrite;
  mb_ds.WriteBit=mbHandleModbusWrite;

  if(cl_ds.configLoaded)
  {
    if((!cl_ds.startRunning)&&cl_ds.defaultOffline)
    {
      mbSetup(DEFAULT_BAUDRATE, DEFAULT_SLAVE_ID);
    }
    else
    {
      mbSetup(mb_ds.baudrate, mb_ds.slaveId);
    }
    if(cl_ds.startRunning)
    {
      clStart();
    }
  }
  else
  {
    mbSetup(DEFAULT_BAUDRATE, DEFAULT_SLAVE_ID);
  }

  if(!cl_ds.isRunning)
  {
    prInitOfflineGpio();
  }
}

void loop()
{
  if(cl_ds.isRunning)
  {
	bool hasAlarms = 0;
	for(uint8_t i=0;i<ALARM_WORD_CNT;i++)
	{		
		if(alarms[i] > 0)
		{
			hasAlarms = 1;
			break;
		}
	}
	cl_ds.alarmAvailable = hasAlarms;
    prLoop();
  }
}

uint8_t clStart()
{
  uint8_t result=EXCEPTION_NEGATIVE_ACKNOWLEDGE;
  if(cl_ds.configLoaded && !cl_ds.isRunning)
  {
    if(cl_ds.halted==HALTED_NONE || cl_ds.halted==HALTED_PAUSED)
    {
      cl_ds.halted=HALTED_NONE;
      cl_ds.isRunning=true;      
      result=prSetup();
      if(result==EXCEPTION_NONE)
      {		  
        prInitOnlineGpio();         
      }
	  else
	  {
        clStop()
	  }
    }
  }
  return result;
}

uint8_t clStop()
{
  uint8_t result=EXCEPTION_NEGATIVE_ACKNOWLEDGE;

  if(cl_ds.isRunning || cl_ds.halted==HALTED_PAUSED)
  {
    cl_ds.isRunning=false;
    cl_ds.halted=HALTED_STOPPED;
    result=EXCEPTION_NONE;
    prInitOfflineGpio();
  }

  return result;
}

uint8_t clPause()
{
  if(cl_ds.isRunning)
  {
    cl_ds.isRunning=false;
    cl_ds.halted=HALTED_PAUSED;
    return EXCEPTION_NONE;
  }
  return EXCEPTION_NEGATIVE_ACKNOWLEDGE;
}

void serialEvent()
{
  mbSerialEvent();
}

ISR(TIMER0_COMPA_vect)
{
  mbTimerEvent();
}
