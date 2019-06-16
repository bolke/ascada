//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Project.h"                                                                    //
#include "ModbusMapping.h"                                                              //
#include "Exceptions.h"                                                                 //
#include "ControlLoop.h"                                                                //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
pr_t pr_ds;                                                                             //project data structure
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t prInitOfflineGpioDef()                                                          //
{                                                                                       //
  for(uint16_t i=0;i<GPIO_CNT;i++)                                                      //loop through all the gpio
  {                                                                                     //
    if(prInitGpioDef(&(pr_ds.gpio[i]),GPIO_DEFAULT) != EXCEPTION_NONE)                  //initialize all the gpio when offline
    {                                                                                   //
      return EXCEPTION_FAILED_TO_EXECUTE;                                               //failed to initialize, stop with initialisation
    }                                                                                   //
  }                                                                                     //
  return EXCEPTION_NONE;                                                                //success
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t prInitOnlineGpioDef()                                                           //
{                                                                                       //
  for(uint16_t i=0;i<GPIO_CNT;i++)                                                      //loop through all the gpio
  {                                                                                     //
    if(prInitGpioDef(&(pr_ds.gpio[i]),GPIO_DEFAULT) != EXCEPTION_NONE)                  //initialize all by default
    {                                                                                   //
      clStop();                                                                         //
      return EXCEPTION_FAILED_TO_EXECUTE;                                               //return failure
    }                                                                                   //
  }                                                                                     //
  prInitGpioDef(&(pr_ds.gpio[0x0D]),GPIO_OUTPUT_HIGH);                                  //
  return EXCEPTION_NONE;                                                                //
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t prInitGpioDef(gpioDef_t* gpio, uint8_t setting){                                //
  gpio->special=false;                                                                  //
  gpio->analog=false;                                                                   //
  gpio->inOut = GPIO_IN;                                                                //
  gpio->lowHigh = GPIO_LOW;                                                             //
  switch(setting)                                                                       //
  {                                                                                     //
    case GPIO_INPUT_LOW:                                                                //
      gpio->inOut = GPIO_IN;                                                            //
      gpio->lowHigh = GPIO_LOW;                                                         //
      break;                                                                            //
    case GPIO_INPUT_HIGH:                                                               //
      gpio->inOut = GPIO_IN;                                                            //
      gpio->lowHigh = GPIO_HIGH;                                                        //
      break;                                                                            //
    case GPIO_OUTPUT_LOW:                                                               //
      gpio->inOut = GPIO_OUT;                                                           //
      gpio->lowHigh = GPIO_LOW;                                                         //
      break;                                                                            //
    case GPIO_OUTPUT_HIGH:                                                              //
      gpio->inOut = GPIO_OUT;                                                           //
      gpio->lowHigh = GPIO_HIGH;                                                        //
      break;                                                                            //
    case GPIO_INPUT_ANALOG:                                                             //
      gpio->lowHigh=GPIO_LOW;                                                           //
      gpio->inOut = GPIO_IN;                                                            //
      gpio->analog=true;                                                                //
      break;                                                                            //
    case GPIO_OUTPUT_ANALOG:                                                            //
      gpio->lowHigh=GPIO_LOW;                                                           //
      gpio->inOut = GPIO_OUT;                                                           //
      gpio->analog=true;                                                                //
      break;                                                                            //
    case GPIO_SPECIAL:                                                                  //
      gpio->special = true;                                                             //
      break;                                                                            //
    default:                                                                            //
      return EXCEPTION_INVALID_VALUE;                                                   //
  }                                                                                     //
  return EXCEPTION_NONE;                                                                //
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t prEnableGpio(gpioDef_t* gpioDef)                                                //
{                                                                                       //
  if(!gpioDef->special)                                                                 //
  {                                                                                     //
    if(gpioDef->inOut==GPIO_IN)                                                         //
    {                                                                                   //
      if(gpioDef->lowHigh==GPIO_HIGH)                                                   //
      {                                                                                 //
        pinMode(gpioDef->address, INPUT_PULLUP);                                        //
      }                                                                                 //
      else                                                                              //
      {                                                                                 //
        pinMode(gpioDef->address, INPUT);                                               //
      }                                                                                 //
    }                                                                                   //
    else if(gpioDef->inOut==GPIO_OUT)                                                   //
    {                                                                                   //
      pinMode(gpioDef->address, OUTPUT);                                                //
      digitalWrite(gpioDef->address,gpioDef->lowHigh);                                  //
    }                                                                                   //
  }                                                                                     //
  return EXCEPTION_NONE;                                                                //
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t prOfflineGpio()                                                                 //
{                                                                                       //
  uint8_t result = EXCEPTION_NONE;                                                      //
  for(uint16_t i=0;i<GPIO_CNT;i++)                                                      //
  {                                                                                     //
    result = prEnableGpio(&(pr_ds.gpio[i]));                                            //
    if(result!=EXCEPTION_NONE)                                                          //
    {                                                                                   //
      break;                                                                            //
    }                                                                                   //
  }                                                                                     //
  return result;                                                                        //
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t prOnlineGpio()                                                                  //
{                                                                                       //
  prOfflineGpio();                                                                      //
  return EXCEPTION_NONE;                                                                //
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
