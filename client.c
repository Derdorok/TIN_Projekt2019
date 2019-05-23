// Client side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <unistd.h> 

#define PORT	 8080 
#define MAXLINE 1024 

// Driver code 
int main() { 
	int sockfd; 
	char *hello = "Client connected"; 
	struct sockaddr_in	 servaddr; 

	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 

	memset(&servaddr, 0, sizeof(servaddr)); 
	
	// Filling server information 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	
	int len; 
	len = sizeof(servaddr);
	
	//send new message
	sendto(sockfd, (const char *)hello, strlen(hello), 
		0, (const struct sockaddr *) &servaddr, 
			sizeof(servaddr)); 
	printf("Initial message sent.\n"); 
		

	//receive new port
	int freeport=0;
	recvfrom(sockfd, &freeport, 4, 
				0, ( struct sockaddr *) &servaddr, &len); 

	printf("New port number assigned: %d\n", freeport); 
	
	//wait for server to create new thread
	sleep(1);
	//change port and send new message
	servaddr.sin_port = htons(freeport);
	sendto(sockfd, (const char *)hello, strlen(hello), 
		0, (const struct sockaddr *) &servaddr, 
			sizeof(servaddr)); 
	printf("Messege send on port %d.\n", freeport); 

	close(sockfd); 
	return 0; 
} 
