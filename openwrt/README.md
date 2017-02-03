# Working with OpenWrt

Four years ago I started using OpenWrt to develop very cheap "whitebox" in the SDN world. Cumulus is nice but OpenWrt is free and really cheap.

This time I use OpenWrt as OS for IoT gateways.

``` Escape character is '^]'.  === IMPORTANT ============================
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

## Building OpenWrt and installing it onto my router Buffalo BHR-4GRV

First, refer to this page to build OpenWrt: https://wiki.openwrt.org/doc/howto/buildroot.exigence

I have built OpenWrt with kernale namespaces enabled ([.config](./config)). 

I find the image under ~/openwrt/bin/ar71xx:

```
arao@debian:~/openwrt/bin/ar71xx$ ls
md5sums
openwrt-ar71xx-generic-nbg460n_550n_550nh-u-boot.bin
openwrt-ar71xx-generic-root.squashfs
openwrt-ar71xx-generic-root.squashfs-64k
openwrt-ar71xx-generic-uImage-gzip.bin
openwrt-ar71xx-generic-uImage-lzma.bin
openwrt-ar71xx-generic-vmlinux.bin
openwrt-ar71xx-generic-vmlinux.elf
openwrt-ar71xx-generic-vmlinux.gz
openwrt-ar71xx-generic-vmlinux.lzma
openwrt-ar71xx-generic-vmlinux-lzma.elf
openwrt-ar71xx-generic-wzr-hp-g450h-squashfs-factory.bin
openwrt-ar71xx-generic-wzr-hp-g450h-squashfs-sysupgrade.bin
openwrt-ar71xx-generic-wzr-hp-g450h-squashfs-tftp.bin
packages
sha256sums
uboot-ar71xx-nbg460n_550n_550nh
```

Now that the image is on Debian linux VM on VirtualBox and my DELL PC does not have Ethernet port, I need to scp the image to my Raspi:

```
[OpenWrt/BHR-4GRV]<--- tftp ---[RasPi]<--- scp ----[Debian/VirtualBox]
```

```
arao@debian:~/openwrt/bin/ar71xx$ scp openwrt-ar71xx-generic-wzr-hp-g450h-squashfs-tftp.bin pi@192.168.57.132:/tmp
```

Next, I register a route to the router:

```
pi@raspberrypi:~ $ sudo ip addr add 192.168.11.2/24 dev eth0
pi@raspberrypi:~ $ ip route
default via 192.168.57.1 dev wlan0 
default via 192.168.57.1 dev wlan0  metric 303 
192.168.11.0/24 dev eth0  proto kernel  scope link  src 192.168.11.2 
192.168.57.0/24 dev wlan0  proto kernel  scope link  src 192.168.57.132  metric 303 
```

Since the router use this IP/MAC address "192.168.11.1/02:AA:BB:CC:DD:22" to accept new image from tftp, I need to add a arp table manually:
```
pi@raspberrypi:~ $ sudo arp -s 192.168.11.1 02:AA:BB:CC:DD:22
pi@raspberrypi:~ $ arp -a
? (192.168.57.130) at 98:54:1b:07:e6:71 [ether] on wlan0
ntt.setup (192.168.57.1) at 00:3a:9d:89:0c:7e [ether] on wlan0
```

Then I use tftp to put the image to my router via tftp.


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


