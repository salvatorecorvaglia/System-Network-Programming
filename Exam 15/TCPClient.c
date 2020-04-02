/**
@addtogroup Group2
@{
*/
/**
@file 	TCPClient.c
@author Catiuscia Melle

@brief 	Presentazione di un TCP Echo Client IPv4, senza hostname resolution

Questo esempio illustra le chiamate base che è necessario implementare per poter avviare un TCP client. 

Il client deve:
- definire l'indirizzo del server da contattare: 
	- o usiamo l'interfaccia di loopback per dialogare col server;
	- o usiamo la funzione <em>inet_pton()</em> per la conversione di un IPv4 dotted decimal in intero a 32 bit in Network Byte Order, da potersi utilizzare in una <em>connect()</em> verso il server TCP.
- stabilire la connessione verso il server TCP;
- inviare una serie di messaggi;
- chiudere la connessione;
- terminare l'esecuzione.
*/

#include "Header.h"
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <signal.h>


#define FAILURE 1 	/**< Valore di ritorno del processo in caso di insuccesso */
#define ERROR 2 	/**< Valore di ritorno del processo in caso di errore */
#define BUFFEr_SIZE 4096


threadFunc(void *arg)
{
    printf("\nThread Creata\n");
    
    
    FILE *f2;
	f2 = fopen("signal.log", "a+"); // a+ (create + append) option will allow appending which is useful in a log file
	if (f2 == NULL) {
		printf("Can't create log-signal file");
	}
	
	
	sigset_t *set = arg;
    int s, sig;    /*storage for symbol */

    
    for (;;) {
        s = sigwait(set, &sig);
        if (s!=0){
			printf("Errore sigwait");
		}
        printf("Signal handling thread got signal %d\n", sig);
        psignal(sig, "Segnale interrotto");
        fprintf(f2, "Segnale catturato: %s\n", strsignal(sig));
        fflush(f2);
    }

    return NULL;
}


void printSockaddr_in(struct sockaddr_in *sa){
 	/* 
 	stringhe in cui verranno memorizzati gli indirizzi IP convertiti in stringa
  	(/usr/include/netinet/in.h)
 	 INET_ADDRSTRLEN                 16 
 	 INET6_ADDRSTRLEN                46
 	*/
 	char ipv4[INET_ADDRSTRLEN] = {0}; 
 	const char *ptr = NULL;

 	ptr = inet_ntop(AF_INET, &(sa->sin_addr), ipv4, INET_ADDRSTRLEN);
 	
 	if (ptr != NULL)
 	{
 		printf("\nIPv4 address = '%s'\n", ipv4);
 	}
 	else
 	{
 		perror("inet_ntop() error: ");
 	}
 	
	printf("Port number = %d\n\n", ntohs(sa->sin_port));
}


