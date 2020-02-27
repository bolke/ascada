#include "Gpio.h"
#include "Project.h"                                                                    
#include "Registers.h"                                                              
#include "Exceptions.h"                                                                 
#include "ControlLoop.h"                                                                

uint8_t prInitOfflineGpio()                                                             
{                                                                                       
  for(uint16_t i=0;i<GPIO_CNT;i++)                                                      
  {                                                                                     
    pr_ds.gpio[i].address=i;
    if(prInitGpio(&(pr_ds.gpio[i]),GPIO_DEFAULT) != EXCEPTION_NONE)                     
    {                                                                                   
      return EXCEPTION_FAILED_TO_EXECUTE;                                               
    }                                                                                   
  }                                                                                     
  return EXCEPTION_NONE;                                                                
}                                                                                       

uint8_t prInitOnlineGpio()                                                              
{                                                                                       
  for(uint16_t i=0;i<GPIO_CNT;i++)                                                      
  {                                                                                     
    pr_ds.gpio[i].address=i;
    if(prInitGpio(&(pr_ds.gpio[i]),GPIO_DEFAULT) != EXCEPTION_NONE)                     
    {                                                                                   
      clStop();                                                                         
      return EXCEPTION_FAILED_TO_EXECUTE;                                               
    }                                                                                   
  }                                                                                     
  return EXCEPTION_NONE;                                                                
}                                                                                       

uint8_t prInitGpio(gpioDef_t* gpio, uint8_t setting){                                   
  gpio->special=false;                                                                  
  gpio->analog=false;                                                                   
  gpio->inOut = GPIO_IN;                                                                
  gpio->lowHigh = GPIO_LOW;                                                             
  switch(setting)                                                                       
  {                                                                                     
    case GPIO_INPUT_LOW:                                                                
      break;                                                                            
    case GPIO_INPUT_HIGH:                                                               
      gpio->lowHigh = GPIO_HIGH;                                                        
      break;                                                                            
    case GPIO_OUTPUT_LOW:                                                               
      gpio->inOut = GPIO_OUT;                                                           
      break;                                                                            
    case GPIO_OUTPUT_HIGH:                                                              
      gpio->inOut = GPIO_OUT;                                                           
      gpio->lowHigh = GPIO_HIGH;                                                        
      break;                                                                            
    case GPIO_INPUT_ANALOG:                                                             
      gpio->analog=true;                                                                
      break;                                                                            
    case GPIO_OUTPUT_ANALOG:                                                            
      gpio->inOut = GPIO_OUT;                                                           
      gpio->analog=true;                                                                
      break;                                                                            
    case GPIO_SPECIAL:                                                                  
      gpio->special = true;                                                             
      break;                                                                            
    default:                                                                            
      return EXCEPTION_INVALID_VALUE;                                                   
  }                                                                                     
  return prEnableGpio(gpio);
}                                                                                       

uint8_t prEnableGpio(gpioDef_t* gpioDef)                                                
{                                                                                       
  if(!gpioDef->special)                                                                 
  {                                                                                     
    if(gpioDef->inOut==GPIO_IN)                                                         
    {                                                                                   
      if(gpioDef->lowHigh==GPIO_HIGH)                                                   
      {                                                                                 
        pinMode(gpioDef->address, INPUT_PULLUP);                                        
      }                                                                                 
      else                                                                              
      {                                                                                 
        pinMode(gpioDef->address, INPUT);                                               
      }                                                                                 
    }                                                                                   
    else if(gpioDef->inOut==GPIO_OUT)                                                   
    {                                                                                   
      pinMode(gpioDef->address, OUTPUT);                                                
      digitalWrite(gpioDef->address,gpioDef->lowHigh);                                  
    }                                                                                   
  }                                                                                     
  return EXCEPTION_NONE;                                                                
}                                                                                       

uint8_t prSetGpio(gpioDef_t* gpio, uint8_t setting)
{
  switch(setting)                                                                       
  {                                                                                     
    case GPIO_INPUT_LOW:                                                                
      gpio->inOut = GPIO_IN;                                                            
      gpio->lowHigh = GPIO_LOW;                                                         
      break;                                                                            
    case GPIO_INPUT_HIGH:                                                               
      gpio->inOut = GPIO_IN;                                                            
      gpio->lowHigh = GPIO_HIGH;                                                        
      break;                                                                            
    case GPIO_OUTPUT_FLIP:
      gpio->inOut = GPIO_OUT;
      if(gpio->lowHigh == GPIO_LOW)
        gpio->lowHigh=GPIO_HIGH;
      else
        gpio->lowHigh=GPIO_LOW;
      break;
    case GPIO_OUTPUT_LOW:                                                               
      gpio->inOut = GPIO_OUT;                                                           
      gpio->lowHigh = GPIO_LOW;                                                         
      break;                                                                            
    case GPIO_OUTPUT_HIGH:                                                              
      gpio->inOut = GPIO_OUT;                                                           
      gpio->lowHigh = GPIO_HIGH;                                                        
      break;                                                                            
    case GPIO_INPUT_ANALOG:                                                             
      gpio->lowHigh=GPIO_LOW;                                                           
      gpio->inOut = GPIO_IN;                                                            
      gpio->analog=true;                                                                
      break;                                                                            
    case GPIO_OUTPUT_ANALOG:                                                            
      gpio->lowHigh=GPIO_LOW;                                                           
      gpio->inOut = GPIO_OUT;                                                           
      gpio->analog=true;                                                                
      break;                                                                            
    case GPIO_SPECIAL:                                                                  
      gpio->special = true;                                                             
      break;                                                                            
    default:                                                                            
      return EXCEPTION_INVALID_VALUE;                                                   
  }                                                                                     
  return prEnableGpio(gpio);
}

