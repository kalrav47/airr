#!/bin/sh
file="/dev/sda"

echo 123 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio123/direction

while true;do

if [ -b "$file" ]
then
        rm -rf usbMount
        mkdir usbMount
        mount /dev/sda usbMount
        ssid=`cat usbMount/ssid`
        connect_wifi `cat usbMount/ssid` `cat usbMount/password`
        umount usbMount
        sync
        sync
        sync
        sleep 30
        ping -c 3 8.8.8.8 >/dev/null 2>&1
        online=$?
        if [ $online -eq 0 ]; then
                echo 1 > /sys/class/gpio/gpio123/value
                oldIP=`ifconfig | awk '/inet addr/{print substr($2,6)}' | tail -1`
		if [ "$oldIP" != "192.168.0.1" ] && [ "$oldIP" != "127.0.0.1" ]; then 
			echo 0 > /sys/class/gpio/gpio123/value
                	firstBlock=`echo $oldIP | cut -d '.' -f1`
                	secondBlock=`echo $oldIP | cut -d '.' -f2`
                	thirdBlock=`echo $oldIP | cut -d '.' -f3`
                	newIP="$firstBlock.$secondBlock.$thirdBlock.199"
                	gateWay="$firstBlock.$secondBlock.$thirdBlock.1"
                	echo "IPv4=$newIP/255.255.255.0/$gateWay" >> /var/lib/connman/$ssid-psk.config
			echo "IPv6=off" >> /var/lib/connman/$ssid-psk.config
			echo "Nameservers=8.8.8.8,8.8.4.4" >> /var/lib/connman/$ssid-psk.config
		fi
        else
                echo 0 > /sys/class/gpio/gpio123/value
        fi
fi                                                              
ping -c 3 8.8.8.8 >/dev/null 2>&1                                                             
online=$?                                                                                     
if [ $online -eq 0 ]; then                                                                    
        echo 1 > /sys/class/gpio/gpio123/value                                                
else                                                                                          
        echo 0 > /sys/class/gpio/gpio123/value        
fi                                                    
sleep 5                                               
done
