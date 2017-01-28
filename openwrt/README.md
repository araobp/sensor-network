# Working with OpenWrt

```
Escape character is '^]'.
 === IMPORTANT ============================
  Use 'passwd' to set your login password
  this will disable telnet and enable SSH
 ------------------------------------------


BusyBox v1.23.2 (2015-07-25 15:09:46 CEST) built-in shell (ash)

  _______                     ________        __
 |       |.-----.-----.-----.|  |  |  |.----.|  |_
 |   -   ||  _  |  -__|     ||  |  |  ||   _||   _|
 |_______||   __|_____|__|__||________||__|  |____|
          |__| W I R E L E S S   F R E E D O M
 -----------------------------------------------------
 CHAOS CALMER (15.05, r46767)
 -----------------------------------------------------
  * 1 1/2 oz Gin            Shake with a glassful
  * 1/4 oz Triple Sec       of broken ice and pour
  * 3/4 oz Lime Juice       unstrained into a goblet.
  * 1 1/2 oz Orange Juice
  * 1 tsp. Grenadine Syrup
 -----------------------------------------------------
```

## My home LAN

|router  |WAN port ip address|
|--------|-------------------|
|openwrt1|192.168.57.101|
|openwrt2|192.168.57.102|
|openwrt3|192.168.57.103|

LAN port (br-lan): 192.168.1.1

There are two ways to connect to the routers from my PC:

From RasPi that is directly connected to one of the router's LAN ports:
```
$ telnet 192.168.1.1
```

From SSH via the WAN port:
```
$ ssh root@192.168.57.102
```

## Adding default GW

```
root@OpenWrt:~# route add default gw 192.168.57.1
root@OpenWrt:~# route
Kernel IP routing table
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
default         192.168.57.1    0.0.0.0         UG    0      0        0 eth0.2
192.168.1.0     *               255.255.255.0   U     0      0        0 br-lan
192.168.57.0    *               255.255.255.0   U     0      0        0 eth0.2
```
## Installing USB serial drivers

```
root@OpenWrt:/# opkg install kmod-usb-serial
root@OpenWrt:/# opkg install kmod-usb-serial-ftdi
root@OpenWrt:/# ls /dev/ttyUSB*
/dev/ttyUSB0
```

## Installing iproute2
```
root@OpenWrt:/# opkg instal ip
```

## Installing Open vSwitch
```
root@OpenWrt:/# opkg install openvswitch
```
```
root@OpenWrt:/# lsmod | grep openvswitch
gre                     3136  2 openvswitch
libcrc32c                480  1 openvswitch
openvswitch            70704  0
```
