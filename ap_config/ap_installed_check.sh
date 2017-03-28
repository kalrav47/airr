ssid=`echo $2 | sha256sum | base64 | head -c 8`
password=`echo $2 | sha384sum | base64 | tail -c 61 | head -c 62`

if [ -f "/root/.ap_installed" ]
then
	cp etc/airr/apmode/startapmode.sh /root/
	rm -rf etc/airr/apmode/
	touch /root/.ap_installed
else
	sh etc/airr/apmode/install_ap_conf.sh $ssid $password
fi
