#!/bin/bash
# 
# run this script in this directory.

ansible-playbook -i inventory/hosts playbooks/openwrt_setup.yaml
