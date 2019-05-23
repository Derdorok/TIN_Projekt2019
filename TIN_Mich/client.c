#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#define BUFFER_SIZE 30000 /* rozmiar wysylanego pakietu */
#define ATTEMPTS 100 /* Ilosc wysłanych pakietów */

/*
argv[1] - nazwa hosta
argv[2] - numer portu
*/

int main(int argc, char **argv)
{
    char buf[BUFFER_SIZE]; /* tworze bufor na wiadomosci */
    struct sockaddr_in myaddr, nadawca;
    struct hostent *he;
    struct timeval time_b, time_e;
    socklen_t addrlen;
    FILE *plik;
    int gniazdo, sent, received;  

    if (argc<3) {
        printf("podaj nazwe hosta i numer portu jako parametry\n");
        return 1;
    }
	
	/* sprawdzam podany adres hosta */
    he = gethostbyname(argv[1]);
    if (he == NULL) {
        printf("Nieznany host: %s\n",argv[1]);
        return 0;
    }

    gniazdo = socket(AF_INET, SOCK_DGRAM, 0); /* tworze gniazdo */
    myaddr.sin_family = AF_INET;  /* ustawiam adres i port docelowy w gniezdzie */
    myaddr.sin_port = htons(atoi(argv[2]));
    myaddr.sin_addr = *(struct in_addr*) he->h_addr;
    
	plik = fopen("pomiarUDP.txt", "w");
    addrlen = sizeof(struct sockaddr_in);
    gettimeofday(&time_b, NULL);  /* pobieram aktualny czas */
    for(int i = 0; i< ATTEMPTS; i++){
        memset(buf, 100, BUFFER_SIZE); /* czysczcze i ustawiam pakiet (wartosc 100 oznacza litere d) czyli wypelniam pakier buforem skladajacym sie z buffer size razy litery d */
        sent = sendto(gniazdo, buf, BUFFER_SIZE, 0,			/* wysyłam pakiet do servera */
               (struct sockaddr*) &myaddr, sizeof(myaddr));
        fprintf(plik, "%d. pakiet: %s\n", i+1, buf); /* zapis pakietu do pliku */
        if (sent != BUFFER_SIZE) printf("sendto() nie powiodl sie\n");

        received = recvfrom(gniazdo, buf, BUFFER_SIZE, 0,  /* odbieram odpowiedz od serwera */
                     (struct sockaddr*) &myaddr, &addrlen);

        printf("%d. Message sent\n", i);
        
    }    
    close(gniazdo);   /* zamykam gniazdo i polaczenie */
    gettimeofday(&time_e, NULL);   /* pobieram czas i wyswietlam ile zajeło wysłanie pakietów*/

    printf("czas: %.6f s\n",
        (((double) (time_e.tv_sec - time_b.tv_sec) * 1000000) +
        ((double) (time_e.tv_usec - time_b.tv_usec)))
        / (1000000.0 * ATTEMPTS));
 	fprintf(plik, "czas: %.6f s\n", (((double) (time_e.tv_sec - time_b.tv_sec) * 1000000) +
        ((double) (time_e.tv_usec - time_b.tv_usec)))
        / (1000000.0 * ATTEMPTS));
        fclose(plik);
    return 0;
}
