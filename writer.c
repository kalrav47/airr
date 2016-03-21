//sudo apt-get install libcurl4-gnutls-dev
// gcc upgrade.c -lcurl
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<arpa/inet.h> 

void main()
{
	int sockfd = 0, n = 0;
	char recvBuff[1024];
	struct sockaddr_in serv_addr; 
	char v_local[1024],v_net[1024];
	FILE *f_local,*f_net,*fp;
	char *url = "http://airr.co.in/hello.php";
	char url2[1024]="\0";
	while(1){	
			system("rm -rf l_version.txt");
			system("curl --header 'Host: www.airr.co.in' 'http://181.224.157.140/hello.php' > l_version.txt");
			
			if(!(f_net= fopen("l_version.txt","r")))
			{
			printf("File couldnt open\n");
			fclose(f_net);
			exit(0);
			}
			else
			{
				fscanf(f_net,"%s",v_net);
				printf("%s\n",v_net);
				fclose(f_net);
				if(strcmp(v_net,"")==0)
				{
					system("rm -rf l_version.txt");
					printf("No command found\n");	
					exit(0);
				}
				if(strlen(v_net) != 11)
				{
					system("rm -rf l_version.txt");
					printf("No proper formated command found\n");	
					exit(0);
				}
				
				memset(recvBuff, '0',sizeof(recvBuff));
				if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
				{
					system("rm -rf l_version.txt");
					printf("\n Error : Could not create socket \n");
					return 1;
				}

				memset(&serv_addr, '0', sizeof(serv_addr)); 

				serv_addr.sin_family = AF_INET;
				serv_addr.sin_port = htons(5000);

				if(inet_pton(AF_INET, "192.168.1.2", &serv_addr.sin_addr)<=0)
				{
					system("rm -rf l_version.txt");
					printf("\n inet_pton error occured\n");
					return 1;
				} 

				if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
				{
					system("rm -rf l_version.txt");
					printf("\n Error : Connect Failed \n");
					return 1;
				} 



				write(sockfd,v_net,strlen(v_net)+1);

				char datar[256];
				usleep(1000);   
				read(sockfd,datar,256);
				printf("--> %s\n",datar);

				sprintf(url2,"curl http://www.airr.co.in/response.php?response_room=%s",datar);		

				system(url2);		

				usleep(100);

				write(sockfd,"RCVQUITMESG",12);

				close(sockfd);
				system("curl --header 'Host: www.airr.co.in' 'http://181.224.157.140/clear.php'");
				system("rm -rf l_version.txt");
			}
	}	
}
