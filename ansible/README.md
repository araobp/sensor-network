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

## Inventory

The file name is "hosts":
```
[raspi]
192.168.XXX.XXX ansible_user=pi
```

## Ping test
```
$ ansible raspi -i hosts -m ping
192.168.XXX.XXX | SUCCESS => {
    "changed": false,
    "ping": "pong"
}
```
