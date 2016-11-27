dpkg -i --force-all dnsmasq_2.62-3+deb7u3_all.deb
cp access_point.conf /etc/dnsmasq.d/access_point.conf
cp interfaces /etc/network/interfaces
sudo ifup wlan1
sudo /etc/init.d/dnsmasq restart
ssid=`echo $1 | sha256sum | base64 | head -c 8`
password=`echo $1 | sha384sum | base64 | tail -c 61 | head -c 62`
sed -i "3s/airr/"${ssid}"/" hostapd.conf
sed -i "9s/123456789/"${password}"/" hostapd.conf
cp hostapd.conf /etc/hostapd.conf
cp startapmode.sh /root/
chmod +x /root/startapmode.sh
crontab -l > mycron
echo "@reboot /root/startapmode.sh" >> mycron
crontab mycron
rm mycron
