#!/bin/bash
file="/dev/sda"

echo 123 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio123/direction

while true;do

if [ -b "$file" ]
then
        rm -rf usbMount
        mkdir usbMount
        mount /dev/sda usbMount
        connect_wifi `cat usbMount/ssid` `cat usbMount/password`
        umount usbMount
	sync
	sync
	sync
	sleep 30
	ping -c 1 8.8.8.8 >/dev/null 2>&1
	online=$?
	if [ $online -eq 0 ]; then
    		echo 1 > /sys/class/gpio/gpio123/value
	else
		echo 0 > /sys/class/gpio/gpio123/value
	fi
	sleep 30
fi
ping -c 1 8.8.8.8 >/dev/null 2>&1
online=$?
if [ $online -eq 0 ]; then
    	echo 1 > /sys/class/gpio/gpio123/value
else
	echo 0 > /sys/class/gpio/gpio123/value
fi
sleep 5
done

