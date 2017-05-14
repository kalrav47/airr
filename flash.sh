#sudo ./flash.sh 0 31344D15E9

cd ../CHIP-SDK/CHIP-buildroot/output/images/
rm -rf tmp-rootfs
mkdir tmp-rootfs
cp rootfs.tar tmp-rootfs/
cd tmp-rootfs/
tar -xvf rootfs.tar
rm rootfs.tar
#cp /home/kalravparsana/airr/wifi_config/wpa_supplicant.conf etc/wpa_supplicant.conf
#cp /home/kalravparsana//airr/wifi_config/interfaces etc/network/

#cp /home/kalravparsana/airr/wifi_config/airr_init.sh usr/sbin/airr_init
#chmod +x usr/sbin/airr_init
#sed -i '13i	airr_init' etc/init.d/S40network

ssid=`echo $2 | sha256sum | base64 | head -c 8`
password=`echo $2 | sha384sum | base64 | tail -c 61 | head -c 62`

#change wifi config (stop power saving mode)
cp  /home/kalravparsana/airr/rtl8723bs_mp.conf etc/modprobe.d/rtl8723bs_mp.conf
chmod 644 etc/modprobe.d/rtl8723bs_mp.conf

if [ "$1" = "0" ]; then
	cp /home/kalravparsana/airr/executables/server usr/sbin/server
	chmod u+x usr/sbin/server
	cp /home/kalravparsana/airr/executables/bridgeServer usr/sbin/bridgeServer
	chmod u+x usr/sbin/bridgeServer
	cp /home/kalravparsana/airr/executables/server.sh usr/sbin/configAndStartServer
	chmod u+x usr/sbin/configAndStartServer

	sed -i 's/airr/'"$ssid"'/g' usr/sbin/configAndStartServer
	sed -i 's/12345678/'"$password"'/g' usr/sbin/configAndStartServer	

	cp /home/kalravparsana/airr/wifi_config/wifi_connect_server.sh usr/sbin/connect_wifi
	chmod +x usr/sbin/connect_wifi

	cp /home/kalravparsana/airr/executables/usbChecker.sh usr/sbin/usbChecker
	chmod u+x usr/sbin/usbChecker

	cp /home/kalravparsana/airr/executables/airrAWS usr/sbin/awsCommMgr
	chmod u+x usr/sbin/awsCommMgr

	cp /home/kalravparsana/airr/executables/timeChecker.sh usr/sbin/timeCheck
	chmod u+x usr/sbin/timeCheck
	
	cp /home/kalravparsana/airr/executables/inittab_server etc/inittab

	mkdir -p root/certs
	wget https://s3-ap-southeast-2.amazonaws.com/kalrav1/"$2".cert.pem -P root/certs
	wget https://s3-ap-southeast-2.amazonaws.com/kalrav1/"$2".private.key -P root/certs
	mv root/certs/"$2".cert.pem root/certs/certificate.pem.crt
	mv root/certs/"$2".private.key root/certs/private.pem.key
	cp /home/kalravparsana/airr/certs/root-CA.crt root/certs
	echo $2 > root/certs/clientID

		
else
	echo "client selected";
	sleep 2;

	cp /home/kalravparsana/airr/executables/inittab_client etc/inittab

	cp /home/kalravparsana/airr/wifi_config/wifi_connect.sh usr/sbin/connect_wifi
	chmod +x usr/sbin/connect_wifi
	echo "::respawn:/usr/sbin/connect_wifi $ssid $password" >> etc/inittab

	cp /home/kalravparsana/airr/executables/gpioRead.sh usr/sbin/gpioRead
	chmod +x usr/sbin/gpioRead
	cp /home/kalravparsana/airr/executables/gpioWrite.sh usr/sbin/gpioWrite
	chmod +x usr/sbin/gpioWrite

	cp /home/kalravparsana/airr/executables/switchBoard usr/sbin/client
	chmod +x usr/sbin/client
	
	touch root/stat.txt
	touch root/usage.txt
	echo "1234567890" > root/id.txt

fi


tar -cvf rootfs-tmp.tar .
rm ../rootfs-tmp.tar
mv rootfs-tmp.tar ../
cd /home/kalravparsana//CHIP-SDK/CHIP-tools/
rm -rf tmp-flash-bin
./chip-create-nand-images.sh ../CHIP-buildroot/output/build/uboot-production-mlc/ ../CHIP-buildroot/output/images/rootfs-tmp.tar tmp-flash-bin
./chip-flash-nand-images.sh tmp-flash-bin
