#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<signal.h>
#include<pthread.h>
#include <wiringPi.h>
#include<time.h>

int gpio_pins_write[9]={23,3,4,17,8,22,10,9,5};

int main(int argc,char *argv[])
{
        int pinNumber = atoi(argv[1]);

		pinMode(gpio_pins_write[pinNumber], OUTPUT);
		digitalWrite(pinNumber, HIGH);
}