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
	int structure[3];
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
	
	//TODO - tu trzeba pobrać dane co ile i jak duże wiadomości powinien wysyłać serwer 
	//i wpisać je do struktury poniżej, w tej chwili jest przykład
	
	//send initial message with session data
	structure[0] = 1; // session initiation
	structure[1] = 5; // delay between each message
	structure[2] = 2000; //size in bytes of each message 
	
	
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
	double send_delay = 2;
	int bytes_read, bytes_sent;
	char receive_data[MAXLINE];
	char* msg = "Msg from client."; //TODO - długość wiadomości musi być regulowana

	gettimeofday(&time_begin, NULL);
	
	while(1){ //TODO - dorobić opcję wyłączania klienta, albo po z góry ustalonym czasie, albo interaktywnie
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
			
			//send data about transmission
			structure[0] = 3; //received messages data
			structure[1] = freeport; //port that client uses during session
			structure[2] = bytes_read; //number of bytes received
			
			servaddr.sin_port = htons(PORT);
			sendto(sockfd, (char *)structure, 3*sizeof(int), 
				0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
			servaddr.sin_port = htons(freeport);	
		} else {
			
		}

		double time_elapsed = ((double)time_send.tv_sec + (double)time_send.tv_usec/1000000) - ((double)time_begin.tv_sec + (double)time_begin.tv_usec/1000000);
		if(time_elapsed > send_delay){ //TODO - send_delay powinien być regulowany
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
		
	}
	
	//TODO - po zakończonej pracy klient powinien wysłać poniższą wiadomość do serwera, to już przetestowałem i działa
	structure[0] = 2; //signal to end session
	structure[1] = freeport; //port that client used during session
	structure[2] = 0; //arbitrary number
	
	servaddr.sin_port = htons(PORT);
	sendto(sockfd, (char *)structure, 3*sizeof(int), 
		0, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 

	close(sockfd); 
	return 0; 
} 
