nmcli c add type wifi con-name airr ifname wlan0 ssid $1 
nmcli con modify airr wifi-sec.key-mgmt wpa-psk
nmcli con modify airr wifi-sec.psk $2
nmcli con up airr
