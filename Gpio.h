#ifndef GPIO_H
  #define GPIO_H

  #include <stdint.h>
  #include "Project.h"

  #define GPIO_INPUT_LOW                0x01
  #define GPIO_INPUT                    GPIO_INPUT_LOW
  #define GPIO_INPUT_HIGH               0x02
  #define GPIO_OUTPUT_LOW               0x03
  #define GPIO_OUTPUT                   GPIO_OUTPUT_LOW
  #define GPIO_OUTPUT_HIGH              0x04
  #define GPIO_INPUT_ANALOG             0x05
  #define GPIO_OUTPUT_ANALOG            0x06
  #define GPIO_SPECIAL                  0x07
  #define GPIO_OUTPUT_FLIP              0x08
  #define GPIO_FLIP						GPIO_OUTPUT_FLIP
  #define GPIO_IN                       0x00
  #define GPIO_OUT                      0x01
  #define GPIO_LOW                      0x00
  #define GPIO_HIGH                     0x01
  
  #define GPIO_SET(A,B)					prSetGpio(&(pr_ds.gpio[A]),B)
  typedef union
  {
    struct
    {
      uint8_t address;
      uint8_t inOut:1;
      uint8_t lowHigh:1;
      uint8_t analog:1;
      uint8_t special:1;
    };
    uint16_t definition;
  }
  gpioDef_t;

  uint8_t prInitOnlineGpio();
  uint8_t prInitOfflineGpio();

  uint8_t prInitGpio(gpioDef_t* gpioDef, uint8_t setting);
  uint8_t prEnableGpio(gpioDef_t* gpioDef);

  uint8_t prSetGpio(gpioDef_t* gpioDef, uint8_t setting);  
#endif
