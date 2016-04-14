/*
This is the program which will work as a server for the project airr.
If incoming connection is from switchboard then it will connect and store 
its data.
And if incoming connection is from Application then it will just pass its
query to perticular switchboard and again pass the reply of switchboard to 
Apllication.

AUTHOR : Kalrav J Parsana
CONTACT : kalravparsana@gmail.com
DATE: 09/04/2016
COMPILATION COMMAND :  gcc server.c -o server -lpthread -w

*** DO NOT SHARE WITHOUT PERMISSION ***
*/

//General header files
//
#include<stdio.h> 
#include<unistd.h>
#include<pthread.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<signal.h>



//Global Structer to store data of all communication switchboards
struct data{
int csd;
char id[12];
struct node *next;
};


// General macros
//
#define MAX 4096
#define MAXCONN 1000


// Global variables
//
int csd=0,socket_server,port,ssize,i;
int count_sb = 0,a=0;
char global_message[1096] = "";
pthread_mutex_t lock;
pthread_t threads;
struct data *root=NULL;
sig_atomic_t alarm_counter;

//If program is exiting then print error and then exit normally.
//
void exit_function(char message[MAX])
{	
	printf("Exiting server with error : %s\n",message);
	exit(0);
}

// Broadcast msg for our switchboard. Strictly 11 chars only.
//
void broadcast_Message_For_SB(char message[MAX])
{
	int i;
	char messsage[4096];
	struct data *tempp;
	printf("Checkpoint 18.\n");
	if(strlen(message) != 11) // If request is not 11 character long then return from here.
	{
		return;
	}
	printf("Checkpoint 19.\n");
	tempp=root;		
	while(tempp  != NULL) // Looking for asked switchboard in database.
	{printf("Checkpoint 20.\n");
		if(strstr(message,tempp->id)!= NULL) // When perticular switchboard found then communicate query with it.
		{	printf("Checkpoint 21.\n");
			send(tempp->csd,message, strlen(message)+1,MSG_NOSIGNAL );printf("Checkpoint 22.\n");
			read(tempp->csd,messsage,4096);printf("Checkpoint 23.\n");
			strcpy(global_message,messsage);printf("Checkpoint 24.\n");
			break;
		}
		else
		{
			tempp=tempp->next;printf("Checkpoint 25.\n");
		}
	}
	
}

static void sig_alarm(int signo)
{
	a=0;
 
}

// Basic function to listen to all accepted connection.
//
/*void *start_listen_To_all( void *param)
{
	int *csdd = (int*)param;
	char message[MAX];
	int recvCount=0;
	int check=0;
	struct data *temp;
	
	 alarm(1);
	while(1)
	{
			if( read(*csdd,message,4096) > 0 ) // Start reading new connection
		{	printf("Checkpoint 8.\n");
			if(strcmp(message,"SWITCHBOARD")==0) // If connection is from switchboard then store its csd (connection number) and unique id.
			{	printf("Checkpoint 9.\n");
				write(*csdd,"ACKNOWLEDGE",12);printf("Checkpoint 10.\n");
				read(*csdd,message,12);printf("Checkpoint 11.\n");
				write(*csdd,"ACKNOWLEDGE",12);printf("Checkpoint 12.\n");

				struct data *ptr = (struct data *)malloc(sizeof(struct data));
				ptr->csd=*csdd;
				strcpy(ptr->id,message);
				check=0;
				printf("Checkpoint 13.\n");
				if(root == NULL ) // If no root, then this is root.
				{
					root = ptr;
					root->next = NULL;
					
				}
				else // Put this connection at last position of link list.
				{
					if(strstr(message,root->id)!= NULL)
					{
						root->csd=*csdd;
						break;
					}
					temp=root;
					while(temp->next != NULL)
					{
						temp=temp->next;
						if(strstr(message,temp->id)!= NULL)
						{
							temp->csd=*csdd;
							check=1;
							break;
						}
					}
					if(check == 0)
					{
						temp->next=ptr;
						ptr->next=NULL;
					}
				}printf("Checkpoint 14.\n");
				break;
			}
			else if(strcmp(message,"HEARTBEATCK")==0) // Switchboard will check the server status (up/down) by sending HEARTBEATCK.
			{
				printf("Checkpoint 15.\n");
				write(*csdd,"I'AMWORKING",12);
				close(*csdd);printf("Checkpoint 16.\n");
				break;
			}	
			else // This means Application has asked something.
			{
				pthread_mutex_lock( &lock);printf("Checkpoint 17.\n");
				broadcast_Message_For_SB(message); // Asking to switchboard for reply for this query sent by Application.
				pthread_mutex_unlock( &lock);printf("Checkpoint 26.\n");

				write(*csdd,global_message, strlen(global_message)+1); printf("Checkpoint 27.\n");// Writing reply back to Application
				strcpy(global_message,"");

				close(*csdd);printf("Checkpoint 28.\n");
				break;
			}
		}
	}alarm(0);
}*/


