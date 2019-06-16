#ifndef MODBUS_H                                                                        //
  #define MODBUS_H                                                                      //
                                                                                        //
  #include "Tools.h"                                                                    //universal definitions
                                                                                        //
  #define DEFAULT_BAUDRATE 9600                                                         //default baudrate, 8n1 setting
  #define DEFAULT_SLAVE_ID 247                                                          //default slave id is 247              
  #define MB_SETTING_SIZE 5                                                             //number of bytes stored in eeprom for modbus              
  #define MESSAGE_LENGTH 0x28                                                           //total buffer, max 40 characters, also max response length              
  #define REGION_RANGE 0x270E                                                           //size of a region
                                                                                        //
  const uint16_t REGION_START[6]                                                        //
  {                                                                                     //
    0x0001,                                                                             //discrete output coils, read write
    0x2711,                                                                             //discrete input contacts, read only
    0x9C41,                                                                             //output holding registers, read write
    0x7531,                                                                             //input registers, read only
    0x0001,                                                                             //discrete output coils, entry for writing
    0x9C41                                                                              //output holding registers, entry for writing
  };                                                                                    //
                                                                                        //
  #define REGION_OUTPUT_COIL_START REGION_START[0x00]                                   //output coil region start
  #define REGION_DISCRETE_INPUT_START REGION_START[0x01]                                //discrete input region start
  #define REGION_INPUT_REGISTER_START REGION_START[0x03]                                //input register region start
  #define REGION_HOLDING_REGISTER_START REGION_START[0x02]                              //holding register region start
  #define REGION_END(start) (start+REGION_RANGE)                                        //return inserted region + range as ending
                                                                                        //
  typedef struct                                                                        //
  {                                                                                     //data structure used in this file
    union16_t address;                                                                  //address used in requests goes here, because msb/lsb flip
    union16_t value;                                                                    //value used in request goes here, because of flipped msb/lsb
    uint16_t silence=0;                                                                 //silence period in timer 0 ticks (4us per tick)
    volatile uint8_t silence_cnt=0;                                                     //silence amount of ticks executed at this moment
    uint8_t silence_ticks=0;                                                            //silence amount of ticks needed to complete silence period
    uint8_t msgPtr = 0;                                                                 //pointer to know where we are with the message
    uint8_t expectedLength = 0;                                                         //expected length of request
                                                                                        //
    union                                                                               //
    {                                                                                   //
      uint8_t msg[MESSAGE_LENGTH] = {0};                                                //message buffer, for both request and response   
      struct                                                                            //
      {                                                                                 //request message block    
        uint8_t msgSlave;                                                               //request slave id
        uint8_t msgFunc;                                                                //request function
      };                                                                                //
    };                                                                                  //
                                                                                        //
    union                                                                               //
    {                                                                                   //settings block start, save able data
      uint8_t settings[MB_SETTING_SIZE]={0};                                            //settings as a byte array
      struct                                                                            //
      {                                                                                 //
        uint32_t baudrate;                                                              //baudrate
        uint8_t slaveId;                                                                //slave id used
      };                                                                                //
    };                                                                                  //
                                                                                        //
    ModbusFuncPtr WriteBit=NULL;                                                        //writing bits function
    ModbusFuncPtr WriteRegister=NULL;                                                   //writing register function
    ModbusFuncPtr ReadBit=NULL;                                                         //reading a bit
    ModbusFuncPtr ReadRegister=NULL;                                                    //reading a register
    ModbusFuncPtr ExecuteFunction=NULL;                                                 //execute given function 
  }                                                                                     //
  mb_t;                                                                                 //type definition of modbus data structure
                                                                                        //
  extern mb_t mb_ds;                                                                    //modbus data structure definition / coupling with variable in cpp file

  uint8_t mbSetup(uint32_t baudrate=DEFAULT_BAUDRATE, uint8_t slaveId=DEFAULT_SLAVE_ID);//modbus setup function, with defaults for baud and slave_id
  void mbSerialEvent();                                                                 //called on serial event, rx, by automagic callback
  void mbTimerEvent();                                                                  //called on timer event, used to keep track of silence, automagic callback
  uint8_t mbHandleModbusRead(uint16_t address,uint16_t* value);                         //handle a read action, with an internal address and put the result in the pointer argument given
  uint8_t mbHandleModbusWrite(uint16_t address,uint16_t* value);                        //handle a write action, with an  internal address and write the given pointer argument    
  uint8_t mbHandleModbusRead(uint16_t address,uint16_t* value);                         //handle a read action, with an internal address and put the result in the pointer argument given
  uint8_t mbHandleModbusWrite(uint16_t address,uint16_t* value);                        //handle a write action, with an  internal address and write the given pointer argument
 
#endif                                                                                  
