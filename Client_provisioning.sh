# configure wifi
ssid=`echo $1 | sha256sum | base64 | head -c 16`
password=`echo $1 | sha384sum | base64 | tail -c 61 | head -c 58`
cd wifi_config
cp startwificonnections.sh /root/
chmod +x /root/startwificonnections.sh
crontab -l > mycron
echo "@reboot /root/startwificonnections.sh "${ssid}" "${password}"" >> mycron
crontab mycron
rm mycron
cd ../

sync
sync
sync

#disable uart
systemctl mask serial-getty@ttyS0.service
systemctl stop serial-getty@ttyS0.service
systemctl disable serial-getty@ttyS0.service

sync
sync
sync

#disable ssh
apt-get purge openssh-server

sync
sync
sync

#disable login from screen
systemctl disable serial-getty@ttyGS0.service
sync
sync
sync

#disable bluetooth
systemctl disable bluetooth.service

sync
sync
sync

#change root password
root_password=`cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1`
echo "root:"${root_password}"" | chgpasswd

sync
sync
sync

