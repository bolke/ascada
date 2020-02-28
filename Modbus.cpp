#include "Modbus.h"
#include "Registers.h"

#define REQ_REGION_START (REGION_START[mb_ds.msgFunc-1])
#define REQ_REGION_END (REQ_REGION_START+REGION_RANGE)

#define BAUDRATE_MIN 1200
#define BAUDRATE_MAX 921600

#define MESSAGE_LENGTH 0x28
#define REQUEST_LENGTH 0x08
#define READ_WORD_MAX 0x10
#define READ_BIT_MAX 0x100

mb_t mb_ds;

uint8_t GetExpectedLength()
{
  uint8_t result = REQUEST_LENGTH;
  if(mb_ds.msgFunc > 0)
  {
    if(mb_ds.msgFunc>=7)
    {

    }
  }
  else
  {
    result=0;
  }
  return result;
}

uint8_t InitSerial(uint32_t baudrate)
{
  uint8_t result=EXCEPTION_DEVICE_FAILURE;
  uint16_t silenceTicks=0;
  if((baudrate<BAUDRATE_MIN)||(baudrate>BAUDRATE_MAX))
  {
    baudrate=DEFAULT_BAUDRATE;
  }
  mb_ds.baudrate=baudrate;
  mb_ds.silence=(8750000/mb_ds.baudrate);
  silenceTicks=mb_ds.silence;
  while(silenceTicks>255)
  {
    silenceTicks=silenceTicks>>1;
  }
  if(silenceTicks<255)
  {
    silenceTicks++;
  }
  cli();
  OCR0A=silenceTicks;
  TIMSK0 |= (1 << OCIE0A);
  mb_ds.silence_ticks=(mb_ds.silence/silenceTicks)+1;
  mb_ds.silence_cnt=0;
  sei();
  result=EXCEPTION_NONE;
  Serial.flush();
  Serial.begin(mb_ds.baudrate);
  return result;
}

uint8_t mbSetup(uint32_t baudrate, uint8_t slaveId)
{
  uint8_t result=0;
  mb_ds.address.val=0;
  mb_ds.value.val=0;
  if(slaveId==0 || slaveId>247)
  {
    slaveId=DEFAULT_SLAVE_ID;
  }
  mb_ds.slaveId=slaveId;
  result=InitSerial(baudrate);
  TIMSK0|=_BV(OCIE0A);
  return result;
}

void SendBuffer(uint8_t* buf,uint16_t len)
{
  union16_t crc;
  crc.val  = GetCrc16(buf,len-2);
  buf[len-2]=crc.buf[0];
  buf[len-1]=crc.buf[1];
  Serial.write(buf,len);
}

void HandleException(uint8_t exceptionCode=0x01)
{
  mb_ds.msgFunc|=0x80;
  mb_ds.msg[2]=exceptionCode;
  SendBuffer(mb_ds.msg,0x05);
}

uint8_t HandleBroadcast()
{
  return EXCEPTION_NONE;
}

uint8_t ReadRegisters()
{
  if((mb_ds.value.val>0)&&(mb_ds.value.val<=READ_WORD_MAX))
  {
    if(((mb_ds.address.val+mb_ds.value.val)<REQ_REGION_END))
    {
      if(mb_ds.ReadRegister!=NULL)
      {
        uint8_t result=EXCEPTION_NONE;
        union16_t data;
        uint8_t cnt=0;
        mb_ds.msg[2] = mb_ds.value.val*2;
        for(uint8_t i=0;i<mb_ds.msg[2];i=i+2)
        {
          result=(*(mb_ds.ReadRegister))(mb_ds.address.val+cnt,&(data.val));
          if(result==EXCEPTION_NONE)
          {
            mb_ds.msg[3+i]=data.buf[1];
            mb_ds.msg[3+i+1]=data.buf[0];
            cnt++;
          }
          else
          {
            return result;
          }
        }
        SendBuffer(mb_ds.msg,5+mb_ds.msg[2]);
        return EXCEPTION_NONE;
      }
    }
    return EXCEPTION_INVALID_ADDRESS;
  }
  return EXCEPTION_INVALID_VALUE;
}

uint8_t ReadBits()
{
  if(mb_ds.value.val>0 &&
     mb_ds.value.val<=READ_BIT_MAX)
  {
    if(((mb_ds.address.val+mb_ds.value.val)<=REQ_REGION_END))
    {
      if(mb_ds.ReadBit!=NULL)
      {
        uint8_t result=EXCEPTION_NONE;
        uint16_t data=0;
        uint8_t n=0;
        mb_ds.msg[2]=mb_ds.value.val>>3;
        if(mb_ds.msg[2]==0)
        {
          mb_ds.msg[2]=1;
        }
        mb_ds.msg[3] = 0;
        for(uint16_t i=0;i<mb_ds.value.val;i++)
        {
          if((i>0) && ((i%8)==0))
          {
            n++;
            mb_ds.msg[3+n] = 0;
          }
          result=(*(mb_ds.ReadBit))(mb_ds.address.val+i,&data);
          if(result==EXCEPTION_NONE)
          {
            if(data!=0)
            {
              mb_ds.msg[3+n] |= 1<<(i%8);
            }
          }
          else
          {
            return result;
          }
        }
        SendBuffer(mb_ds.msg,5+mb_ds.msg[2]);
        return EXCEPTION_NONE;
      }
      return EXCEPTION_DEVICE_FAILURE;
    }
    return EXCEPTION_INVALID_ADDRESS;
  }
  return EXCEPTION_INVALID_VALUE;
}

