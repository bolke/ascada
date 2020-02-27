#ifndef PROJECT_H                                                                      
  #define PROJECT_H                                                                    
                                                                                       
  #include "Tools.h"                                                                   
  #include "Gpio.h"
  
  #define VDATE                         "Jan 28 2020"                                   
  #define VTIME                         "23:16:12"                                      
                                                                                        
  #define PROJECT_ID                    "00"                                            
  #define MAGIC_NUMBER                  0xC369                                          
  #define MAGIC_ADDRESS                 0x00                                            
                                                                                        
  #define ALARM_WORD_CNT                0x10                                            
  #define ALARM_BIT_CNT                 (ALARM_WORD_CNT*0x10)                           
                                                                                        
  #define GPIO_CNT                      0x15                                            
  #define GPIO_DEFAULT                  GPIO_INPUT_HIGH                                 
                
  typedef struct                                                                        
  {                                                                                     
    gpioDef_t gpio[GPIO_CNT];                                                           
    uint16_t alarms[ALARM_WORD_CNT] = {0};                                              
  }                                                                                     
  pr_t;                                                                                 
                                                                                        
  extern pr_t pr_ds;                                                                    
                                                                                        
  uint8_t prSetup();                                                                    
  uint8_t prLoop();                                                                     
  uint8_t prReadRegister(uint16_t address,uint16_t* value);                             
  uint8_t prWriteRegister(uint16_t address,uint16_t* value);                            

  #endif                                                                                
