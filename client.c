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
	/* meta structure 
	first number selects type of message 1 - new client 2 - close connection 
	3 - data reporting */
	int structure[2];
	char* hello = "Message from client";
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
	
	//ask for a new port
	structure[0] = 1;
	structure[1] = 1;
	
	//send first message
	sendto(sockfd, (char *)structure, 2*sizeof(int), 
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
	int tmp = 0;
	
	while(tmp != 9){
		printf("1 to send message, 9 to close client\n");
		scanf("%d", &tmp);
		if(tmp == 1){
			servaddr.sin_port = htons(freeport);
			sendto(sockfd, (const char *)hello, strlen(hello), 
				0, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
		}
		if(tmp == 9){
				structure[0] = 2;
				structure[1] = freeport;
	
			servaddr.sin_port = htons(PORT);
			sendto(sockfd, (char *)structure, 2*sizeof(int), 
				0, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
		}
	}
	close(sockfd); 
	return 0; 
} 