int main(int argc, char *argv[]){

 	pthread_t t1;
    int s;
    struct timeval start;
	struct timeval end;
	double secs = 0;
	sigset_t set;
	
	
	
	sigfillset(&set);
    s = pthread_sigmask(SIG_BLOCK, &set, NULL);
    if (s!=0){
		printf("Errore sigmask");
	}

    
    s = pthread_create(&t1, NULL, threadFunc, (void *) &set);
	if (s!=0){
		printf("Errore creazione thread");
	}


	if (argc != 3) 
	{
	    fprintf(stdout, "Usage: %s <hostname> [<service name>]\n", argv[0]);
	    return FAILURE;
	} 

	int res = 0; //valore di ritorno delle Sockets API
	
	int sockfd = 0; //connection socket: servirà per la comunicazione col server
	
	struct sockaddr_in server; //IPv4 server address, senza hostname resolution 
	socklen_t len = sizeof(server); //dimensione della struttura di indirizzo
	
	memset(&server, 0, sizeof(server)); //azzero la struttura dati
	
	printf("\tHostname Resolution for %s:%s\n", argv[1], argv[2]);
	
	int g_err; //valore di ritorno della getaddrinfo
	char ipstr[INET6_ADDRSTRLEN]; //stringa atta a contenere un indirizzo IPv4 e/o IPv6
	
	struct addrinfo hints;
	struct addrinfo *result; 
	struct addrinfo *p; //ptr addizionale
	
	memset(&hints, 0, sizeof hints);
	
	hints.ai_family = AF_UNSPEC; 	//sia struct sockaddr_in che sockaddr_in6
	hints.ai_socktype = 0; //sia TCP che UDP service
	hints.ai_flags |= AI_V4MAPPED; 
	g_err = getaddrinfo(argv[1], argv[2], &hints, &result);
	if (g_err != 0) 
	{
		fprintf(stdout, "getaddrinfo Error (%d): %s\n", g_err, gai_strerror(g_err)); 
		return ERROR;
	}
	
	

	
	p = result;
	/* IPv4
	eseguo il cast della struct sockaddr puntata da p->ai_addr
	ad una struct sockaddr_in per poterne interpretare i campi di indirizzo IPv4 e porta
	*/
	
	struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
	printf("\tIPv4 Address: %s:%d\n",
					inet_ntop(p->ai_family, &(ipv4->sin_addr), ipstr, sizeof(ipstr)), ntohs(ipv4->sin_port) );
							
	printf("\tTCP Client app connecting to TCP server at '%s'\n", argv[1]);
	server.sin_port = htons(25);   // porta alla quale inviare la connessione
	server.sin_family = AF_INET;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("socket error: ");
		return FAILURE;
	}
	
	//avvio il 3WHS
	res = connect(sockfd, p->ai_addr, len);
	if (res != 0)
	{
		perror("connect() error: ");
		close(sockfd);
		return FAILURE;
	}
	
   	struct sockaddr_storage sa;
    socklen_t sa_len;
	sa_len = sizeof(struct sockaddr);
	if (getsockname(sockfd, (struct sockaddr *) &sa, &sa_len) == -1) {
     	perror("getsockname() failed");
     	return -1;
   	}
   
   	printSockaddr_in((struct sockaddr *) &sa);
	
	char msg[BUFSIZE] = "";
	int counter = 0;
	ssize_t n = 0;
	
	n = recv(sockfd, msg, BUFSIZE -1, 0);
	printf("server reply: '%s'\n", msg);
	
	
		snprintf(msg, BUFSIZE-1, "ehlo %s\n", argv[1]);
		
		n = send(sockfd, msg, strlen(msg), 0);
		if (n == -1)
		{
			perror("send() error: ");
			close(sockfd);
			return FAILURE;
		}
		printf("Client queued %d bytes to server\n", (int) n);
		
		n = recv(sockfd, msg, BUFSIZE -1, 0);
		if (n == -1) {
			perror("recv() error: ");
			close(sockfd);
			return FAILURE;
		} else if (n == 0) {
			printf("server closed connetion\n");
		}
		if (n > 0) {
			msg[n] = 0;
			printf("server reply: '%s'\n", msg);
		}
	
	
	
	
	snprintf(msg, BUFSIZE-1, "QUIT\n");
		
		
		n = send(sockfd, msg, strlen(msg), 0);
		if (n == -1)
		{
			perror("send() error: ");
			close(sockfd);
			return FAILURE;
		}
		gettimeofday(&start, NULL);
		printf("Client queued %d bytes to server\n", (int) n);
		
		n = recv(sockfd, msg, BUFSIZE -1, 0);
		gettimeofday(&end, NULL);
		if (n == -1) {
			perror("recv() error: ");
			close(sockfd);
			return FAILURE;
		} else if (n == 0) {
			printf("server closed connetion\n");
		}
		if (n > 0) {
			msg[n] = 0;
			printf("server reply: '%s'\n", msg);
		}
		
		
		secs = (double)(end.tv_usec - start.tv_usec) / 1000000 + (double)(end.tv_sec - start.tv_sec);
		printf("time taken %f secondi\n",secs);
		
		
    	FILE *f;
		f = fopen("time.log", "a+"); // a+ (create + append) option will allow appending which is useful in a log file
		if (f == NULL) {
			printf("Can't create log file");
		}
		fprintf(f, "time taken %f secondi\n",secs);

return 0;
}

/** @} */
