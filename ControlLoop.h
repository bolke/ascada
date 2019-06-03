#ifndef CONTROLLOOP_H
#define CONTROLLOOP_H

  #include <stdint.h>
  #include "Tools.h"
  #include "Project.h"
  #include "ModbusMapping.h"

  #define HALTED_NONE 0
  #define HALTED_HALTED 1
  #define HALTED_PAUSED 2
  #define HALTED_STOPPED 3

  typedef struct
  {
    union
    {
      struct
      {
        uint16_t isRunning:1;
        uint16_t halted:2;
        uint16_t unexpectedShutdown:3;
        uint16_t hasAlarms:1;
        uint16_t configLoaded:1;
      };
      uint16_t status = 0;
    };
    union
    {
      uint8_t settings[1]={0};
      struct
      {
        uint8_t startRunning:1;
        uint8_t defaultOffline:1;
      };
    };
  } cl_t;

  extern cl_t cl_ds;

  uint8_t clSetup();
  uint8_t clLoop();
  uint8_t clStart();
  uint8_t clStop();
  uint8_t clPause();
  bool clDetectAlarms();

#endif
