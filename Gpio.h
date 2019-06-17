#ifndef GPIO_H
  #define GPIO_H

  #include "Tools.h"
  
  #define GPIO_INPUT_LOW                0x01                                            //input low value first declared, as it is default for input
  #define GPIO_INPUT                    GPIO_INPUT_LOW                                  //default value for gpio input   
  #define GPIO_INPUT_HIGH               0x02                                            //
  #define GPIO_OUTPUT_LOW               0x03                                            //
  #define GPIO_OUTPUT                   GPIO_OUTPUT_LOW                                 //
  #define GPIO_OUTPUT_HIGH              0x04                                            //
  #define GPIO_INPUT_ANALOG             0x05                                            //
  #define GPIO_OUTPUT_ANALOG            0x06                                            //  
  #define GPIO_SPECIAL                  0x07                                            //
  #define GPIO_OUTPUT_FLIP              0x08                                            //
  #define GPIO_IN                       0x00                                            //
  #define GPIO_OUT                      0x01                                            //
  #define GPIO_LOW                      0x00                                            //
  #define GPIO_HIGH                     0x01                                            //

  typedef union                                                                         //
  {                                                                                     //
    struct                                                                              //
    {                                                                                   //
      uint8_t address;                                                                  //what io pin
      uint8_t inOut:1;                                                                  //input(0) or output(1)
      uint8_t lowHigh:1;                                                                //has pull up enabled when (1), if input, has output default high(1) or low(0)
      uint8_t analog:1;                                                                 //analog reading(1) if input, enables pwm(1), if output
      uint8_t special:1;                                                                //special function enabled(1), spi, i2c, timer, interrupt input, etc    
    };                                                                                  //
    uint16_t definition;                                                                //
  }                                                                                     //
  gpioDef_t;                                                                            //

  uint8_t prDefaultGpio();                                                              //
  uint8_t prInitOnlineGpioDef();                                                        //run this when starting to initialize the gpio
  uint8_t prInitOfflineGpioDef();                                                       //run this when offline to initialize the gpio
                                                                                        //                                                                                        
  uint8_t prInitGpioDef(gpioDef_t* gpioDef, uint8_t setting);                           //
  uint8_t prEnableGpio(gpioDef_t* gpioDef);                                             //

  uint8_t prSetGpio(gpioDef_t* gpioDef, uint8_t setting);
#endif
