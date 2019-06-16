#ifndef PROJECT_H                                                                       //
  #define PROJECT_H                                                                     //
                                                                                        //
  #include "Tools.h"                                                                    //
  #include "ProjectDefines.h"                                                           //
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
  
  typedef struct                                                                        //
  {                                                                                     //
    gpioDef_t gpio[GPIO_CNT];                                                           //project gpio definition
    uint16_t alarms[ALARM_WORD_CNT] = {0};                                              //project alarm definition
  }                                                                                     //
  pr_t;                                                                                 //protect data structure type definition
                                                                                        //
  extern pr_t pr_ds;                                                                    //project data structure
                                                                                        //
  uint8_t prOfflineGpio();                                                              //
  uint8_t prOnlineGpio();                                                               //
  uint8_t prDefaultGpio();                                                              //
  uint8_t prInitOnlineGpioDef();                                                        //run this when starting to initialize the gpio
  uint8_t prInitOfflineGpioDef();                                                       //run this when offline to initialize the gpio
                                                                                        //
  uint8_t prInitGpioDef(gpioDef_t* gpioDef, uint8_t setting);                           //
  uint8_t prEnableGpio(gpioDef_t* gpioDef);                                             //
#endif                                                                                  //
