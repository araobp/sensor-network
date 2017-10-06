# Plug&play protocol (UART/I2C)

Update: 2017-OCT-7

This is a very simple I/O link supporting Plug&Play. This protocol works over UART and I2C at a minimal cost. I am also considering to support it over CAN.

## Commands over UART

I2C backplane master accepts commands from UART.

I use [Tera Term](
https://ttssh2.osdn.jp/index.html.en) as terminal software on my PC.

Typical setup:
```
[I2C backplane master(scheduler)]---UART---[FTDI]---USB---[Teraterm]
```

I2C backplane slave also accepts same commands as those for I2C backplane master:
```
[I2C backplane slave]---UART---[FTDI]---USB---[Teraterm]
```

Example (at Teraterm):
```
#WHO  <--- I2C backplane master echoes back the command you enter at Teraterm
$:WHO:BACKPLANE-MASTER  <--- Then returns a response to Teraterm
```

In addition to Teraterm, you can use any terminal software (e.g., putty) or any serial port libraries (e.g., "pyserial" for Python or "serialport" for Node.js) to get access to I2C backplane master via UART/USB.

### Terminal software settings

[Menu bar] Setup -> Serial port -> Baud rate
```
115200 for I2C backplane master
9800 for I2C backplane slaves
```

All the commands ends with '\n' (new line char), so new line setting is as follows:

[Menu bar] Setup -> Terminal -> New-line
```
 Receive: LF
 Transmit: LF
 ```

### Device initial configuration

An I2C slave address must be set to the device with WDA command.

If I2C address of the device is 19 then:
```
#WDA:19
WDA:19
```

### Commands

#### WHO (who are you?)
```
I2C backplane master/slave        Teraterm
       |<------------WHO-------------|
       |                             |
       |-----$:WHO:<device_name>---->|
```

#### SAV (save the current setting onto EEPROM)

The current setting is saved to PIC'S EEPROM.

```
I2C backplane master/slave        Teraterm
       |<------------SAV-------------|
```

#### STA (start sending data)
```
I2C backplane master/slave        Teraterm
       |<------------STA-------------|
```

#### STP (stop sending data)
```
I2C backplane master/slave        Teraterm
       |<------------STP-------------|
       |                             |
       |---------*:STP:ACK---------->|
```

#### SET (set the new setting to the device)
```
I2C backplane master/slave        Teraterm
       |<--------SET:<value>---------|
```

#### GET (return the current setting)
```
I2C backplane master/slave        Teraterm
       |<------------GET-------------|
       |                             |
       |--------$:GET:<value>------->|
```

#### WDA (write I2C slave device address)
```
I2C backplane master/slave        Teraterm
       |<------WDA:<device_id>-------|
```

#### RDA (read I2C slave device address)
```
I2C backplane master/slave        Teraterm
       |<------------RDA-------------|
```

### Commands specific to I2C backplane master

#### I2C (set I2C backplane slave address)
By setting I2C slave's device id (I2C address of I2C backplane slave) to I2C backplane master, you can get access to I2C backplane slave via I2C backplane master, and you can send commands to the slave.

address = 1 means I2C backplane Master.

```
I2C backplane master              Teraterm
       |<--I2C:<address in decimal>--|
```

#### MAP (show device map)
```
I2C backplane master                        Teraterm
    |<----------------MAP----------------------|
    |                                          |
    |--$:MAP:<list of slave addresses in CSV-->|

    *1  $:MAP:<list of slave addresses in CSV>
```

#### CSC (clear schedule)
```
I2C backplane master              Teraterm
       |<------------CSC-------------|
```

#### POS (set write schedule position)
```
I2C backplane master              Teraterm
       |<-------POS:<position>-------|
```

#### WSC (write I2C slave address on schedule)
```
I2C backplane master              Teraterm
       |<------WSC:<device id>-------|
```

#### RSC (read schedule)
```
I2C backplane master              Teraterm
       |<------------RSC-------------|
       |                             |
       |------------<*1>------------>|

(*1) example output
$:RSC:0,0,0,0|0,0,0,0|0,0,0,19|0,0,0,0|0,0,0,0|0,0,0,0|0,0,0,0
```

#### SSC (save schedule onto EEPROM)
```
I2C backplane master              Teraterm
       |<------------SSC-------------|
```

#### SFT (set NO_DATA filter for subscribers)

This is a filter for PUBSUB operations to filter out response with NO_DATA from I2C backplane slave, in reseponse to SEN from I2C backplane master.
```
I2C backplane master              Teraterm
       |<------------SFT-------------|
```

#### UFT (unset NO_DATA filter for subscribers)
```
I2C backplane master              Teraterm
       |<------------UFT-------------|
```

#### % (sensor data)
```
I2C backplane master                       Teraterm
  |---%<address>:<type:<data array in CSV>---->|
```

Example:
```
%10:FLOAT:0.09,-0.13,+1.06
```

#### SEN (request sensor data)
If the target device is accelerometer (i2c slave address 19) then:
```
I2C backplane master              Teraterm
       |<-----SEN:<device_id>--------|
       |                             |
       |----------<data>------------>|
```

Example:
```
#SEN:19
%19:FLOAT:0.20,0.17,-0.97
```

### Recommended link start-up sequence

```
I2C backplane master     pyserial(Python)/serialport(Node.js)
      |                              |
      |<------------STP--------------|  Send STP(Stop) to the slave to stop sending data.
      |----------*:STP:ACK---------->|  Confirm that ACK has been received on the master side.
      |                              |  Clear the output buffer on the master side.
      |                              |  Clear the input buffer on the master side.
      |<------------WHO--------------|  Send WHO(Who are you?) to get device ID of the block.
      |-------$:WHO:<device ID>----->|  Keep the device ID on the master side.
      |                              |
      |<---------SET:<value>---------|  Send SET to configure the device.
      |                              |
      |<-----------SAV---------------|  Send SAV to save the config on the slave side.
      |                              |
      |<-----------STA---------------|  Send STA(Start) to make the device start sending data.
      |                              |
```

## I2C Backplane operations

### TLV format of SEN response payload

I2C backplane master sends SEN to I2C backplane slave to fetch sensor data. The data follows this format, and MSB is sent before LSB:

```
+------+------+-------------------+
|Type  |Length|Value              |
|8bit  |8bit  |8bit * length      |
+------+------+-------------------+
```
Type is one of the following data types:
- uint8_t: 0 ~ 255
- int8_t: -128 ~ 127
- uint16_t: 0 ~ 65535
- int16_t: -32768 ~ 32767
- (int16_t)(float * 100): -327.68 ~ 327.67

In case of uint16_t, int16_t and float, Value contains data in this form:
```
+------+------+----------+----------+----+
|Type  |Length|[MSB][LSB]|[MSB][LSB]|... |
+------+------+----------+----------+----+
```

### TLV format of EXT_I2C payload

I2C backplane master sends EXT_I2C to I2C backplane slave to send a char array of extended command. The message follows this format:

```
+-------+------+---------------------------+
|Type   |Length|Value                      |
|EXT_I2C|8bit  |Char array ending with '\0'|
+-------+------+---------------------------+
```

Note that Length is the number of characters including '\0'.

### Operations over I2C backplane

#### Commands

You can send SET, STA, STP, SAV, WHO, SEN and GET commands to I2C backplane slave via I2C backplane master. Refer to I2C command described in the above.

#### PLG operation (detect I2C backplane slave plug-in)

This operation is performed in background at every 480msec(8msec x 60) interval.
```
I2C backplane slaves        I2C backplane master
(dev addr 0x0n)
    |<----General Call PLG------------|
    |-----(I2C ACK bit on)----------->|
    |                                 | start device scan process
      |<----WHO to dev addr 0x01------|
        |<----WHO to dev addr 0x02----|
    |<----WHO to dev addr 0x0n--------|
    |-----<device ID>---------------->| detect new backplane slave
```

#### WHO operation (scan device)

This operation is performed in background at every 4800msec(8msec x 600) interval.
```
I2C backplane slaves        I2C backplane master
    |                                 | start device scan process
      |<----WHO to dev addr 0x01------|
        |<----WHO to dev addr 0x02----|
    |<----WHO to dev addr 0x0n--------|
    |-----<device ID>---------------->| detect new backplane slave
```

#### INV operation (invoke I2C slave address to start measuring data)
```
I2C backplane slave         I2C backplane master
    |<-------------INV----------------| invocation
    |                                 |    delay
    |                                 |      :
    |<-------------SEN----------------|
    |------<data in TLV format>------>|  
```

### Character LCD block (AQM1602XA-RN-GBW)

Note: you can also send all of these commands to I2C backplane slave (AQM1602XA-RN-GBW) via I2C backplane master. Refer to I2C command described in the above.


#### INI (initialize LCD)
```
I2C backplane slave               Teraterm
       |<------------INI-------------|

```

#### CMD (command)
```
I2C backplane slave               Teraterm
      |<---------CMD:<cmd>-----------|
```
"cmd" is one-byte command in decimal.

#### DAT (data)
```
I2C backplane slave               Teraterm
      |<---------DAT:<data>----------|
```
"data" is one-byte data in decimal.

#### CLR (clear)
```
I2C backplane slave               Teraterm
      |<-------------CLR-------------|
```

#### STR (string)
```
I2C backplane slave               Teraterm
      |<---------STR:<string>--------|
```

"string" is string data in ASCII code.

#### CUL (move cursor left)
```
I2C backplane slave               Teraterm
      |<-------------CUL-------------|
```

#### CUR (move cursor right)
```
I2C backplane slave               Teraterm
      |<-------------CUR-------------|
```

#### NWL (new line)
```
I2C backplane slave               Teraterm
      |<-------------NWL-------------|
```

#### HOM (return home)
```
I2C backplane slave               Teraterm
      |<-------------HOM-------------|
```

#### DSP (display)
```
I2C backplane slave               Teraterm
      |<-----DSP<<line1><line2>>-----|
```
line1 and line2 are string data in ASCII code. Both line1 and line2 MUST be 16 characters in length (totally 32 characters).

### Multiple hall sensors block for detecting positions (MULTI_A1324LUA_T)

SEN's value is a bit mask in decimal. Refer to the source code.

```
const adc_channel_t ANALOG_IN[AINS] = {
    channel_AN2, channel_AN1, channel_AN0,
    channel_AN7
};

SET:1 --> analog_in_flag[0] = 1;
SET:2 --> analog_in_flag[1] = 1;
SET:4 --> analog_in_flag[2] = 1;
SET:8 --> analog_in_flag[3] = 1;

SET:3 corresponds to "SET:1 and SET:2".
```

## EEPROM area (PIC16F1829's built-in EEPROM)

|Address|Usage                 |Node                  |
|-------|----------------------|----------------------|
|0      |Device ID             |                      |
|1      |Device setting        |                      |
|2~30   |Schedule              |Backplane-master only |

## Scheduler

* 0: every 8msec
* 1: every 16 msec (8 * 2)
* 2: every 48 msec (8 * 6): ~50msec
* 3: every 96 msec (8 * 12): ~100msec
* 4: every 480 msec (8 * 60): ~500msec
* 5: every 960 msec (8 * 120): ~1sec
* 6: every 4800 msec (8 * 600): ~5sec

## I2C address registry

|Address |Device ID              |Capability                      |FOSC  |UART baud rate|
|--------|-----------------------|--------------------------------|------|--------------|
|0x01(01)|BACKPLANE-MASTER       |I2C master (w/ 8msec timer)     |32MHz |115200        |
|0x10(16)|AQM1602XA-RN-GBW       |Character LCD                   |4MHz  |9600          |
|0x11(17)|A1324LUA_T             |Hall sensor                     |4MHz  |9600          |
|0x12(18)|HDC1000                |Temperature and humidity sensor |4MHz  |9600          |
|0x13(19)|KXR94-2050             |Three-axis accelerometer        |32MHz |9600          |
|0x14(20)|SHT31-DIS              |Temperature and humidity sensor |4MHz  |9600          |
|0x11(21)|MULTI_A1324LUA_T       |Hall sensor for position sensing|4MHz  |9600          |

Note: TI does not recommed using HDC1000 in a new design (http://www.ti.com/product/HDC1000/description), so I use SHT31-DIS from now on.
