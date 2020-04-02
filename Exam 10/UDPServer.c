#include <pthread.h>
#include <stdio.h>
#include <stdlib.h> 
#include <fcntl.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include "Header.h"
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static volatile int avail = 0;

static void *                  
threadFunc(void *arg)
{	
	int port = *((int *) arg);
	int s;
	printf("\tIPv4 UDP Server app\n");
		
	int res = 0; //valore di ritorno delle APIs

	/*
	socket: servirà per la comunicazione col server
	*/
	int sockfd = 0;
	
	/*
	open socket di tipo datagram
	*/
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1)
	{
		perror("socket error: ");
		return NULL;
	}
	
	/*
	indirizzo IPv4 del server, senza hostname resolution
	*/
	struct sockaddr_in server;
	socklen_t len = sizeof(server);
	
	memset(&server, 0, sizeof(server)); //azzero la struttura dati
	server.sin_family = AF_INET; //specifico l'Address Family IPv4
	
	/*
	Specifico l'indirizzo del server: qualsiasi interfaccia
	*/
	server.sin_addr.s_addr = htonl(INADDR_ANY);
		
	/*
	Specifico la well-known port
	*/
	server.sin_port = htons(port);
	
	//setto l'indirizzo well-known del socket
	res = bind(sockfd, (struct sockaddr *)&server, len);
	if (res == -1)
	{
		perror("Bind error: ");
		close(sockfd);
		exit(1);
	}//fi
	
		
	ssize_t n = 0;
	char buffer[BUFSIZE];
	char msg[BUFSIZE];
	struct sockaddr_in client;
	char address[INET_ADDRSTRLEN] = "";
	snprintf(msg, BUFSIZE-1, "prodotto creato");
	int quit = 0;
	
	while (!quit)
	{
		n = recvfrom(sockfd, buffer, BUFSIZE-1, 0, (struct sockaddr *)&client, &len);
		if (n == -1)
		{
			perror("recvfrom() error: ");
			continue;
// 			close(sockfd);
// 			return FAILURE;
		}
	
		buffer[n] = '\0';
		printf("\tRicevuto messaggio:\n\t'%s'\n\tda: %s:%d\n", buffer, \
		inet_ntop(AF_INET, &(client.sin_addr), address, INET_ADDRSTRLEN), \
		ntohs(client.sin_port) );

		printf("Sending reply...\n");
		n = sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&client, len);
		if (n == -1)
		{
			perror("sendto() error: ");
			continue;
// 			close(sockfd);
// 			return FAILURE;
		}
		s = pthread_mutex_lock(&mtx);
		avail++;
		printf("\nThread connessa alla porta %d sta creando oggetto... \n", port);
		printf("\nQuantità attuale: %d \n", avail);
		s = pthread_mutex_unlock(&mtx);
		pthread_cond_signal( &cond ); 
		
	}//wend
		
	//qui non ci arrivo mai...	
	close(sockfd);
    return NULL;
}

int main(){
	
	pthread_t t1, t2, t3;
    int  s, port1, port2, port3;
	int numConsumed=0;
	port1=49152;
	port2=49153;
	port3=49154;
   time_t t;
    
t = time(NULL);
    
    s = pthread_create(&t1, NULL, threadFunc, &port1);
		if (s!=0){
		printf("Errore creazione thread");
		}
	s = pthread_create(&t2, NULL, threadFunc, &port2);
		if (s!=0){
		printf("Errore creazione thread");
		}
	s = pthread_create(&t3, NULL, threadFunc, &port3);
		if (s!=0){
		printf("Errore creazione thread");
		}
	
	 for (;;) {
        s = pthread_mutex_lock(&mtx);
        if (s != 0)
            printf("main pthread_mutex_lock");

        while (avail == 0) {            /* Wait for something to consume */
            s = pthread_cond_wait(&cond, &mtx);
            if (s != 0)
                printf("pthread_cond_wait");
        }

        /* At this point, 'mtx' is locked... */

        while (avail > 0) {             /* Consume all available units */

            /* Do something with produced unit */

            numConsumed ++;
            avail--;
            printf("T=%ld: numConsumed=%d\n", (long) (time(NULL) - t),
                    numConsumed);

            
        }

        s = pthread_mutex_unlock(&mtx);
        if (s != 0)
            printf("main pthread_mutex_unlock");

        if (numConsumed>1000)
            break;

        /* Perhaps do other work here that does not require mutex lock */

    }
	s = pthread_join(t1, NULL);
	s = pthread_join(t2, NULL);
	s = pthread_join(t3, NULL);
	
	
	
return 0;
}

/** @} */
