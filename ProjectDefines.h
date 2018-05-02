#ifndef PROJECTDEFINES_H
#define PROJECTDEFINES_H  

  #define VDATE                         "Apr 03 2018"                                   //__DATE__ //"Apr 03 2018"
  #define VTIME                         "12:14:23"                                      //__TIME__ //"12:14:23"
                            
  #define MAGIC_NUMBER                  0xC369                                          //eeprom value used to check if eeprom is set
  #define MAGIC_ADDRESS                 0x00                                            //eeprom address to read

  #define ALARM_WORD_CNT                0x10
  #define ALARM_BIT_CNT                 (ALARM_WORD_CNT*16)
  
  #define GPIO_CNT                      0x15

  #define GPIO_INPUT                    0x01
  #define GPIO_INPUT_LOW                GPIO_INPUT
  #define GPIO_INPUT_HIGH               0x02  
  #define GPIO_OUTPUT                   0x03
  #define GPIO_OUTPUT_LOW               GPIO_OUTPUT
  #define GPIO_OUTPUT_HIGH              0x04
  #define GPIO_INPUT_ANALOG             0x05
  #define GPIO_OUTPUT_ANALOG            0x06
  #define GPIO_SPECIAL                  0x07
  
  #define GPIO_DEFAULT                  GPIO_INPUT_HIGH  

  #define GPIO_IN                       0x0
  #define GPIO_OUT                      0x1

  #define GPIO_LOW                      0x0
  #define GPIO_HIGH                     0x1
  
  typedef union
  {
    struct
    {
      uint8_t address;                                                                  //what io pin
      uint8_t inOut:1;                                                                  //input(0) or output(1)
      uint8_t lowHigh:1;                                                                //has pullupp enabled when (1), if input, has output default high(1) or low(0)
      uint8_t analog:1;                                                                 //analog reading(1) if input, enables pwm(1), if output
      uint8_t special:1;                                                                //special function enabled(1), spi, i2c, timer, interrupt input, etc    
    };
    uint16_t definition;
  }
  gpioDef_t;
  
#endif

