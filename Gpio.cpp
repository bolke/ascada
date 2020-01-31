#include "Gpio.h"
#include "Project.h"                                                                    
#include "Registers.h"                                                              
#include "Exceptions.h"                                                                 
#include "ControlLoop.h"                                                                
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t prInitOfflineGpioDef()                                                          //
{                                                                                       //
  for(uint16_t i=0;i<GPIO_CNT;i++)                                                      //loop through all the gpio
  {                                                                                     //
    pr_ds.gpio[i].address=i;                                                            //set the address
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
    pr_ds.gpio[i].address=i;                                                            //set the address
    if(prInitGpioDef(&(pr_ds.gpio[i]),GPIO_DEFAULT) != EXCEPTION_NONE)                  //initialize all by default
    {                                                                                   //
      clStop();                                                                         //stop the control loop if setting fails
      return EXCEPTION_FAILED_TO_EXECUTE;                                               //return failure
    }                                                                                   //
  }                                                                                     //
  return EXCEPTION_NONE;                                                                //success
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t prInitGpioDef(gpioDef_t* gpio, uint8_t setting)
{                                                                                       //
  gpio->special=false;                                                                  //set everything to the default settings and change accordingly to 
  gpio->analog=false;                                                                   //the requested setting
  gpio->inOut = GPIO_IN;                                                                //default in and low
  gpio->lowHigh = GPIO_LOW;                                                             //
  switch(setting)                                                                       //
  {                                                                                     //
    case GPIO_INPUT_LOW:                                                                //
      break;                                                                            //
    case GPIO_OUTPUT_LOW:                                                               //
      gpio->inOut = GPIO_OUT;                                                           //
      break;                                                                            //
    case GPIO_OUTPUT_HIGH:                                                              //
      gpio->inOut = GPIO_OUT;                                                           //
    case GPIO_INPUT_HIGH:                                                               //
      gpio->lowHigh = GPIO_HIGH;                                                        //
      break;                                                                            //
    case GPIO_INPUT_ANALOG:                                                             //
      gpio->analog=true;                                                                //
    case GPIO_OUTPUT_ANALOG:                                                            //
      gpio->inOut = GPIO_OUT;                                                           //
      break;                                                                            //
    case GPIO_SPECIAL:                                                                  //
      gpio->special = true;                                                             //
      break;                                                                            //
    default:                                                                            //
      return EXCEPTION_INVALID_VALUE;                                                   //
  }                                                                                     //
  return prEnableGpioDef(gpio);                                                         //
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t prEnableGpioDef(gpioDef_t* gpioDef)                                             //
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
uint8_t prSetGpioDef(gpioDef_t* gpio, uint8_t setting)
{
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
    case GPIO_OUTPUT_FLIP:                                                              //
      gpio->inOut = GPIO_OUT;                                                           //
      if(gpio->lowHigh == GPIO_LOW)                                                     //
        gpio->lowHigh=GPIO_HIGH;                                                        //
      else                                                                              //
        gpio->lowHigh=GPIO_LOW;                                                         //
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
  return prEnableGpioDef(gpio);                                                         //
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t prSetGpio(uint8_t gpioNr, uint8_t setting)
{
  return prSetGpio(&(pr_ds.gpio[gpioNr]),setting);
}
