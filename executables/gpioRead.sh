#!/bin/bash

set -m

input=( 100 102 106 108 110 114 116 118 120 123 34 99 101 103 129 )
output=( 35 49 131 133 135 137 139 195 196 50 107 109 111 48 47 132)

function gpioRead {
while true;do
		if [ 1 == `cat /sys/class/gpio/gpio$1/value` ];then
			if [ -f /root/.$2 ];then
				rm /root/.$2;
				sleep 1;
				sync
			elif [ ! -f /root/.$2 ];then
				touch /root/.$2;
				sleep 1;
				sync
			fi
		fi
		sleep 0.3
done
}

for i in "${input[@]}"
do
   echo "$i" > /sys/class/gpio/export
   echo in > /sys/class/gpio/gpio"$i"/direction 
done

sync
sync
sync

for i in ${!input[*]};
do
	gpioRead ${input[$i]} ${output[$i]} &
done

while [ 1 ];do sleep 300; 2> /dev/null; [ $? == 1 ] && break; done
