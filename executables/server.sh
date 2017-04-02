#!/bin/bash
while true;do
R=`ifconfig | head -10 | tail -1 | grep tether`
if [ $? -eq 0 ]; then
        break;
else
        connmanctl enable wifi
	sleep 2
        connmanctl tether wifi on airr 12345678
        sleep 2
fi
sleep 3
done

touch /tmp/apStarted

startServer &
startBridgeServer &
awsConnect &
