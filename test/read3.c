#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include<time.h>
int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0;
    char recvBuff[1024];
char msg[4096];
    struct sockaddr_in serv_addr; 

    if(argc != 3)
    {
        printf("\n Usage: %s <ip of server> <message>\n",argv[0]);
        return 1;
    } 
	printf("1.\n");

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 
int on = 1;
	int ret = setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
	printf("2.\n");
    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000); 

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 
printf("3.\n");
    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 
	//send(sockfd,argv[2],strlen(argv[2])+1,MSG_NOSIGNAL  );
	printf("4.\n");
    	
	write(sockfd,"SWITCHBOARD",12);
	read(sockfd,msg,4096);
	printf("%s\n",msg);
write(sockfd,"0123456783",11);
	read(sockfd,msg,12);

	printf("%s\n",msg);
long cc=1;

	while(1)
	{
		if(read(sockfd,msg,4096)>0)
		{
			printf("Got msg : %s\n",msg);
			if(strcmp(msg,"01234567830")==0)
			{
			write(sockfd,"REPLYREAD-3",12);
			}
			else
			write(sockfd,"RECVFAILEDD",12);
			printf("count --> %ld\n",cc++);
		}
	}
	close(sockfd);

    return 0;
}
