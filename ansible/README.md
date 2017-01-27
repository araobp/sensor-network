#### SSH key generation

```
$ ssh-keygen -t rsa
$ ssh-copy-id pi@192.168.XXX.XXX
```

#### Inventory

The file name is "hosts":
```
[raspi]
192.168.XXX.XXX ansible_user=pi
```

#### Ping test
```
$ ansible raspi -i hosts -m ping
192.168.XXX.XXX | SUCCESS => {
    "changed": false, 
    "ping": "pong"
}
```
