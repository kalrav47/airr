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
	
