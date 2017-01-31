# Working with OpenWrt

Four years ago I started using OpenWrt to develop very cheap "whitebox" in the SDN world. Cumulus is nice but OpenWrt is free and really cheap.

This time I use OpenWrt as OS for IoT gateways.

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

192.168.56.0/24 is used by VirtualBox, so I assign 192.168.57.0/24 range to WAN ports of my routers:

|router  |WAN port ip address|
|--------|-------------------|
|openwrt1|192.168.57.101|
|openwrt2|192.168.57.102|
|openwrt3|192.168.57.103|

LAN port (br-lan): 192.168.1.1

Buffalo BHR-4GRV
```
   H/W switch
   [ ] LAN port
   [ ] LAN port
   [ ] LAN port
   [ ] LAN port
   [ ] WAN port --> connected to Home Gateway
```

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

Things connected to the Internet via my home LAN:
```
                                                192.168.57.1/24             (          )
[Blocks]---USB---[OpenWrt]---[L2 switch with WiFi]---[Home Gateway]--------(The Internet)
                      |                    |                                (          )
          PCs/smartphones/tablets  TV/VideoRecorder/AppleTV
```

Add default GW:
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

I have installed drivers for FTDI's USB-UART bridge.

```
root@OpenWrt:/# opkg install kmod-usb-serial
root@OpenWrt:/# opkg install kmod-usb-serial-ftdi
root@OpenWrt:/# ls /dev/ttyUSB*
/dev/ttyUSB0
```

## Installing iproute2

iproutes is must-have.

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

## Working with Ansible and UCI (I never use NetConf)

A combination of Ansible and UCI is much easier to use than those products provided by tail-f (NetConf): ==> [ansible](https://github.com/araobp/blocks/ansible)

## Building OpenWrt

https://wiki.openwrt.org/doc/howto/buildroot.exigence

## vCPE with USB over IP

My goal is to use OpenWrt as IoT gateway, so I want to make this experiment:

Pattern 1:
```
[USB device]----USB----[OpenWrt]----USB over IP----[Device driver on Windows10]
```

Pattern 2:
```
[USB device]----USB----[OpenWrt]----USB over IP----[vCPE on NFV infrastracture]
```

Refer to these pages:
- http://usbip.sourceforge.net/
- https://wiki.openwrt.org/doc/howto/usb.iptunnel

The "vCPE" approach seems clever than managing device drivers on remote routers (remote CPEs), but I am concerned with its latency.


