file="/dev/sda"

while true;do

if [ -b "$file" ]
then
        rm -rf usbMount
        mkdir usbMount
        mount /dev/sda usbMount
        connect_wifi `cat usbMount/ssid` `cat usbMount/password`
        umount usbMount
        sleep 90
else
        echo "$file not found."
fi
sleep 5
done

