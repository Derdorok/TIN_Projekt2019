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
	n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
				0, ( struct sockaddr *) &cliaddr, &len); 
	buffer[n] = '\0'; 
	printf("Client : %s on port %d\n", buffer, port); 
	
	close(sockfd);
	return NULL;
}

// Driver code 
int main() { 
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
	servaddr.sin_port = htons(PORT); 
	
	// Bind the socket with the server address 
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
			sizeof(servaddr)) < 0 ) 
	{ 
		perror("initial bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	//first free port
	int freeport = 8081;
	//maximum number of client threads
	pthread_t cThread[10];
	int len, n; 
	len = sizeof(servaddr);
	
	//main loop
	while(1){
	//wait for initial client message
	n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
				0, ( struct sockaddr *) &cliaddr, &len); 
	buffer[n] = '\0'; 
	printf("Client : %s\n", buffer); 
	
	//send client first free port number
	sendto(sockfd, &freeport, 4, 
		0, (const struct sockaddr *) &cliaddr, len); 
	printf("New port number sent: %d.\n", freeport); 
	
	//create new client thread on free port
	pthread_create(&cThread[freeport-8081], NULL, client_listener, freeport);
	
	freeport++;
	}
	
	//pthread_join(cThread, NULL);
	
	return 0; 
} 
