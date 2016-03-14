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

#include "gpio.h"

pthread_t pt1,pt2,pt3;
pthread_mutex_t lock;

int fd, c;
//GPIO_USER_DATA gpio_data;


int flag=0;

void *fun1();
void *fun2();
void *fun3();


FILE *fpp;
 char id[11];
int sockfd = 0, n = 0,i;
char recvBuff[1024];
struct sockaddr_in serv_addr; 

char ip[15];

int ERR=0;

/* Set direction IN or OUT for GPIO */
int gpio_set_direction(int fd, GPIO_USER_DATA *gpio_data)
{
	int ret;

//	printf("%s\n", __func__);

	ret = ioctl(fd, GPIO_SET_DIRECTION, gpio_data);
        if (ret < 0)
                printf("GPIO_SET_VALUE ioctl failed, %s\n", __func__);
}

/* Set value for GPIO */
int gpio_set_value(int fd, GPIO_USER_DATA *gpio_data)
{
	int ret;

//	printf("%s\n", __func__);

	ret = ioctl(fd, GPIO_SET_VALUE, gpio_data);
	if (ret < 0)
                printf("GPIO_SET_VALUE ioctl failed, %s\n", __func__);
}

/* Get GPIO value */
int gpio_get_value(int fd, GPIO_USER_DATA *gpio_data)
{
	int ret;

        ret = ioctl(fd, GPIO_GET_VALUE, gpio_data);
        if (ret < 0)
                printf("GPIO_GET_VALUE ioctl failed, %s\n", __func__);
}



int main(int argc,char *argv[])
{
	
	//strcpy(ip,argv[1]);	

	pthread_create(&pt1,NULL,fun1,NULL);
	pthread_create(&pt2,NULL,fun2,NULL);	
	pthread_create(&pt3,NULL,fun3,NULL);

	pthread_exit(NULL);

	return 0;
}


void *fun1()
{

	fpp=fopen("id.txt","r");
	fscanf(fpp,"%s",id);
	printf("id - %s\n",id); 

	
	memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 
	
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000); 

    if(inet_pton(AF_INET,"192.168.1.123", &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 
	while(1) {
	
    read(sockfd, recvBuff,50);
	
	  if(strcmp(recvBuff,"") != 0)
		{
	 		
			if(strstr(recvBuff,id) != NULL)
				{
					pthread_mutex_lock(&lock);
					for(i=0;i<=strlen(recvBuff);i++)
						recvBuff[i]=recvBuff[i+10];
					if(flag==1) flag=0;
					else if(flag==0) flag=1;
					pthread_mutex_unlock(&lock);
					printf("flag =  %d\n",flag);
				}
	fflush(stdout);
  recvBuff[0]='\0';
        }
     
}
    if(n < 0)
    {
        printf("\n Read error \n");
    } 

 

}


void *fun2()
{
	fd = open("/dev/bcm-gpio", O_RDWR);
	/* memset the structures */
	GPIO_USER_DATA gpio_data;

	memset(&gpio_data, 0, sizeof(gpio_data));

	while(1)
	{	
		pthread_mutex_lock(&lock);
		gpio_data.gpio_num=5;
		gpio_data.gpio_direction = 1;
		gpio_data.gpio_value = 1;	
		gpio_get_value(fd, &gpio_data);
		printf("The output value of GPIO %u = %u\n", gpio_data.gpio_num,gpio_data.gpio_value);
		usleep(10000);
	
		if(gpio_data.gpio_value==1)
		{
			if(flag==1) flag=0;
			else if(flag==0)  flag=1;
			printf("flag =  %d\n",flag);
			 usleep(10000);

		
		}
		pthread_mutex_unlock(&lock);
		sleep(1);
	}
	close(fd);

}

void *fun3()
{
	
	 GPIO_USER_DATA gpio_data;
	 memset(&gpio_data, 0, sizeof(gpio_data));

	 gpio_data.gpio_num=20;
         gpio_data.gpio_direction = 1;

	while(1)
	{
		if(flag==1)
		{
		//	printf("inside on\n");
			pthread_mutex_lock(&lock);
			printf("inside on\n");
		//	gpio_data.gpio_num=19;
		//	gpio_data.gpio_direction = 1;
			gpio_data.gpio_value = 1;	
			gpio_set_value(fd, &gpio_data);
			pthread_mutex_unlock(&lock);
		}
		else
		{
		//	printf("inside off\n");
			pthread_mutex_lock(&lock);
			printf("inside off\n");

		//	gpio_data.gpio_num=19;
		//	gpio_data.gpio_direction = 1;
			gpio_data.gpio_value = 0;	
			gpio_set_value(fd, &gpio_data);
			pthread_mutex_unlock(&lock);	
		}
	}
}
	




