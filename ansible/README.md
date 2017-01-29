# Ansible

I use Ansible for configuring RasPi and OpenWrt routers.

(I never use NetConf)

Once I developed an Ansible-like took [neutron-lan](https://github.com/araobp/neutron-lan), but I don't use it this time.

## SSH key generation

```
$ ssh-keygen -t rsa
$ ssh-copy-id pi@192.168.XXX.XXX
```

For OpenWrt to work with Ansible, refer to these pages:
- http://www.ojscurity.com/2014/12/manage-openwrt-devices-with-ansible.html
- http://www.gilesorr.com/blog/openwrt-ansible.html

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

TASK [add route to default gw] *************************************************
changed: [192.168.57.102]

TASK [install packages] ********************************************************
ok: [192.168.57.102] => (item=python-light)
ok: [192.168.57.102] => (item=python-pip)
ok: [192.168.57.102] => (item=ip)
ok: [192.168.57.102] => (item=openvswitch)
ok: [192.168.57.102] => (item=kmod-usb-serial)
ok: [192.168.57.102] => (item=kmod-usb-serial-ftdi)
ok: [192.168.57.102] => (item=mosquitto)
ok: [192.168.57.102] => (item=mosquitto-client)
ok: [192.168.57.102] => (item=libmosquitto)

PLAY RECAP *********************************************************************
192.168.57.102             : ok=3    changed=1    unreachable=0    failed=0   

```
