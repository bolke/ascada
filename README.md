# aSCADA
Arduino (Atmega328) Scada base. Scada (Supervisory Control and Data Acquisition) system using arduino as base.
Communication is done through modbus rtu. Acts as a platform for different hardware projects which communicate through 
modbus. Atmega328 is what I use, but it should be fairly easy to adapt to any other platform. The only thing it needs 
at this moment is a serial port and eeprom, and the eeprom is option. You could hardcode the eeprom.

On startup, aSCADA tries to load settings from EEPROM and configure the device accordingly. If configured correctly, the EEPROM contains the following: 
```
----------------------------------------------------------------------------
address|name                 |default value |description
----------------------------------------------------------------------------
0000   |MAGIC NUMBER byte 0  |0xC3          |syncing addres, used to make sure
0001   |MAGIC NUMBER byte 1  |0x69          |there is usefull data available
0002   |config address byte 0|0x00          |where to find the config, 16 bit
0003   |config address byte 1|0x04          |address, somewhere in eeprom
----------------------------------------------------------------------------
from here use the config address as base (0004 == config address default)
----------------------------------------------------------------------------
0004   |baudrate byte 0      |0x00          |the baudrate is 4 bytes (uint32_t) 
0005   |baudrate byte 1      |0x00          |big, 9600 baud default speed.
0006   |baudrate byte 2      |0x25          |
0007   |baudrate byte 4      |0x80          |
0008   |unit id              |0xF7          |the modbus id, 247 by default
0009   |startup setting      |0x03          |bit settings used when starting
-----------------------------------------------------------------
```

If the settings fail to load from eeprom, the device will revert to default modbus settings. The default modbus serial settings are 9600 8n1. Unit id default is 0xF7.

# Modbus RTU
Contains a modbus-rtu client implementation. Modbus is one master, multiple clients, which only respond on addressed requests. 
Because of history naming of addresses are a bit weird, and aren't used anymore for the purposes that they were created for.

```
-------------------------------------------------------------------------------------------------------
r/w  func|coil/register|data address |type|r/w|original name                   |description
-------------------------------------------------------------------------------------------------------
1/5      |1-9999       |0x0000-0x270E|bit |r/w|Discrete output coils.          |read and write bits
2        |10001-19999  |0x0000-0x270E|bit |r  |Discrete input contacts.        |read only bits
4        |30001-39999  |0x0000-0x270E|byte|r  |Analog input registers.         |read only uint16_t
3/6      |40001-49999  |0x0000-0x270E|byte|r/w|Analog output holding registers.|read and write uint16_t
--------------------------------------------------------------------------------------------------------
```

Device registers are mapped into holding registers. The atmega328p addresses described in the atmega328p datasheet are read and writeable at 0x0000 to 0x00C7, all 199 registers. 
EEPROM registers are mapped onto holding registers 0x00C7 to 0x02C7, 512 registers of 16 bit (1024 8 bit EEPROM registers). Read and writeable through modbus.
See http://www.simplymodbus.ca/ for an explanation of modbus (rtu/tcp/rtu over tcp/etc). 

# Registers
Modbus makes it possibly to read data from the device through a serial protocol. aSCADA maps functions to address ranges. This is done in 
Registers.h. There is an array of mapped functions in flash (saving memory) with starting addresses and adress ranges. The addresses used in 
these definitions are modbus addresses, but when the coupled function is called the address is remapped to start at 0 (requested address - starting
address). The addresses used are the addresses in the table above, in hex format. 

```
    MB_READ_RANGE(0x9C41,0x00C7,ReadDeviceReg),           // read a device register
    MB_WRITE_RANGE(0x9C41,0x00C7,WriteDeviceReg),         // write to a device register, might break / brick or halt device
    MB_READ_RANGE(0x9D08,0x0200,ReadEepromReg),           // read from eeprom, 0x9D08 == eeprom address 0, all addresses are mapped
    MB_WRITE_RANGE(0x9D08,0x0200,WriteEepromReg),         // write to eeprom, 0x9D08 == eeprom address 0, all addresses are mapped
    MB_READ_RANGE(0x7531,0x0008,ReadVersionReg),          // read the project version information (VDATE and VTIME)
    MB_READ(0x7539,ReadStatusReg),                        // read the status register, which is only 16 bit in size
    MB_READ(0x753A,ReadSettingsReg),                      // read data from the settings struct, which is also only 16 bit 
    MB_READ_RANGE(0x753B,0x0002,ReadUptimeReg),           // read uptime, uint32_t value, passed the value that millis() returns
    MB_READ_RANGE(0x753D,ALARM_WORD_CNT,ReadAlarmReg),    // read alarm bits, ALARM_WORD_CNT is calculated at compile time
    MB_WRITE_RANGE(0x0001,0x0003,WriteFuncCoil),          // bits to toggle, use these to toggle function (simple rpc ... kinda)
    MB_READ_RANGE(0x9F08,0x200,prReadRegister),           // 256 multi purpoise registers for the project
    MB_WRITE_RANGE(0x9F08,0x200,prWriteRegister)          // reading and writing
```

If you want to make any ranges available, add the functions, increment the MB_CNT #define, and add the ranges. Ranges are bound 
to how modbus works, so the registers / functions / read - writing etc are bound to what addresses are used. 

## Holding register
  00-c7 == device registers
  c7-4c6 == eeprom registers
  
## Input register
```
  0-7 == version information 
    Mmm dd yyyy hh:mm:ss, returned without spaces, as seen in the following example:
	Jan012020 151905
  8 == cl_ds.status
    /* bit description
     * [0] running 
     * [1] halted bit 0 (0==false,1==halted,2==paused,3==stopped)
     * [2] halted bit 1 
     * [3] unexpected shutdown bit 0 same as register 0x54 bit 1/2/3
     * [4] unexpected shutdown bit 1
     * [5] unexpected shutdown bit 2
     * [6] config loaded 
     * [7] alarm available
     * [8]
     * [9]
     * [A]
     * [B]
     * [C]
     * [D]
     * [E]
     * [F]
     */
  9 number of alarm words (total alarm count is word count * 16)
  A millis low bytes
  B millis high bytes
  C-1C alarm registers, default hex 10 in size
```
  
## WriteBit
```
  80 ff00 start() or resume()
  81 ff00 pause()
  82 ff00 stop()
```