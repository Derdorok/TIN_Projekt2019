// Server side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <pthread.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <math.h>

#include "LogThreat.h"

#define PORT	 8080 
#define MAXLINE 1024 

//structure to remember free port numbers;
volatile int freeports[20];

struct thread_struct {
    int port; //port used for communication with client
    int logID; //unique ID for storage of logs
    double data_delay; //delay between each message
    int data_size; //size in bytes of each message
};

//new_client
void *client_listener(void * parm) {
	struct thread_struct *args = (thread_struct *)parm;
	int port = args->port;
	int logID = args->logID;
	double send_delay = args->data_delay;
	int data_size = args->data_size;
	
	printf("New thread created %d.\n", port); 
	
	int sockfd; 
	char buffer[MAXLINE]; 
	struct sockaddr_in servaddr, cliaddr; 
	
	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	memset(&servaddr, 0, sizeof(servaddr)); 
	memset(&cliaddr, 0, sizeof(cliaddr)); 
	
	// Filling server information 
	servaddr.sin_family = AF_INET; // IPv4 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_port = htons(port); 
	
	// Bind the socket with the server address 
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
			sizeof(servaddr)) < 0 ) 
	{ 
		perror("client_listener bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	int n;
	unsigned int len; 
	len = sizeof(servaddr);
	
	//wait for client message
//	n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
//				0, ( struct sockaddr *) &cliaddr, &len); 
//	buffer[n] = '\0'; 
//	printf("Client : %s on port %d\n", buffer, port); 
	
	fd_set readfds;
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
	struct timeval tv, time_send, time_begin;

	char* msg = (char *)malloc(sizeof(char)*(data_size+1)); 
	for(int i = 0; i<data_size; ++i){
		msg[i] = 'S';
	}
	msg[data_size] = '\0';

	int bytes_sent, msg_counter = 0;
	
	gettimeofday(&time_begin, NULL);
	while(freeports[port-8081] > 1){
		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);
		tv.tv_sec = 0;
		tv.tv_usec = 5000;

		gettimeofday(&time_send, NULL);

		int retval = select(sockfd+1, &readfds, NULL, NULL, &tv);

		if(retval == -1){
			perror("select error"); 
		} else if(retval == 1){
			n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
				0, ( struct sockaddr *) &cliaddr, &len); 
			buffer[n] = '\0';
			printf("(Server) Message received: [%s]\n", buffer); 
			++msg_counter;

			//TODO - dodanie danych do logów
			 
		} else {
			//printf("(Server) retval: %d\n", retval); 
		}
		

		double time_elapsed = ((double)time_send.tv_sec + (double)time_send.tv_usec/1000000) - ((double)time_begin.tv_sec + (double)time_begin.tv_usec/1000000);
		if(time_elapsed > send_delay){
/*
			int n = log10(msg_counter) + 1;
			char* num_received = (char*)malloc(sizeof(char)*(n+1));
			sprintf(num_received, "%d", msg_counter);
			num_received[n] = '\0';
			char* response;
						
			if(response = (char *)malloc(strlen(msg)+strlen(num_received)+1)){
				response[0] = '\0';
				strcat(response, msg);
				strcat(response, num_received);
			} else {
				printf("malloc failed!\n");
			}
*/

			bytes_sent = sendto(sockfd, (const char *)msg, strlen(msg), 
				0, (const struct sockaddr *) &cliaddr, 
					sizeof(servaddr)); 
			printf("(Server) Message sent on port %d. Bytes sent: %d. Time elapsed: %.4fs\n", port, bytes_sent, time_elapsed);
			
			//TODO - dodanie danych do logów

			msg_counter = 0;
			gettimeofday(&time_begin, NULL);

/*
			if(response){
				free (response);
			}
			if(num_received){
				free (num_received);
			}
*/
		}
		
	}

	if(msg){
		free(msg);
	}

	close(sockfd);
	freeports[port-8081]=1;
	printf("Thread on port %d closing \n", port); 
	return NULL;
}

// Driver code 
int main() { 
	int sockfd; 
	int structure[3]; //meta structure
	int freeport;
	int ID = 100;
	struct sockaddr_in servaddr, cliaddr; 
	
	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	memset(&servaddr, 0, sizeof(servaddr)); 
	memset(&cliaddr, 0, sizeof(cliaddr)); 
	
	// Filling server information 
	servaddr.sin_family = AF_INET; // IPv4 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_port = htons(PORT); 
	
	// Bind the socket with the server address 
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
			sizeof(servaddr)) < 0 ) 
	{ 
		perror("initial bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	//TODO - stworzyć wątek do przetwarzania danych
	
	//initialize all ports to be free
	for (int i = 0; i<20; i++){
		freeports[i] = 1;	
	}
	//maximum number of client threads
	pthread_t cThread[10];
	unsigned int len; 
	len = sizeof(servaddr);
	//LogThreat* system = new LogThreat();
	
	//main loop
	while(1){
		//wait for initial client message
		recvfrom(sockfd, structure, 3*sizeof(int), 
					0, ( struct sockaddr *) &cliaddr, &len); 
	
		if(structure[0] == 1){		
			//increase unique ID for logs storage
			ID++;
			//send client first free port number
			freeport = 0;
			for (int i = 0; i<20; i++){
				if (freeports[i] == 1){
					freeport = i+8081;
					freeports[i] = ID;
					break;
				}
			}
			sendto(sockfd, &freeport, 4, 
				0, (const struct sockaddr *) &cliaddr, len); 
			printf("New client, port number sent: %d ID: %d.\n", freeport, ID); 
			
			struct thread_struct args;
			args.port = freeport;
			args.logID = ID;
			args.data_delay = structure[1];
			args.data_size = structure[2];
		
			//create new client thread on free port and supply it with data needed for communication
			pthread_create(&cThread[freeport-8081], NULL, client_listener, (void *)&args);
		}
		if(structure[0] == 2){	
			//Signal to close old client port and kill thread	
			printf("Client requested end of session, closing port number: %d.\n", structure[1]); 
			freeport = structure[1]-8081;
			if(freeport>=0 && freeport<20){
				freeports[freeport]=-1;
			}
		}
		if(structure[0] == 3){		 
			//Data about messages client received
			int data_port = structure[1];
			int data_bytes = structure[2];
			int data_ID = freeports[data_port-8081];
			printf("Client on port number: %d with log ID: %d recieived %d bytes.\n", data_port, data_ID, data_bytes);
			//TODO - dodanie danych do logów
		}
		if(structure[0] == 4){		
			//Data about messages client sent
			int data_port = structure[1];
			int data_bytes = structure[2];
			int data_ID = freeports[data_port-8081];
			printf("Client on port number: %d with log ID: %d sent %d bytes.\n", data_port, data_ID, data_bytes);
			//TODO - dodanie danych do logów
		}
	}
	
	return 0; 
} 
