#install ap mode configuration
cd ap_config
chmod +x install_ap_conf.sh
sh install_ap_conf.sh $1
cd ../

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
