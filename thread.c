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
//#include <wiringPi.h>
#include<time.h>

struct val{
	int  fd;
	int pin;
	int ld;
	time_t start;
	long int usage;
};


struct val *values_read[9],*values_write[9];
pthread_t pt1,pt2,pt3,pt4,pt_gpio_read[8],pt_gpio_write[8];
pthread_mutex_t lock;

int fd, c;
int flag[9]={0,};
int us[8]={0,};
struct sockaddr_in serv_addr;
int gpio_pins_read[9]={18,6,13,19,26,12,16,20,21};
int gpio_pins_write[9]={23,3,4,17,8,22,10,9,5};
void *fun1();
void *fun2();
void *fun3();
void *fun4();
void *gpio_read(struct val *);
void *gpio_write(struct val *);

void incase_of_sig()
{
	FILE *f_stat;
	//digitalWrite(27,LOW);
	f_stat=fopen("stat.txt","w");
	fprintf(f_stat,"%d %d %d %d %d %d %d %d %d",flag[0],flag[1],flag[2],flag[3],flag[4],flag[5],flag[6],flag[7],flag[8]);
	fclose(f_stat);
	exit(0);
}


int main(int argc,char *argv[])
{
	int i=0;
	FILE *f_stat;
	signal(SIGSEGV,incase_of_sig);
	signal(SIGINT,incase_of_sig);
	signal(SIGPIPE,incase_of_sig);
	//wiringPiSetupGpio();

	f_stat=fopen("stat.txt","r");
	fscanf(f_stat,"%d %d %d %d %d %d %d %d %d",&flag[0],&flag[1],&flag[2],&flag[3],&flag[4],&flag[5],&flag[6],&flag[7],&flag[8]);
	fclose(f_stat);

	/*for(i=0;i<8;i++)
	{
		pinMode(gpio_pins_write[i], OUTPUT);  
    		pinMode(gpio_pins_read[i], INPUT);
	}*/

	pthread_create(&pt1,NULL,fun1,NULL);
	pthread_create(&pt2,NULL,fun2,NULL);
	pthread_create(&pt3,NULL,fun3,NULL);
	pthread_create(&pt4,NULL,fun4,NULL);
	pthread_exit(NULL);
	return 0;
}

void *fun1()
{
	char id[12],recv[4096],data[20];
	int ld=0,sockfd;
	FILE *f_id;
	f_id=fopen("id.txt","r");
	fscanf(f_id,"%s",id);
	fclose(f_id);

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Error : Could not create socket \n");
		incase_of_sig();
	} 

	memset(&serv_addr, '0', sizeof(serv_addr)); 
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(5000); 

	if(inet_pton(AF_INET,"192.168.6.107", &serv_addr.sin_addr)<=0)
	{
		printf("\n inet_pton error occured\n");
		incase_of_sig();
	}

	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
	       printf("\n Error : Connect Failed \n");
	       incase_of_sig();
	} 
	usleep(100);
	write(sockfd,"SWITCHBOARD",12);
	read(sockfd,recv,12);
	write(sockfd,id,11);
	read(sockfd,recv,12);

	usleep(300);
	while(1)
	{
		if(read(sockfd,recv,4096)>0)
		{
			if(strstr(recv,id) != NULL && strlen(recv)==11 )
			{
				if(recv[10]=='R')
				{
					sprintf(data,"%s%d%d%d%d%d%d%d%d%d",id,flag[0],flag[1],flag[2],flag[3],flag[4],flag[5],flag[6],flag[7],flag[8]);
					write(sockfd,data,22);
					continue;
				}
				else
				{
					pthread_mutex_lock(&lock);
					if(!isdigit(recv[10]))
					{
						write(sockfd,"ERROROCURED",12);
						pthread_mutex_unlock(&lock);
						continue;
					}
					ld=recv[10]-48;
					if(flag[ld]==1)
					{
						flag[ld]=0;
						write(sockfd,"GOTYOURMESG",12);
					}
					else if(flag[ld]==0)
					{
						flag[ld]=1;
						write(sockfd,"GOTYOURMESG",12);
					}
					pthread_mutex_unlock(&lock);
				}
			}
		}
	}
}


