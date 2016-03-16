all:
	arm-linux-gnueabi-gcc thread.c -I/home/kalrav/wiringPi/wiringPi/ -w -L/home/kalrav/Desktop/usr/lib -lpthread -lwiringPi
	arm-linux-gnueabi-gcc writer.c -o writer -w
clean:
	rm -rf a.out
	rm -rf writer
