# configure wifi
ssid=`echo $1 | sha256sum | base64 | head -c 30`
password=`echo $1 | sha384sum | base64 | tail -c 61 | head -c 58`
cp startwificonnections.sh /root/
chmod +x /root/startwificonnections.sh
crontab -l > mycron
echo "@reboot /root/startwificonnections.sh "${ssid}" "${password}"" >> mycron
crontab mycron
rm mycron

#disable uart
systemctl mask serial-getty@ttyS0.service
systemctl stop serial-getty@ttyS0.service
systemctl disable serial-getty@ttyS0.service

#disable ssh
apt-get purge openssh-server

#disable login from screen
systemctl disable serial-getty@ttyGS0.service

#disable bluetooth
systemctl disable bluetooth.service

#change root password
root_password=`cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1`
echo "root:"${root_password}"" | chgpasswd
