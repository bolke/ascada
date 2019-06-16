#ifndef PROJECT_H                                                                       //
  #define PROJECT_H                                                                     //
                                                                                        //
  #include "Tools.h"                                                                    //
  #include "ProjectGpio.h"
  
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
  typedef struct                                                                        //
  {                                                                                     //
    gpioDef_t gpio[GPIO_CNT];                                                           //project gpio definition
    uint16_t alarms[ALARM_WORD_CNT] = {0};                                              //project alarm definition
  }                                                                                     //
  pr_t;                                                                                 //protect data structure type definition
                                                                                        //
  extern pr_t pr_ds;                                                                    //project data structure
                                                                                        //
  uint8_t prSetup();
  uint8_t prLoop();
  
#endif                                                                                  //
