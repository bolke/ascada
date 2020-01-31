#ifndef PROJECT_H                                                                      
  #define PROJECT_H                                                                    
                                                                                       
  #include "Tools.h"                                                                   
  #include "Gpio.h"
  
  #define VDATE                         "Jan 28 2020"                                   //build date, used as version, format "MMM DD YYYY"
  #define VTIME                         "23:16:12"                                      //build time, used as version, format "hh:mm:ss"
                                                                                        
  #define PROJECT_ID                    "00"                                            //unique project identifier, format base64 "xx"
  #define MAGIC_NUMBER                  0xC369                                          //eeprom value used to check if eeprom is set
  #define MAGIC_ADDRESS                 0x00                                            //eeprom address to read the magic number from
                                                                                        
  #define ALARM_WORD_CNT                0x10                                            //number of reserved alarm words 
  #define ALARM_BIT_CNT                 (ALARM_WORD_CNT*0x10)                           //total number of alarms, number of words time 16
                                                                                        
  #define GPIO_CNT                      0x15                                            //total number of gpio devices, connected to device
  #define GPIO_DEFAULT                  GPIO_INPUT_HIGH                                 //default value of unused gpio
                
  typedef struct                                                                        
  {                                                                                     
    gpioDef_t gpio[GPIO_CNT];                                                           //project gpio definition
    uint16_t alarms[ALARM_WORD_CNT] = {0};                                              //project alarm definition
  }                                                                                     
  pr_t;                                                                                 //protect data structure type definition
                                                                                        
  extern pr_t pr_ds;                                                                    //project data structure for external use
                                                                                        
  uint8_t prSetup();                                                                    //project initialize, run once at startup
  uint8_t prLoop();                                                                     //project run
  uint8_t prReadRegister(uint16_t address,uint16_t* value);                             //read a project register, project specific modbus reading
  uint8_t prWriteRegister(uint16_t address,uint16_t* value);                            //write a project register, project specific modbus writing

  #endif                                                                                  
