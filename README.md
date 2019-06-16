# aSCADA
Arduino Atmega328 Scada base. Scada (Supervisory Control and Data Acquisition) system using arduino as base.
Communication is done through modbus rtu.

# Modbus RTU
Contains a modbus-rtu client implementation. Modbus is one master, multiple clients, which only respond on addressed requests. 
Because of history naming of addresses are a bit weird, but that's history.

```
-----------------------------------------------------------------
|coil/register|data address|type|original name
-----------------------------------------------------------------
|1-9999       |0000-270E   |bit |Discrete output coils.
|10001-19999  |0000-270E   |bit |Discrete input contacts.
|30001-39999  |0000-270E   |byte|Analog input registers.
|40001-49999  |0000-270E   |byte|Analog output holding registers.
------------------------------------------------------------------
```

See http://www.simplymodbus.ca/ for an explanation of modbus (rtu/tcp/etc). 

# Scada
Will be used for controlling different hardware setups.

# Todo
[ ] Documentation.
[ ] Control loop implementation.
[ ] Loading of saved settings.
[ ] Testing multiple arduino's on a rs485 bus.