void *fun2()
{
	int i;
	
	for(i=0;i<9;i++)
        {
                values_read[i]=(struct val* )malloc(sizeof(struct val));
                values_write[i]=(struct val* )malloc(sizeof(struct val));      
        }

        for(i=0;i<9;i++)
        {
                insert_values(values_read[i],fd,gpio_pins_read[i],i);
                insert_values(values_write[i],fd,gpio_pins_write[i],i);
	}
	
	/*for(i=0;i<9;i++)
	{
		pthread_create(&pt_gpio_read[i],NULL,gpio_read,values_read[i]);
		pthread_create(&pt_gpio_write[i],NULL,gpio_write,values_write[i]);
	}
	

	pthread_exit(NULL);
	
	close(fd);*/
}


void insert_values(struct val *ptr,int fds,int pins,int lds)
{
	ptr->fd=fds;
	ptr->pin=pins;
	ptr->ld=lds;
}

/*void *gpio_read(struct val *temp)
{
	while(1)
	{
		pthread_mutex_lock(&lock);
       		if(digitalRead(temp->pin))
                {
                        if(flag[temp->ld]==1)
			{
				flag[temp->ld]=0;
				sleep(3);
			}
                        else if(flag[temp->ld]==0)
			{
				flag[temp->ld]=1;
				sleep(3);
			}
		}
                pthread_mutex_unlock(&lock);
	}
}*/

/*void *gpio_write(struct val *temp)
{
	while(1)
	{
		if(flag[temp->ld]==1)
        	{
                	pthread_mutex_lock(&lock);
               		digitalWrite(temp->pin, HIGH);
			temp->start=time(NULL);
                        pthread_mutex_unlock(&lock);
			sleep(3);
                }
                else
               	{
        		pthread_mutex_lock(&lock);
               		digitalWrite(temp->pin,LOW);
			temp->usage += time(NULL) - start ; 
                        pthread_mutex_unlock(&lock);
			sleep(3);    
              	}
	}
}*/



void *fun3()
{
	FILE *f_stat;
	while(1)
	{
		pthread_mutex_lock(&lock);			
		f_stat=fopen("stat.txt","w");
		fprintf(f_stat,"%d %d %d %d %d %d %d %d %d",flag[0],flag[1],flag[2],flag[3],flag[4],flag[5],flag[6],flag[7],flag[8]);
        	fclose(f_stat);
		pthread_mutex_unlock(&lock);
		sleep(3);
	}
}

void *fun4()
{
	char rcbuff[100]="";
	int sockfdd;
	while(1)
	{
		if((sockfdd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			printf("\n Error : Could not create socket \n");
			incase_of_sig();
	    	} 
		
		struct timeval tv;	
		tv.tv_sec=2;

		if(setsockopt(sockfdd,SOL_SOCKET,SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval)))
		{
			incase_of_sig();
		}

	    	memset(&serv_addr, '0', sizeof(serv_addr)); 

	    	serv_addr.sin_family = AF_INET;
	    	serv_addr.sin_port = htons(5000); 

	  	if(inet_pton(AF_INET,"192.168.6.107", &serv_addr.sin_addr)<=0)
	    	{
	       		printf("\n inet_pton error occured\n");
			incase_of_sig();
	    	}

	   	if( connect(sockfdd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	    	{
	       		printf("\n Error : Connect Failed \n");
	       		incase_of_sig();
	   	}
	
		//pinMode(27,OUTPUT);
		//digitalWrite(27,HIGH); 
	
		if(write(sockfdd,"HEARTBEATCK",12) <= 0 )
		{
			incase_of_sig();
		}

		while(1){
		if(read(sockfdd,rcbuff,100)>0)
		{
			//digitalWrite(27,HIGH);
			break;
		}
		else
		{
			//digitalWrite(27,LOW);
			incase_of_sig();
	  	}}
		close(sockfdd);
		sleep(1);	
	}

}

















