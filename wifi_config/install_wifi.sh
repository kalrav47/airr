cp interfaces /etc/network/interfaces

ssid=`echo $1 | sha256sum | base64 | head -c 8`
password=`echo $1 | sha384sum | base64 | tail -c 61 | head -c 12`

sed -i "6s/airr/"${ssid}"/" wpa_supplicant.conf
sed -i "7s/123456789/"${password}"/" wpa_supplicant.conf

cp wpa_supplicant.conf /etc/wpa_supplicant/wpa_supplicant.conf
sync
sync