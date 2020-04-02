/**
@addtogroup Group4 
@{
*/
/**
@file 	UDPClient.c
@author Catiuscia Melle

@brief 	Presentazione di un UDP Echo Client IPv4, senza hostname resolution ma con 
		l'utilizzo della funzione inet_pton() per la conversione di un IPv4 dotted 
		decimal in intero a 32 bit.
*/

#include "Header.h"
#include <pthread.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
sem_t *sem;
static void *                  
threadFunc(void *arg)
{	
	sem_t *sem;
	if ((sem = sem_open("/sem2", 0)) == SEM_FAILED ) {
        printf("thread sem_open");
        exit(EXIT_FAILURE);
    }
    
	sem_wait(sem);
	printf("\tIPv4 UDP Client app\n");
	
	//messaggio da inviare al server
	char msg[BUFSIZE] = "";
	int port = *((int *) arg);
	memset(msg, '.', 1);

	
	msg[BUFSIZE-1] = '\0'; //tronco il messaggio a BUFSIZE caratteri

	int res = 0; 	//valore di ritorno delle APIs
	int sockfd = 0; //socket descriptor per la comunicazione col server
	
	// open socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1)
	{
		perror("socket error: ");
		return FAILURE;
	}
	
	/*
	indirizzo IPv4 del server, senza hostname resolution
	*/
	struct sockaddr_in server;
	socklen_t len = sizeof(server);
	
	memset(&server, 0, sizeof(server)); //azzero la struttura dati
	server.sin_family = AF_INET; //specifico l'Address FAmily IPv4
	
	//Specifico l'indirizzo del server inet_pton()

	server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	
	/*
	Specifico la porta del server da contattare 
	in NETWORK BYTE ORDER
	*/
	server.sin_port = htons(port);


	ssize_t n = 0;
	n = sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&server, len);
	if (n == -1)
	{
		perror("sendto() error: ");
		close(sockfd);
		return FAILURE;
	}
	char serverIP[INET_ADDRSTRLEN] = "";
	
	printf("Client sent %d bytes to server at '%s:%d'\n", (int) n, inet_ntop(AF_INET, &(server.sin_addr), serverIP, INET_ADDRSTRLEN), ntohs(server.sin_port));
	
	//blocco processo ai fini di monitoraggio dello stesso

	n = recvfrom(sockfd, msg, n, 0, (struct sockaddr *)&server, &len);
	//n = recvfrom(sockfd, msg, BUFSIZE-1, 0, (struct sockaddr *)&server, &len);
	if (n == -1)
	{
		perror("recvfrom() error: ");
		close(sockfd);
		return FAILURE;
	}
	
	msg[n] = '\0';
	printf("Server reply = '%s'\n", msg);
	sem_post(sem);
	close(sockfd);
	
return NULL;
}



int main(int argc, char *argv[]){




	int t1, t2, s;
	int port1=48150;
	int port2=48252;
	int value;
	sem_t *sem;
	
	 if ((sem = sem_open("/sem2", O_CREAT, 0644, 1)) == SEM_FAILED ) {
        printf("sem_open");
        exit(EXIT_FAILURE);
    }
	//
	//for(int j=0;j<5;j++){
	s = pthread_create(&t1, NULL, threadFunc, &port1);
	if (s!=0){
		printf("Errore creazione thread");
	}
	
	
	s = pthread_create(&t2, NULL, threadFunc, &port2);
	if (s!=0){
		printf("Errore creazione thread");
	}
		
		
	s = pthread_join(t1, NULL);
	s = pthread_join(t2, NULL);
	//}
	sleep(1);
}

/** @} */
