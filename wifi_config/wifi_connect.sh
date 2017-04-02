#!/bin/bash
while true;do
ping -c 1 192.168.0.1 >/dev/null 2>&1
online=$?
if [ $online -eq 0 ]; then
    echo "Online"
else
        echo "Offline"
        connmanctl enable wifi > /dev/null 2>&1
        connmanctl scan wifi > /dev/null 2>&1
        ssid=`connmanctl services | grep $1 | tail -1 | grep -oE '[^ ]+$'`
        #echo $ssid

        #for i in $(echo $ssid | sed "s/_/ /g")
        #do
        # call your procedure/other scripts here below
        #       echo "$i"
        #done

        echo "[service_$ssid]" > /var/lib/connman/$1-psk.config
        echo "Type = wifi" >> /var/lib/connman/$1-psk.config
        echo "Name = $1" >> /var/lib/connman/$1-psk.config
        echo "Passphrase = $2" >> /var/lib/connman/$1-psk.config
	sync
	sync
	sync
        sleep 30
fi
sleep 30
done

