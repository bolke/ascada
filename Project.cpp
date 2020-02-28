#include "Ascada.h"

#define LED_PIN 0x0D

pr_t pr_ds;                                                                          

unsigned long tick=0;                                                                

uint8_t prReadRegister(uint16_t address,uint16_t* value)
{
  return EXCEPTION_NONE;                                                             
}

uint8_t prWriteRegister(uint16_t address,uint16_t* value)
{
  return EXCEPTION_NONE;                                                             
}

uint8_t prSetup()
{
  tick=millis();
  prSetGpio(&(pr_ds.gpio[LED_PIN]),GPIO_OUTPUT_HIGH);
  return EXCEPTION_NONE;
}

uint8_t prLoop()
{
  if(millis()-tick > 1000)
  {
    GPIO_SET(LED_PIN,GPIO_FLIP);
    tick=millis();
  }
  return EXCEPTION_NONE;
}
