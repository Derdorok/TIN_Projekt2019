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

#include "LogThreat.h"

#define PORT	 8080 
#define MAXLINE 1024 

//structure to remember free port numbers;
volatile int freeports[20];

struct thread_struct {
    int port;
    int logID;
    //LogThreat::logNode* lastLog;
};

//new_client
void *client_listener(void * parm) {
	struct thread_struct *args = parm;
	int port = args->port;
	int logID = args->logID;
	
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
	
	int len, n; 
	len = sizeof(servaddr);
	
	//wait for client message
//	n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
//				0, ( struct sockaddr *) &cliaddr, &len); 
//	buffer[n] = '\0'; 
//	printf("Client : %s on port %d\n", buffer, port); 
	
	fd_set readfds;
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
	struct timeval tv, time_send, time_begin;
	double send_delay = 5;
	char* msg = "Received messages: ";
	int bytes_sent, msg_counter = 0;
	
	gettimeofday(&time_begin, NULL);
	while(1){
		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);
		tv.tv_sec = 1;
		tv.tv_usec = 0;
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

			 
		} else {
			//printf("(Server) retval: %d\n", retval); 
		}
		

		double time_elapsed = ((double)time_send.tv_sec + (double)time_send.tv_usec/1000000) - ((double)time_begin.tv_sec + (double)time_begin.tv_usec/1000000);
		if(time_elapsed > send_delay){
			char num_received[2];
			sprintf(num_received, "%d", msg_counter);
			num_received[1] = '\0';
			char* response;
			
			if(response = malloc(strlen(msg)+strlen(num_received)+1)){
				response[0] = '\0';
				strcat(response, msg);
				strcat(response, num_received);
			} else {
				printf("malloc failed!\n");
			}


			bytes_sent = sendto(sockfd, (const char *)response, strlen(response), 
				0, (const struct sockaddr *) &cliaddr, 
					sizeof(servaddr)); 
			printf("(Server) Message sent on port %d. Bytes sent: %d. Time elapsed: %.4fs\n", port, bytes_sent, time_elapsed);

			msg_counter = 0;
			gettimeofday(&time_begin, NULL);
			if(response){
				free (response);
			}
		}
		
	}

	close(sockfd);
	return NULL;
}

// Driver code 
int main() { 
	int sockfd; 
	/* meta structure 
	first number selects type of message 1 - new client 2 - close connection 
	3 - data reporting */
	int structure[2];
	int freeport;
	int ID = 0;
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
		recvfrom(sockfd, structure, 2*sizeof(int), 
					0, ( struct sockaddr *) &cliaddr, &len); 
	
		if(structure[0] == 1){
			printf("New client\n"); 		
			//send client first free port number
			freeport = 0;
			for (int i = 0; i<20; i++){
				if (freeports[i] == 1){
					freeport = i+8081;
					freeports[i] = 0;
					break;
				}
			}
			sendto(sockfd, &freeport, 4, 
				0, (const struct sockaddr *) &cliaddr, len); 
			printf("Port number sent: %d.\n", freeport); 
			
			ID++;
			//LogThreat::logNode* tmp = system->addThreat(ID);
			
			struct thread_struct args;
			args.port = freeport;
			args.logID = ID;
			//args.lastLog = tmp;
			
			//create new client thread on free port an provide ID for logs
			pthread_create(&cThread[freeport-8081], NULL, client_listener, (void *)&args);
		}
		if(structure[0] == 2){		
			printf("closing port number: %d.\n", structure[1]); 
			//close old client port and kill thread
			freeport = structure[1]-8081;
			if(freeport>=0 && freeport<20){
				freeports[freeport]=-1;
			}
		}
		if(structure[0] == 3){

			//TODO - data processing to another thread
		}
	}
	
	return 0; 
} 
