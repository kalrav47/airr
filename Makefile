all:
	arm-linux-gnueabi-gcc thread.c -I/home/kalrav/wiringPi/wiringPi/ -w -L/home/kalrav/Desktop/usr/lib -lpthread -lwiringPi
