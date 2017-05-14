#!/bin/bash

#declare -a input=(100 102 106 108 110 114 116 118 120 123 34 99 101 103 129)
#declare -a output=(35 49 131 133 135 137 139 195 196 50 132 107 109 111 48 47)                   

output=( 35 49 131 133 135 137 139 195 196 50 107 109 111 48 47 132)

for i in "${output[@]}"
do
   echo "$i" > /sys/class/gpio/export
   echo out > /sys/class/gpio/gpio$i/direction
   echo 0 > /sys/class/gpio/gpio$i/value
done

sync
sync
sync

while true;do

	for i in "${output[@]}"
	do
		if [ -f /root/.$i ] && [ 0 == `cat /sys/class/gpio/gpio$i/value` ];then
			echo 1 > /sys/class/gpio/gpio$i/value
			sync
		elif [ ! -f /root/.$i ] && [ 1 == `cat /sys/class/gpio/gpio$i/value` ];then
			echo 0 > /sys/class/gpio/gpio$i/value
			sync
		fi
	done
	sleep 0.3;
done		

