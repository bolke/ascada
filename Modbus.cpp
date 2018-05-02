//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <EEPROM.h>                                                                     //used in gathering input registers
#include "Arduino.h"                                                                    //used for serial functions
#include "Modbus.h"                                                                     //header for the outside world
#include "Exceptions.h"                                                                 //header containing exception codes
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define REQ_REGION_START (REGION_START[mb_ds.msgFunc-1])                                //return the region coupled with the function
#define REQ_REGION_END (REQ_REGION_START+REGION_RANGE)                                  //return the end of the region coupled with the function
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define BAUDRATE_MIN 1200                                                               //minimum baudrate
#define BAUDRATE_MAX 921600                                                             //maximum baudrate
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define MESSAGE_LENGTH 0x28                                                             //total buffer, max 40 characters, also max response length
#define REQUEST_LENGTH 0x08                                                             //possible request length by default
#define READ_WORD_MAX 0x10                                                              //max 16 registers (Words) read ((MESSAGE_LENGTH - 8) / 2)
#define READ_BIT_MAX 0x100                                                              //max 128 bits read, fits easily in MESSAGE_LENGTH
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
mb_t mb_ds;                                                                             //modbus data structure, accessible through external in header
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t GetExpectedLength()                                                             //return the expected length if checked against
{                                                                                       //
  uint8_t result = REQUEST_LENGTH;                                                      //default is REQUEST_LENGTH (8)
  if(mb_ds.msgFunc > 0)                                                                 //must be larger than 0
  {                                                                                     //
    if(mb_ds.msgFunc>=7)                                                                //none of the 6 default functions
    {                                                                                   //insert any special functions here, if necessary
    }                                                                                   //otherwise just return the REQUEST_LENGTH, as by default
  }                                                                                     //if there's no function, there's no expected length.
  else                                                                                  //
    result=0;                                                                           //set to 0, try again later
  return result;                                                                        //return result;
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t InitSerial(uint32_t baudrate){                                                  //initialize serial
  uint8_t result=EXCEPTION_SLAVE_DEVICE_FAILURE;                                        //preset to failure
  uint16_t silenceTicks=0;                                                              //number of ticks used to trigger timer0 compare interrupt
  if((baudrate<BAUDRATE_MIN)||(baudrate>BAUDRATE_MAX))                                  //check if baud is within range
    baudrate=DEFAULT_BAUDRATE;                                                          //if not, use default baudrate
  mb_ds.baudrate=baudrate;                                                              //save the baud
  mb_ds.silence=(8750000/mb_ds.baudrate);                                               //calculate the silence period, 8 bit per 10 baud, 3.5 char length
  silenceTicks=mb_ds.silence;                                                           //use total silence ticks as base
  while(silenceTicks>255)                                                               //create most precise 8 bit value
    silenceTicks=silenceTicks>>1;                                                       //by shifting to the right
  if(silenceTicks<255)                                                                  //make sure the silence period will be completed
    silenceTicks++;                                                                     //increment by one, a little bit extra waiting time
  cli();                                                                                //stop interrupts
  OCR0A=silenceTicks;                                                                   //setup the timer0 compare value
  TIMSK0 |= (1 << OCIE0A);                                                              //set timer0 interrupt for compare event
  mb_ds.silence_ticks=(mb_ds.silence/silenceTicks)+1;                                   //calculate the amount of times it needs to trigger
  mb_ds.silence_cnt=0;                                                                  //reset the counter
  sei();                                                                                //enable the interrupts
  result=EXCEPTION_NONE;                                                                //return true if success
  Serial.flush();                                                                       //clear any remaining data
  Serial.begin(mb_ds.baudrate);                                                         //set baud to given value
  return result;                                                                        //true on success, failure resets to default
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t mbSetup(uint32_t baudrate, uint8_t slaveId){                                    //modbus setup, using baud and slave id
  uint8_t result=0;                                                                     //on default return 0
  mb_ds.address.val=0;                                                                  //init address value to 0
  mb_ds.value.val=0;                                                                    //init value value to 0
  if(slaveId==0 || slaveId>247)                                                         //if slave id is invalid
    slaveId=DEFAULT_SLAVE_ID;                                                           //set to default slave id
  mb_ds.slaveId=slaveId;                                                                //set slave id
  result=InitSerial(baudrate);                                                          //and set the baudrate to the given value
  TIMSK0|=_BV(OCIE0A);                                                                  //couple timer 0 compare interrupt
  return result;                                                                        //return success of serial setup
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SendBuffer(uint8_t* buf,uint16_t len){                                             //sending of the actual buffer
  union16_t crc;                                                                        //16 bit crc
  crc.val  = GetCrc16(buf,len-2);                                                       //get the crc of whatever we send
  buf[len-2]=crc.buf[0];                                                                //set lsb
  buf[len-1]=crc.buf[1];                                                                //set msb
  Serial.write(buf,len);                                                                //push it out
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HandleException(uint8_t exceptionCode=0x01){                                       //set and send the buffer
  mb_ds.msgFunc|=0x80;                                                                  //set the function to exception function
  mb_ds.msg[2]=exceptionCode;                                                           //set the given code into response
  SendBuffer(mb_ds.msg,0x05);                                                           //and send
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t HandleBroadcast()                                                               //handle a broadcasting message
{                                                                                       //todo take care of the broadcast message
  return EXCEPTION_NONE;                                                                //default return this
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t ReadRegisters()                                                                 //read holding and input registers
{                                                                                       //
  if((mb_ds.value.val>0)&&(mb_ds.value.val<=READ_WORD_MAX))                             //check if the value is in range
  {                                                                                     //
    if(((mb_ds.address.val+mb_ds.value.val)<REQ_REGION_END)){                           //check if the addresses are within range
    {                                                                                   //
      if(mb_ds.ReadRegister!=NULL)                                                      //check if there is a register read function
      {                                                                                 //
        uint8_t result=EXCEPTION_NONE;                                                  //return value
        union16_t data;                                                                 //data container
        uint8_t cnt=0;                                                                  //keeping track of address
        mb_ds.msg[2] = mb_ds.value.val*2;                                               //set number of bytes
        for(uint8_t i=0;i<mb_ds.msg[2];i=i+2)                                           //walk through all the addresses
        {                                                                               //
          result=(*(mb_ds.ReadRegister))(mb_ds.address.val+cnt,&(data.val));            //
          if(result==EXCEPTION_NONE)                                                    //successfully read some data
          {                                                                             //
            mb_ds.msg[3+i]=data.buf[1];                                                 //pop the lsb in
            mb_ds.msg[3+i+1]=data.buf[0];                                               //pop the msb in
            cnt++;                                                                      //address increment
          }                                                                             //
          else                                                                          //
            return result;                                                              //whatever error was returned, return it upwards
        }                                                                               //
        SendBuffer(mb_ds.msg,5+mb_ds.msg[2]);                                           //send the response
        return EXCEPTION_NONE;                                                          //no exception
      }                                                                                 //
    }                                                                                   //
    return EXCEPTION_INVALID_ADDRESS;                                                   //address out of range
  }                                                                                     //
  return EXCEPTION_INVALID_VALUE;                                                       //region out of range
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t ReadBits()                                                                      //
{                                                                                       //
  if(mb_ds.value.val>0&&mb_ds.value.val<=READ_BIT_MAX)                                  //check if the value is in range
      {                                                                                 //
    if(((mb_ds.address.val+mb_ds.value.val)<=REQ_REGION_END))                           //check if the addresses are within range
        {                                                                               //
     if(mb_ds.ReadBit!=NULL)                                                            //check if function has been set
     {                                                                                  //
        uint8_t result=EXCEPTION_NONE;                                                  //keeping track of result
        uint16_t data=0;                                                                //temp var for data read
        uint8_t n=0;                                                                    //byte counter
        mb_ds.msg[2]=mb_ds.value.val>>3;                                                //number of bytes used in response
        if(mb_ds.msg[2]==0)                                                             //if it's still zero
          mb_ds.msg[2]=1;                                                               //set to 1, always one byte used
        mb_ds.msg[3] = 0;                                                               //set first byte to 0
        for(uint16_t i=0;i<mb_ds.value.val;i++)                                         //loop through all bits
            {                                                                           //
        if((i>0) && ((i%8)==0))                                                         //check if it's a new byte
        {                                                                               //
        n++;                                                                            //increment byte counter
            mb_ds.msg[3+n] = 0;                                                         //reset the byte to 0
          }                                                                             //
          result=(*(mb_ds.ReadBit))(mb_ds.address.val+i,&data);                         //request a bit value
          if(result==EXCEPTION_NONE)                                                    //on success
          {                                                                             //
            if(data!=0)                                                                 //and if it's set
              mb_ds.msg[3+n] |= 1<<(i%8);                                               //put it into the byte if set
          }                                                                             //
          else                                                                          //
            return result;                                                              //return error code
        }                                                                               //
        SendBuffer(mb_ds.msg,5+mb_ds.msg[2]);                                           //send the response
        return EXCEPTION_NONE;                                                          //no exception
      }                                                                                 //
      return EXCEPTION_SLAVE_DEVICE_FAILURE;                                            //no bit reading function is registered
    }                                                                                   //
    return EXCEPTION_INVALID_ADDRESS;                                                   //return invalid address exception
  }                                                                                     //
  return EXCEPTION_INVALID_VALUE;                                                       //return invalid value exception
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t HandleMisc()                                                                   //handling any non default functions
    {                                                                                  //
  switch(mb_ds.msgFunc)                                                                //handle the received request
  {                                                                                    //
    default:                                                                           //by default use the following function pointer
      if(mb_ds.ExecuteFunction!=NULL)                                                  //this function pointer
        return (*(mb_ds.ExecuteFunction))(mb_ds.msgFunc,NULL);                         //and call it, returning the result of the function
      return EXCEPTION_INVALID_FUNCTION;                                               //otherwise return not available
  }                                                                                    //
}                                                                                      //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t HandleRequest()                                                                 //handling a complete request
{                                                                                       //
  uint8_t result=EXCEPTION_NONE;                                                        //default is success
  if(mb_ds.msgSlave==0)                                                                 //message for everyone? (including me)
  {                                                                                     //
    if(CheckCrc(mb_ds.msg,mb_ds.msgPtr))                                                //check crc
      result=HandleBroadcast();                                                         //handle it
  }                                                                                     //
  else if((mb_ds.msgSlave==mb_ds.slaveId))                                              //message for me
  {                                                                                     //
    if(CheckCrc(mb_ds.msg,mb_ds.msgPtr)){                                               //check crc
      if(mb_ds.msgFunc<7){                                                              //default function
        mb_ds.address.buf[0]=mb_ds.msg[3];                                              //address msb
        mb_ds.address.buf[1]=mb_ds.msg[2];                                              //address lsb
        mb_ds.value.buf[0]=mb_ds.msg[5];                                                //value msb
        mb_ds.value.buf[1]=mb_ds.msg[4];                                                //value lsb
        if(mb_ds.address.val <= REGION_RANGE){                                          //address within range
          mb_ds.address.val+=REQ_REGION_START;                                          //set address in correct range
          switch(mb_ds.msgFunc){                                                        //handle default functions
            case 1:                                                                     //read coil status (bit)
            case 2:                                                                     //read input status (bit)
              result=ReadBits();                                                        //read the requested bits
              break;                                                                    //and done
            case 3:                                                                     //read holding registers (word)
            case 4:                                                                     //read input registers (word)
              result=ReadRegisters();                                                   //read the requested registers
              break;                                                                    //and done
            case 5:                                                                     //write coil (bit)
              if(mb_ds.WriteBit!=NULL){                                                 //is function set, then write
                result=(*(mb_ds.WriteBit))(mb_ds.address.val,&(mb_ds.value.val));       //
                if(result==EXCEPTION_NONE)                                              //on success
                  SendBuffer(mb_ds.msg,mb_ds.msgPtr);                                   //send the buffer
              }else                                                                     //otherwise, return failure
                result=EXCEPTION_SLAVE_DEVICE_FAILURE;                                  //can't execute, slave error
              break;                                                                    //and done
            case 6:                                                                     //write holding register (word)
              if(mb_ds.WriteRegister!=NULL)                                             //
              {                                                                         //check me writing bits function
                result=(*(mb_ds.WriteRegister))(mb_ds.address.val,&(mb_ds.value.val));  //
                if(result==EXCEPTION_NONE)                                              //on success
                  SendBuffer(mb_ds.msg,mb_ds.msgPtr);                                   //return the request
              }                                                                         //
              else                                                                      //otherwise the writing bit function hasn't been set
                result=EXCEPTION_SLAVE_DEVICE_FAILURE;                                  //can't execute, slave error
              break;                                                                    //and done
          }                                                                             //
        }                                                                               //
        else                                                                            //address exception
          result=EXCEPTION_INVALID_ADDRESS;                                             //address exception code
      }                                                                                 //
      else                                                                              //otherwise a non default function
        result=HandleMisc();                                                            //and that also may be handled
    }                                                                                   //ignore if crc fails
    if(result!=EXCEPTION_NONE)                                                          //if there is a exception
      HandleException(result);                                                          //send it back
  }                                                                                     //otherwise no for us
  return result;                                                                        //return result, 0 == success
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void mbTimerEvent()                                                                     //
{                                                                                       //interrupts for timer 0 compare trigger
  if(mb_ds.silence_cnt<=mb_ds.silence_ticks)                                            //check if we have reached the limit
    mb_ds.silence_cnt++;                                                                //increment counter if not
}                                                                                       //ignore timer otherwise
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void mbSerialEvent()                                                                    //
{                                                                                       //
  bool ignore=(mb_ds.silence_cnt>mb_ds.silence_ticks)&&(mb_ds.msgPtr>0);                //
  if(ignore)                                                                            //
  {                                                                                     //on ignore reset the buffer
    mb_ds.msgPtr=0;                                                                     //reset buffer
    mb_ds.expectedLength=0;                                                             //default expected length value
    ignore=false;                                                                       //start reading data as if it's a new message
  }                                                                                     //
  while(Serial.available())                                                             //
  {                                                                                     //if there is data
    if(ignore)                                                                          //if there are leftovers and silence period has expired
      Serial.read();                                                                    //ignore
    else{                                                                               //
      if((mb_ds.msgPtr<MESSAGE_LENGTH))                                                 //
      {                                                                                 //can't store more than buffer size
        mb_ds.msg[mb_ds.msgPtr]=Serial.read();                                          //put data into buffer
        mb_ds.msgPtr++;                                                                 //increment counter
        if(mb_ds.expectedLength==0)                                                     //keep reading if expected length is unknown
          mb_ds.expectedLength=GetExpectedLength();                                     //set expected length
        if(mb_ds.msgPtr==mb_ds.expectedLength)                                          //
        {                                                                               //is there a complete request
          HandleRequest();                                                              //handle it
          ignore=true;                                                                  //ignore leftovers
        }                                                                               //
      }                                                                                 //
    }                                                                                   //
  }                                                                                     //
  mb_ds.silence_cnt=0;                                                                  //reset counter
  if(ignore)                                                                            //
  {                                                                                     //on ignore reset the buffer
    mb_ds.msgPtr=0;                                                                     //reset buffer
    mb_ds.expectedLength=0;                                                             //default expected length value
  }                                                                                     //
}                                                                                       //
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
