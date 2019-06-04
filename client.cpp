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
#include <iostream>
#include <poll.h>

#define PORT	 8080 
#define MAXLINE 1024 

// Driver code 
int main(int argc, char* argv[]) { 
	int serv_msg_len = 10, cli_msg_len = 10;
	double send_delay = 2, serv_send_delay = 5;
	if(argc == 2) {
		if(atoi(argv[1]) > 0){	
			cli_msg_len = atoi(argv[1]);
		}
	} else if(argc == 3) {
		if(atoi(argv[1]) > 0){	
			cli_msg_len = atoi(argv[1]);
		}
		if(atoi(argv[2]) > 0){	
			serv_msg_len = atoi(argv[2]);
		}
	} else if(argc == 4) {
		if(atoi(argv[1]) > 0){	
			cli_msg_len = atoi(argv[1]);
		}
		if(atoi(argv[2]) > 0){	
			serv_msg_len = atoi(argv[2]);
		}
		if(atoi(argv[3]) > 0){	
			send_delay = atof(argv[3]);
		}
	} else if(argc > 4) {
		if(atoi(argv[1]) > 0){	
			cli_msg_len = atoi(argv[1]);
		}
		if(atoi(argv[2]) > 0){	
			serv_msg_len = atoi(argv[2]);
		}
		if(atoi(argv[3]) > 0){	
			send_delay = atof(argv[3]);
		}
		if(atoi(argv[4]) > 0){	
			serv_send_delay = atof(argv[4]);
		}
	}

	int sockfd; 
	int structure[3];
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
		
	//send initial message with session data
	structure[0] = 1; // session initiation
	structure[1] = serv_send_delay; // delay between each message (sec)
	structure[2] = serv_msg_len; //size in bytes of each message 	
	
	//send first message
	sendto(sockfd, (char *)structure, 3*sizeof(int), 
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

	int bytes_read, bytes_sent;
	char receive_data[MAXLINE];

	char* msg = (char *)malloc(sizeof(char)*(cli_msg_len+1)); 
	for(int i = 0; i<cli_msg_len; ++i){
		msg[i] = 'C';
	}
	msg[cli_msg_len] = '\0';

	pollfd cinfd[1];
	cinfd[0].fd = fileno(stdin);
	cinfd[0].events = POLLIN;
	std::string command;

	gettimeofday(&time_begin, NULL);
	while(1){ 
		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);
		tv.tv_sec = 0;
		tv.tv_usec = 5000;

		gettimeofday(&time_send, NULL);

		int retval = select(sockfd+1, &readfds, NULL, NULL, &tv);

		if(retval == -1){
			perror("select error"); 
		} else if(retval == 1){
			bytes_read = recvfrom(sockfd, &receive_data, MAXLINE, 
				0, ( struct sockaddr *) &servaddr, &len); 
			receive_data[bytes_read] = '\0';
			printf("(Client) Message received: [%s]\n", receive_data); 
			
			//send data about transmission
			structure[0] = 3; //received messages data
			structure[1] = freeport; //port that client uses during session
			structure[2] = bytes_read; //number of bytes received
			
			servaddr.sin_port = htons(PORT);
			sendto(sockfd, (char *)structure, 3*sizeof(int), 
				0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
			servaddr.sin_port = htons(freeport);	
		} 

		double time_elapsed = ((double)time_send.tv_sec + (double)time_send.tv_usec/1000000) - ((double)time_begin.tv_sec + (double)time_begin.tv_usec/1000000);
		if(time_elapsed > send_delay){ 
			bytes_sent = sendto(sockfd, (const char *)msg, strlen(msg), 
				0, (const struct sockaddr *) &servaddr, 
					sizeof(servaddr)); 
			printf("(Client) Message sent on port %d. Bytes sent: %d. Time elapsed: %.4fs\n", freeport, bytes_sent, time_elapsed); 
			//send data about transmission
			structure[0] = 4; //sent messages data
			structure[1] = freeport; //port that client uses during session
			structure[2] = strlen(msg); //number of bytes sent
			
			servaddr.sin_port = htons(PORT);
			sendto(sockfd, (char *)structure, 3*sizeof(int), 
				0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
			servaddr.sin_port = htons(freeport);	
			gettimeofday(&time_begin, NULL);
		}

		if(poll(cinfd,1,1)){
			getline(std::cin, command);
			if(command == "q"){
				printf("Exiting...\n");
				break;
			}
		}	
	}
	
	//TODO - po zakończonej pracy klient powinien wysłać poniższą wiadomość do serwera, to już przetestowałem i działa
	structure[0] = 2; //signal to end session
	structure[1] = freeport; //port that client used during session
	structure[2] = 0; //arbitrary number
	
	servaddr.sin_port = htons(PORT);
	sendto(sockfd, (char *)structure, 3*sizeof(int), 
		0, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 

	if(msg){
		free(msg);
	}

	close(sockfd); 
	return 0; 
} 
