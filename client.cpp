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
#include <errno.h>
#include <sys/select.h>
#include <sys/fcntl.h>
#include <sys/time.h>

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
	
	unsigned int len;
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
	
	servaddr.sin_port = htons(freeport);
	fd_set readfds;
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
	struct timeval tv, time_send, time_begin;
	double send_delay = 2;
	int bytes_read, bytes_sent;
	char receive_data[MAXLINE];
	char* msg = "Msg from client.";

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
			bytes_read = recvfrom(sockfd, &receive_data, MAXLINE, 
				0, ( struct sockaddr *) &servaddr, &len); 
			receive_data[bytes_read] = '\0';
			printf("(Client) Message received: [%s]\n", receive_data); 
		} else {
			
		}

		double time_elapsed = ((double)time_send.tv_sec + (double)time_send.tv_usec/1000000) - ((double)time_begin.tv_sec + (double)time_begin.tv_usec/1000000);
		if(time_elapsed > send_delay){
			bytes_sent = sendto(sockfd, (const char *)msg, strlen(msg), 
				0, (const struct sockaddr *) &servaddr, 
					sizeof(servaddr)); 
			printf("(Client) Message sent on port %d. Bytes sent: %d. Time elapsed: %.4fs\n", freeport, bytes_sent, time_elapsed); 
			gettimeofday(&time_begin, NULL);
		}
		
	}

	close(sockfd); 
	return 0; 
} 
