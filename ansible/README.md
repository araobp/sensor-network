# Ansible

I use Ansible for configuring RasPi and OpenWrt routers.

(I never use NetConf)

Once I developed an Ansible-like took [neutron-lan](https://github.com/araobp/neutron-lan), but I don't use it this time.

## SSH key generation

```
$ ssh-keygen -t rsa
$ ssh-copy-id root@192.168.XXX.XXX
```

For OpenWrt to work with Ansible, refer to these pages:
- http://www.ojscurity.com/2014/12/manage-openwrt-devices-with-ansible.html
- http://www.gilesorr.com/blog/openwrt-ansible.html

```
arao@debian:~$ cat .ssh/id_rsa.pub | ssh root@192.168.57.103 'cat > /etc/dropbear/authorized_keys && chmod 600 /etc/dropbear/authorized_keys'
```

## Directory structure for Ansible

- [.ansinle.cfg](./.ansible.cfg)
- [inventory](./inventory)
- [playbooks](./playbooks)
- [shell script for setting up OpenWrt routers](./openwrt_setup.sh)


## Ping test
```
$ ansible raspi -i hosts -m ping
192.168.XXX.XXX | SUCCESS => {
    "changed": false,
    "ping": "pong"
}
```

## OpenWrt setup

Since Ansible requires Python, the following minimum manual steps are required before using Ansible:

```
root@OpenWrt:~# route add default gw <ip address of default gw>
root@OpenWrt:~# opkg update
root@OpenWrt:~# opkg install python-light
```

The below is the output of ./openwrt_setup.sh:

```

   Ansible  ----------------------> SSH server
 [Debian Linux]                 [OpenWrt         ]
 [VirtualBox  ]                 [Buffalo BHR-4GRV]
```

```
arao@debian:~/blocks/ansible$ ./openwrt_setup.sh 

PLAY [setup OpenWrt] ***********************************************************

TASK [setup] *******************************************************************
ok: [192.168.57.102]

TASK [add route to default gw permanently] *************************************
changed: [192.168.57.102] => (item=uci set network.wan.gateway=192.168.57.1)
changed: [192.168.57.102] => (item=uci commit)
changed: [192.168.57.102] => (item=/etc/init.d/network reload)

TASK [install packages] ********************************************************
ok: [192.168.57.102] => (item=python-pip)
ok: [192.168.57.102] => (item=ip)
changed: [192.168.57.102] => (item=tcpdump)
ok: [192.168.57.102] => (item=openvswitch)
changed: [192.168.57.102] => (item=coreutils-stty)
ok: [192.168.57.102] => (item=kmod-usb-serial)
ok: [192.168.57.102] => (item=kmod-usb-serial-ftdi)
ok: [192.168.57.102] => (item=mosquitto)
ok: [192.168.57.102] => (item=mosquitto-client)
ok: [192.168.57.102] => (item=libmosquitto)

PLAY RECAP *********************************************************************
192.168.57.102             : ok=3    changed=2    unreachable=0    failed=0   

```

## Working with UCI

UCI is CLI for OpenWrt.

I use UCI to configure OpenWrt's networking features like this:
```
root@OpenWrt:~# uci show network.wan
network.wan=interface
network.wan.ifname='eth0.2'
network.wan.proto='static'
network.wan.ipaddr='192.168.57.102'
network.wan.netmask='255.255.255.0'
root@OpenWrt:~# uci set network.wan.gateway=192.168.57.1
root@OpenWrt:~# uci commit
root@OpenWrt:~# /etc/init.d/network reload 
```