uint8_t HandleMisc()
{
  switch(mb_ds.msgFunc)
  {
    default:
      if(mb_ds.ExecuteFunction!=NULL)
      {
        return (*(mb_ds.ExecuteFunction))(mb_ds.msgFunc,NULL);
      }
      return EXCEPTION_INVALID_FUNCTION;
  }
}

uint8_t HandleRequest()
{
  uint8_t result=EXCEPTION_NONE;
  if(CheckCrc(mb_ds.msg,mb_ds.msgPtr))
  {
    if(mb_ds.msgSlave==0)
    {
      result=HandleBroadcast();
    }
    else if((mb_ds.msgSlave==mb_ds.slaveId))
    {
      if(mb_ds.msgFunc<7)
      {
        mb_ds.address.buf[0]=mb_ds.msg[3];
        mb_ds.address.buf[1]=mb_ds.msg[2];
        mb_ds.value.buf[0]=mb_ds.msg[5];
        mb_ds.value.buf[1]=mb_ds.msg[4];
        if(mb_ds.address.val <= REGION_RANGE)
        {
          mb_ds.address.val+=REQ_REGION_START;
          switch(mb_ds.msgFunc)
          {
            case 1:
            case 2:
              result=ReadBits();
              break;
            case 3:
            case 4:
              result=ReadRegisters();
              break;
            case 5:
              if(mb_ds.WriteBit!=NULL)
              {
                result=(*(mb_ds.WriteBit))(mb_ds.address.val,&(mb_ds.value.val));
                if(result==EXCEPTION_NONE)
                {
                  SendBuffer(mb_ds.msg,mb_ds.msgPtr);
                }
              }
              else
              {
                result=EXCEPTION_DEVICE_FAILURE;
              }
              break;
            case 6:
              if(mb_ds.WriteRegister!=NULL)
              {
                result=(*(mb_ds.WriteRegister))(mb_ds.address.val,&(mb_ds.value.val));
                if(result==EXCEPTION_NONE)
                {
                  SendBuffer(mb_ds.msg,mb_ds.msgPtr);
                }
              }
              else
              {
                result=EXCEPTION_DEVICE_FAILURE;
              }
              break;
          }
        }
        else
        {
          result=EXCEPTION_INVALID_ADDRESS;
        }
      }
      else
      {
        result=HandleMisc();
      }
    }
    if(result!=EXCEPTION_NONE)
    {
      HandleException(result);
    }
  }
  return result;
}

void mbTimerEvent()
{
  if(mb_ds.silence_cnt<=mb_ds.silence_ticks)
  {
    mb_ds.silence_cnt++;
  }
}

void mbSerialEvent()
{
  bool ignore=(mb_ds.silence_cnt>mb_ds.silence_ticks)&&(mb_ds.msgPtr>0);
  if(ignore)
  {
    mb_ds.msgPtr=0;
    mb_ds.expectedLength=0;
    ignore=false;
  }
  while(Serial.available())
  {
    if(ignore)
    {
      Serial.read();
    }
    else if((mb_ds.msgPtr<MESSAGE_LENGTH))
    {
      mb_ds.msg[mb_ds.msgPtr]=Serial.read();
      mb_ds.msgPtr++;
      if(mb_ds.expectedLength==0)
      {
        mb_ds.expectedLength=GetExpectedLength();
      }
      if(mb_ds.msgPtr==mb_ds.expectedLength)
      {
        HandleRequest();
        ignore=true;
      }
    }
  }
  mb_ds.silence_cnt=0;
  if(ignore)
  {
    mb_ds.msgPtr=0;
    mb_ds.expectedLength=0;
  }
}

uint8_t mbHandleModbusRead(uint16_t address, uint16_t* value)
{
  modbusMapping_t target;
  for(uint16_t i=0;i<MB_CNT;i++)
  {
    PROGRAM_READTYPE (&mbMapping[i], target);
    if(target.isRead &&
       target.regStart<=address &&
       address<(target.regStart+target.regCnt))
    {
        return (*(target.funcPtr))(address-target.regStart,value);
    }
  }
  return EXCEPTION_INVALID_ADDRESS;
}

uint8_t mbHandleModbusWrite(uint16_t address, uint16_t* value)
{
  modbusMapping_t target;
  for(uint16_t i=0;i<MB_CNT;i++)
  {
    PROGRAM_READTYPE (&mbMapping[i], target);
    if(!target.isRead &&
       target.regStart<=address &&
       address<(target.regStart+target.regCnt))
    {
      return (*(target.funcPtr))(address-target.regStart,value);
    }
  }
  return EXCEPTION_INVALID_ADDRESS;
}