// Main Program
//
int main(void)
{
	// Local Variables
	struct sockaddr_in saddress; 
	char message[MAX];
	printf("Checkpoint 1.\n");

	pthread_mutex_init( &lock, NULL); // Mutex lock init

	 if (signal(SIGALRM, sig_alarm) == SIG_ERR)
	   {
	       exit(0);
	   }

	// Socket connection creation process starts 
	//
	socket_server = socket( AF_INET, SOCK_STREAM, 0);
	if( socket_server == -1)
	{
		exit_function("Socket init write");
	}
	printf("Checkpoint 2.\n");
	const struct timeval sock_timeout={.tv_sec=2, .tv_usec=0};
	
	int on = 1;
	int ret = setsockopt( socket_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
	
	/* bind address to socket */
	saddress.sin_family = AF_INET;
	saddress.sin_port = htons(5000);
	saddress.sin_addr.s_addr = htonl(INADDR_ANY);
	printf("Checkpoint 3.\n");
	if( bind( socket_server, (struct sockaddr*)&saddress, sizeof(saddress)) == -1)
	{
		exit_function("Socket bind write");
	}

	/* mark socket to listen for incoming connections */
	listen( socket_server, MAXCONN);
	printf("Checkpoint 4.\n");
	printf("Server started ..... \n");
	fd_set readset;int retval;
	/* wait for a client connection */
	while(1)
	{
		printf("Checkpoint 5.\n");
		csd = accept( socket_server, (struct sockaddr*)NULL,NULL);
		printf("Checkpoint 6 with csd %d.\n",csd);a=1;
		//FD_ZERO(&readset); 
    		//FD_SET(socket_server,&readset); 
		const struct timeval sock_timeout={.tv_sec=1, .tv_usec=0};
		/*retval = select(socket_server+1, &readset, NULL, NULL, &sock_timeout); 
		    if (retval <= 0) 
		    { 
			continue;  //error or connection timed out 
		    } 
		    else 
		    { 
			pthread_create( &threads, NULL,start_listen_To_all, (void*)&csd);
			printf("Checkpoint 29.\n");
			pthread_join(threads,NULL);
		   // } 
		 // Wait untill communication finishes.
		printf("Checkpoint 30.\n");*/


		//new code -----------------------------------------
			int check=0;
		struct data *temp;
		a=1;
		 alarm(1);
		while(a==1)
		{
			if( recv(csd,message,4096,MSG_DONTWAIT) > 0 ) // Start reading new connection
			{printf("Checkpoint 8.\n");
				if(strcmp(message,"SWITCHBOARD")==0) // If connection is from switchboard then store its csd (connection number) and unique id.
				{	printf("Checkpoint 9.\n");
					write(csd,"ACKNOWLEDGE",12);printf("Checkpoint 10.\n");
					read(csd,message,12);printf("Checkpoint 11.\n");
					write(csd,"ACKNOWLEDGE",12);printf("Checkpoint 12.\n");

					struct data *ptr = (struct data *)malloc(sizeof(struct data));
					ptr->csd=csd;
					strcpy(ptr->id,message);
					check=0;
					printf("Checkpoint 13.\n");
					if(root == NULL ) // If no root, then this is root.
					{
						root = ptr;
						root->next = NULL;
					
					}
					else // Put this connection at last position of link list.
					{
						if(strstr(message,root->id)!= NULL)
						{
							root->csd=csd;
							break;
						}
						temp=root;
						while(temp->next != NULL)
						{
							temp=temp->next;
							if(strstr(message,temp->id)!= NULL)
							{
								temp->csd=csd;
								check=1;
								break;
							}
						}
						if(check == 0)
						{
							temp->next=ptr;
							ptr->next=NULL;
						}
					}printf("Checkpoint 14.\n");
					break;
				}
				else if(strcmp(message,"HEARTBEATCK")==0) // Switchboard will check the server status (up/down) by sending HEARTBEATCK.
				{
					printf("Checkpoint 15.\n");
					write(csd,"I'AMWORKING",12);
					close(csd);printf("Checkpoint 16.\n");
					break;
				}	
				else // This means Application has asked something.
				{
					pthread_mutex_lock( &lock);printf("Checkpoint 17.\n");
					broadcast_Message_For_SB(message); // Asking to switchboard for reply for this query sent by Application.
					pthread_mutex_unlock( &lock);printf("Checkpoint 26.\n");

					write(csd,global_message, strlen(global_message)+1); printf("Checkpoint 27.\n");// Writing reply back to Application
					strcpy(global_message,"");

					close(csd);printf("Checkpoint 28.\n");
					break;
				}
			}
		}alarm(0);if(a==0) close(csd);
		
	}	
	pthread_exit(NULL);
	close(socket_server);
	return 0;
}




























