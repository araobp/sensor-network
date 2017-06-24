# Ansible

I am examining if Ansible is useful for configuring RasPi, OpenWrt routers and IoT devices.

(I never use NetConf)

Once I developed an Ansible-like took [neutron-lan](https://github.com/araobp/neutron-lan), but I don't use it this time.

## Testbed LAN

![lan](https://docs.google.com/drawings/d/1zNuTBCUMr9O2U8A4oZ5Yg9mgKPmDFFEwM2ZbsiDVOCs/pub?w=960&h=720)

## Ansible modules for OpenWrt

- [opkg](http://docs.ansible.com/ansible/opkg_module.html)
- [command](http://docs.ansible.com/ansible/command_module.html)

## SSH key generation

The normal key generation process is as follows:
```
$ ssh-keygen -t rsa
$ ssh-copy-id root@192.168.57.101
```

But it does not work with OpenWrt. Refer to the following pages:
- http://www.ojscurity.com/2014/12/manage-openwrt-devices-with-ansible.html
- http://www.gilesorr.com/blog/openwrt-ansible.html


So, for OpenWrt, take the following steps instead:
```
$ ssh-keygen -t rsa
$ cat .ssh/id_rsa.pub | ssh root@192.168.57.101 'cat > /etc/dropbear/authorized_keys && chmod 600 /etc/dropbear/authorized_keys'
```

## Directory structure for Ansible

I initiate ansible-playbook in this folder to configure the routers.

- [ansinle.cfg](./ansible.cfg)
- [inventory](./inventory)
- [playbooks](./playbooks)
- [shell script for setting up testbed LAN](./testbed_setup.sh)


## Ping test

Ansible supports ping test:
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

## Sample log

The below is the output of ./openwrt_setup.sh:

```

   Ansible  ----------------------> SSH server
 [Debian Linux]                 [OpenWrt         ]
 [VirtualBox  ]                 [Buffalo BHR-4GRV]
```

=> [sample log](./20170204.md)

## Next step

Modify the firmware for IoT blocks (PIC16F1 8-bit MCU) to support full config automation via UART in a plug&play manner with Ansible.


