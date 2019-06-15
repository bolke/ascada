#ifndef PROJECTDEFINES_H                                                                //
#define PROJECTDEFINES_H                                                                //
                                                                                        //
  #define VDATE                         "Jun 15 2019"                                   //build date, used as version, format "MMM DD YYYY"
  #define VTIME                         "21:39:12"                                      //build time, used as version, format "hh:mm:ss"
                                                                                        //
  #define PROJECT_ID                    "00"                                            //unique project identifier, format base64 "xx"
  #define MAGIC_NUMBER                  0xC369                                          //eeprom value used to check if eeprom is set
  #define MAGIC_ADDRESS                 0x00                                            //eeprom address to read the magic number from
                                                                                        //
  #define ALARM_WORD_CNT                0x10                                            //number of reserved alarm words 
  #define ALARM_BIT_CNT                 (ALARM_WORD_CNT*0x10)                           //total number of alarms
                                                                                        //
  #define GPIO_CNT                      0x15                                            //total number of gpio devices, connected to device
  #define GPIO_DEFAULT                  GPIO_INPUT_HIGH                                 //default value of unused gpio
                                                                                        //
  #define GPIO_INPUT_LOW                0x01                                            //input low value first declared, as it is default for input
  #define GPIO_INPUT                    GPIO_INPUT_LOW                                  //default value for gpio input   
  #define GPIO_INPUT_HIGH               0x02                                            //
  #define GPIO_OUTPUT_LOW               0x03                                            //
  #define GPIO_OUTPUT                   GPIO_OUTPUT_LOW                                 //
  #define GPIO_OUTPUT_HIGH              0x04                                            //
  #define GPIO_INPUT_ANALOG             0x05                                            //
  #define GPIO_OUTPUT_ANALOG            0x06                                            //
  #define GPIO_SPECIAL                  0x07                                            //
  #define GPIO_IN                       0x00                                            //
  #define GPIO_OUT                      0x01                                            //
  #define GPIO_LOW                      0x00                                            //
  #define GPIO_HIGH                     0x01                                            //
                                                                                        //
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
                                                                                        //
#endif                                                                                  //
