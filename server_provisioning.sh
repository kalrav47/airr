#install ap mode configuration
cd ap_config
chmod +x install_ap_conf.sh
sh install_ap_conf.sh $1
cd ../

sync
sync
sync

#copy files
cd executables
cp server ~/
chmod +x ~/server
cp server.sh ~/
chmod +x ~/server.sh
cd ../
crontab -l > mycron
echo "@reboot /root/server.sh" >> mycron
crontab mycron
rm mycron

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

