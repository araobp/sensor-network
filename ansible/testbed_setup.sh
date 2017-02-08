#!/bin/bash
# 
# run this script in this directory.

ansible-playbook -i inventory/hosts playbooks/raspi_setup.yaml
ansible-playbook -i inventory/hosts playbooks/openwrt_setup.yaml
