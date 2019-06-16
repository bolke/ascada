# aSCADA
Arduino Atmega328 Scada base. Scada (Supervisory Control and Data Acquisition) system using arduino as base.
Communication is done through modbus rtu. Acts as a platform for different hardware projects which communicate through 
modbus.

On startup, aSCADA tries to load settings from EEPROM and configure the device accordingly. If configured correctly, the EEPROM contains the following: 
```
-----------------------------------------------------------------
address|name                 |default value |
-----------------------------------------------------------------
0000   |MAGIC NUMBER byte 0  |0xC3          |
0001   |MAGIC NUMBER byte 1  |0x69          |
0002   |config address byte 0|0x00          |
0003   |config address byte 1|0x04          |

0004   |baudrate byte 0      |0x00          |
0005   |baudrate byte 1      |0x00          |
0006   |baudrate byte 2      |0x25          |
0007   |baudrate byte 4      |0x80          |
0008   |unit id              |0xF7          |
0009   |startup setting      |0x03          |
-----------------------------------------------------------------
```

If the settings fail to load from eeprom, the device will revert to default modbus settings. The default modbus serial settings of aSCADA is 9600 8n1. Unit id default is 0xF7.

# Modbus RTU
Contains a modbus-rtu client implementation. Modbus is one master, multiple clients, which only respond on addressed requests. 
Because of history naming of addresses are a bit weird, but that's history.

```
-----------------------------------------------------------------
r/w  func|coil/register|data address|type|r/w|original name
-----------------------------------------------------------------
1/5      |1-9999       |0000-270E   |bit |r/w|Discrete output coils.
2        |10001-19999  |0000-270E   |bit |r  |Discrete input contacts.
4        |30001-39999  |0000-270E   |byte|r  |Analog input registers.
3/6      |40001-49999  |0000-270E   |byte|r/w|Analog output holding registers.
------------------------------------------------------------------
```

See http://www.simplymodbus.ca/ for an explanation of modbus (rtu/tcp/etc). 
