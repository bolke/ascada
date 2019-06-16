#ifndef PROJECT_H                                                                       //
  #define PROJECT_H                                                                     //
                                                                                        //
  #include "Tools.h"                                                                    //
  #include "ProjectDefines.h"                                                           //
                                                                                        //
  typedef struct                                                                        //
  {                                                                                     //
    uint16_t alarms[ALARM_WORD_CNT] = {0};                                              //project alarm definition
  }                                                                                     //
  pr_t;                                                                                 //protect data structure type definition
                                                                                        //
  extern pr_t pr_ds;                                                                    //project data structure
#endif                                                                                  //
