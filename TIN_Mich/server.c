#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	 unsigned int port;
     const int BUFFER_SIZE = 30000; /* rozmiar bufora */ 
     char bufor[BUFFER_SIZE];
	 int gniazdo;
     struct sockaddr_in adr, nadawca;
	 socklen_t addrlen;
	 int received, sent;
	 
	 if (argc < 2) {
        printf("podaj numer portu jako parametr\n");
        return 1;
    }

	 gniazdo = socket(PF_INET, SOCK_DGRAM, 0); /* tworze gniazdo udp na podanym porcie */
	 adr.sin_family = AF_INET;
	 adr.sin_port = htons(atoi(argv[1]));
	 adr.sin_addr.s_addr = INADDR_ANY;
	
	/* rejestruje stworzone gniazdo w systemie */
	 if (bind(gniazdo, (struct sockaddr*) &adr, 
		 sizeof(struct sockaddr_in)) < 0) {
		 printf("Bind nie powiodl sie.\n");
		 return 1;
	 }
	 
    addrlen = sizeof(struct sockaddr_in);
    int i = 0;
     while(1){
        memset(bufor, 0, BUFFER_SIZE);  /* czyszcze i ustawiam bufor pakietu */ 
   
        received = recvfrom(gniazdo, bufor, BUFFER_SIZE, 0,  /* odbieram wiadomosc */ 
                     (struct sockaddr*) &nadawca, &addrlen);
        
        sent = sendto(gniazdo, bufor, BUFFER_SIZE, 0,
               (struct sockaddr*) &nadawca, sizeof(nadawca));
        if (sent != BUFFER_SIZE) printf("sendto() nie powiodl sie\n");
        printf("%d\n", i);
        i++;

     }
	 
	
	 close(gniazdo); /* zamykam gniazdo i polaczenie */
	 return 0; 
}
