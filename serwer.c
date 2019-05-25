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

#define PORT	 8080 
#define MAXLINE 1024 

//structure to remember free port numbers;
volatile int freeports[20];

//new_client
void *client_listener(void * parm) {
	int port = (int)parm;
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
	
	//add socket timeout
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	
	// Bind the socket with the server address 
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
			sizeof(servaddr)) < 0 ) 
	{ 
		perror("client_listener bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	int len, n; 
	len = sizeof(servaddr);
	
	//while - check if client didn't close communication 
	while(freeports[port-8081] == 0){
	
		//wait for client message
		n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
					0, ( struct sockaddr *) &cliaddr, &len); 
		
		if(n>0){
			buffer[n] = '\0'; 
			printf("Client : '%s' on port %d\n", buffer, port); 
		}
	}
	
	//close the port signal that it is free to use and exit
	close(sockfd);
	freeports[port-8081]=1;
	printf("Thread on port %d closing \n", port); 
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
	int len; 
	len = sizeof(servaddr);
	
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
			
			//create new client thread on free port
			pthread_create(&cThread[freeport-8081], NULL, client_listener, freeport);
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
