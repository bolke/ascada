#ifndef MODBUS_H
  #define MODBUS_H

  #include "Tools.h"

  #define DEFAULT_BAUDRATE 9600
  #define DEFAULT_SLAVE_ID 247
  #define MB_SETTING_SIZE 5
  #define MESSAGE_LENGTH 0x28
  #define REGION_RANGE 0x270E

  const uint16_t REGION_START[6]
  {
    0x0001,
    0x2711,
    0x9C41,
    0x7531,
    0x0001,
    0x9C41
  };

  #define REGION_OUTPUT_COIL_START REGION_START[0x00]
  #define REGION_DISCRETE_INPUT_START REGION_START[0x01]  
  #define REGION_HOLDING_REGISTER_START REGION_START[0x02]
  #define REGION_INPUT_REGISTER_START REGION_START[0x03]
  #define REGION_END(start) (start+REGION_RANGE)

  typedef struct
  {
    union16_t address;
    union16_t value;
    uint16_t silence=0;
    volatile uint8_t silence_cnt=0;
    uint8_t silence_ticks=0;
    uint8_t msgPtr = 0;
    uint8_t expectedLength = 0;

    union
    {
      uint8_t msg[MESSAGE_LENGTH] = {0};
      struct
      {
        uint8_t msgSlave;
        uint8_t msgFunc;
      };
    };

    union
    {
      uint8_t settings[MB_SETTING_SIZE]={0};
      struct
      {
        uint32_t baudrate;
        uint8_t slaveId;
      };
    };

    ModbusFuncPtr WriteBit=NULL;
    ModbusFuncPtr WriteRegister=NULL;
    ModbusFuncPtr ReadBit=NULL;
    ModbusFuncPtr ReadRegister=NULL;
    ModbusFuncPtr ExecuteFunction=NULL;
  }
  mb_t;

  extern mb_t mb_ds;

  uint8_t mbSetup(uint32_t baudrate=DEFAULT_BAUDRATE, uint8_t slaveId=DEFAULT_SLAVE_ID);
  void mbSerialEvent();
  void mbTimerEvent();
  uint8_t mbHandleModbusRead(uint16_t address,uint16_t* value);
  uint8_t mbHandleModbusWrite(uint16_t address,uint16_t* value);

#endif
