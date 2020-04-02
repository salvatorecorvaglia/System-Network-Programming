
#include "Header.h"
#include <pthread.h>
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

static void *                   /* Loop 'arg' times incrementing 'glob' */
threadFunc(void *arg)
{	
	int number = *((int *) arg);
	int s;
	printf("\nthread n %d creata\n", number);
	printf("\nPrendo il lock...\n", number);
	            
    int res = 0; //valore di ritorno delle Sockets API
	
	int sockfd = 0; //connection socket: servirà per la comunicazione col server
	
	struct sockaddr_in server; //IPv4 server address, senza hostname resolution 
	socklen_t len = sizeof(server); //dimensione della struttura di indirizzo
	
	memset(&server, 0, sizeof(server)); //azzero la struttura dati
	server.sin_family = AF_INET; //specifico l'Address FAmily IPv4
	if( number==1)
		server.sin_port = htons(PORT);
	if( number==2)
		server.sin_port = htons(PORT2);

		printf("\tTCP Client app connecting to localhost TCP server...\n");
		

		server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	
	//connessione al server
	/* open socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("socket error: ");
		return FAILURE;
	}
	
	//avvio il 3WHS
	res = connect(sockfd, (struct sockaddr *)&server, len);
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

   	
	//messaggio predefinito da inviare al server
	char msg[BUFSIZE] = "";
	int counter = 0;
	ssize_t n = 0;
	
	
		snprintf(msg, BUFSIZE-1, "msg from %d", number);
		
		sleep(1);
		
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
		printf("\Rilascio il lock...\n", number);
		pthread_cond_signal( &cond ); 
         
		}
	
	//4-way teardown
	 close(sockfd);
	 	

        
    
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

	pthread_t t1, t2;
    int  s;
    int n1=1;
    int n2=2;
	s = pthread_create(&t1, NULL, threadFunc, &n1);
		if (s!=0){
		printf("Errore creazione thread");
		}
	pthread_cond_wait( & cond, & mtx ); 
	s = pthread_create(&t2, NULL, threadFunc, &n2);
		if (s!=0){
		printf("Errore creazione thread");
		}
		
	s = pthread_join(t1, NULL);
	s = pthread_join(t2, NULL);
	
	

return 0;
}

/** @} */
