# USB-UART bridge with 3.3V supply

## Components

|Components                   |URL                                         |
|-----------------------------|--------------------------------------------|
|USB-UART bridge              |http://akizukidenshi.com/catalog/g/gM-08461/|
|Three terminal regulator 3.3V|http://akizukidenshi.com/catalog/g/gI-00538/|
|Pin header (3 pins)          |http://akizukidenshi.com/catalog/g/gC-00167/
|Universal board (16 holes)   |http://akizukidenshi.com/catalog/g/gP-02515/|

# Circuit

Configuration
```
          5V to 3.3V        FT234X
          +------+         +-----+
3.3V o----|      |----o----|     |---------
GND  o----|      |----o----|     |---------  USB cable
TX   o----|      |----o----|     |---------
RX   o----|      |----o----|     |---------
          +------+         +-----+
```

5V to 3.3V step down circuit
```
                +---------+
                |TA48M033F|
                +---------+
                  |  |  |            *1 Capacitor 0.1 micro farad
            +-----+  +  +-----+      *2 Capacitor 47 micro farad
            |        |        |
            |        +--|  |--+
            |        |  *1    |
3.3V o------+        |        +------o 5V
            |        |
            +--|  |--+
                *2   |
GND  o---------------+---------------o GND
TX   o-------------------------------o TX
RX   o-------------------------------o RX
```
