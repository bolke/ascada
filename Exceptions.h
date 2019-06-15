#ifndef EXCEPTIONS_H                                                                    //exception header definition
  #define EXCEPTIONS_H                                                                  //
                                                                                        //
    #define EXCEPTION_NONE 0x00                                                         //no exception present
    #define EXCEPTION_INVALID_FUNCTION 0x01                                             //invalid function code in request
    #define EXCEPTION_INVALID_ADDRESS 0x02                                              //invalid address in request
    #define EXCEPTION_INVALID_VALUE 0x03                                                //invalid value in request, depends upon value usage
    #define EXCEPTION_SLAVE_DEVICE_FAILURE 0x04                                         //failure when executing request
    #define EXCEPTION_ACKNOWLEDGE 0x05                                                  //received request, couldn't finish on time
    #define EXCEPTION_SLAVE_DEVICE_BUSY 0x06                                            //can't execute request at the moment
    #define EXCEPTION_NEGATIVE_ACKNOWLEDGE 0x07                                         //can't acknowledge request
    #define EXCEPTION_NOT_RUNNING 0x08                                                  //can't do stuff, because not in run
                                                                                        //
#endif                                                                                  //
