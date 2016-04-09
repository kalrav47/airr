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
int count_sb = 0;
char global_message[1096] = "";
pthread_mutex_t lock;
pthread_t threads;
struct data *root=NULL;


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

	if(strlen(message) != 11) // If request is not 11 character long then return from here.
	{
		return;
	}
	
	tempp=root;		
	while(tempp  != NULL) // Looking for asked switchboard in database.
	{
		if(strstr(message,tempp->id)!= NULL) // When perticular switchboard found then communicate query with it.
		{
			send(tempp->csd,message, strlen(message)+1, MSG_NOSIGNAL );
			recv(tempp->csd,messsage,4096,0);
			strcpy(global_message,messsage);
			break;
		}
		else
		{
			tempp=tempp->next;
		}
	}
	
}

// Basic function to listen to all accepted connection.
//
void *start_listen_To_all( void *param)
{
	int *csdd = (int*)param;
	char message[MAX];
	struct data *temp;

	while(1)
	{
		if( read(*csdd,message,4096) > 0 ) // Start reading new connection
		{
			if(strcmp(message,"SWITCHBOARD")==0) // If connection is from switchboard then store its csd (connection number) and unique id.
			{
				write(*csdd,"ACKNOWLEDGE",12);
				read(*csdd,message,4096);
				write(*csdd,"ACKNOWLEDGE",12);

				struct data *ptr = (struct data *)malloc(sizeof(struct data));
				ptr->csd=*csdd;
				strcpy(ptr->id,message);

				if(root == NULL ) // If no root, then this is root.
				{
					root = ptr;
					root->next = NULL;
				}
				else // Put this connection at last position of link list.
				{
					temp=root;
					while(temp->next != NULL)
					{
						temp=temp->next;
					}
					temp->next=ptr;
					ptr->next=NULL;
				}
				break;
			}
			else if(strcmp(message,"HEARTBEATCK")==0) // Switchboard will check the server status (up/down) by sending HEARTBEATCK.
			{
				write(*csdd,"I'AMWORKING",12);
				close(*csdd);
				break;
			}	
			else // This means Application has asked something.
			{
				pthread_mutex_lock( &lock);
				broadcast_Message_For_SB(message); // Asking to switchboard for reply for this query sent by Application.
				pthread_mutex_unlock( &lock);

				write(*csdd,global_message, strlen(global_message)+1); // Writing reply back to Application
				strcpy(global_message,"");

				close(*csdd);
				break;
			}
		}
	}
}


// Main Program
//
int main(void)
{
	// Local Variables
	struct sockaddr_in saddress; 
	char message[MAX];

	pthread_mutex_init( &lock, NULL); // Mutex lock init
	// Socket connection creation process starts 
	//
	socket_server = socket( AF_INET, SOCK_STREAM, 0);
	if( socket_server == -1)
	{
		exit_function("Socket init write");
	}

	int on = 1;
	int ret = setsockopt( socket_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
	
	/* bind address to socket */
	saddress.sin_family = AF_INET;
	saddress.sin_port = htons(5000);
	saddress.sin_addr.s_addr = htonl(INADDR_ANY);

	if( bind( socket_server, (struct sockaddr*)&saddress, sizeof(saddress)) == -1)
	{
		exit_function("Socket bind write");
	}

	/* mark socket to listen for incoming connections */
	listen( socket_server, MAXCONN);
	
	printf("Server started ..... \n");

	/* wait for a client connection */
	while(1)
	{
		csd = accept( socket_server, (struct sockaddr*)NULL,NULL);

		pthread_create( &threads, NULL,start_listen_To_all, (void*)&csd);

		pthread_join(threads,NULL); // Wait untill communication finishes.
	}	
	
	close(socket_server);
	return 0;
}




























