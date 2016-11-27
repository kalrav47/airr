# configure wifi
cd wifi_config
chmod +x install_wifi.sh
sh install_wifi.sh $1
cd ../

sync
sync
sync

#copy files
cd executables
cp export_gpio.sh ~/
chmod +x ~/export_gpio.sh
cp client ~/
chmod +x ~/client
cp client.sh ~/
chmod +x ~/client.sh
cd ../
touch ~/stat.txt
touch ~/usage.txt

crontab -l > mycron
echo "@reboot /root/export_gpio.sh" >> mycron
echo "@reboot /root/client.sh" >> mycron
crontab mycron
rm mycron

sync
sync
sync

#create id file
echo $2 | md5sum | grep -o '[[:digit:]]' | head -n 10 | tr -d '\n'> id

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

