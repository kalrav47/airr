dpkg -i --force-all dnsmasq_2.62-3+deb7u3_all.deb
cp access_point.conf /etc/dnsmasq.d/access_point.conf
cp interfaces /etc/network/interfaces
sudo ifup wlan1
sudo /etc/init.d/dnsmasq restart
cp hostapd.conf /etc/hostapd.conf
cp hostapd-systemd.service /lib/systemd/system/hostapd-systemd.service
sudo update-rc.d hostapd disable
sudo systemctl daemon-reload
sudo systemctl enable hostapd-systemd
crontab -l > mycron
echo "@reboot sudo systemctl start hostapd-systemd" >> mycron
crontab mycron
rm mycron