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
	
	int check=0;
	int sockfd2 = 0; /* listening socket del server TCP */
	int peerfd2 = 0; /* connected socket del client TCP */
	int ret2 = 0; 	/* valore di ritorno delle Sockets API */
	
	sockfd2 = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd2 == -1)
	{
		perror("socket() error: ");
		return FAILURE;
	}
	
	
	struct sockaddr_in addr2;
	memset(&addr2, 0, sizeof(addr2));
	addr2.sin_family= AF_INET;//IPv4 family
	
	
	addr2.sin_addr.s_addr = htonl(INADDR_ANY);
	
	
	addr2.sin_port = htons(PORT1);
	
	ret2 = bind(sockfd2, (struct sockaddr *)&addr2, sizeof(struct sockaddr_in));
	if (ret2 == -1)
	{
		perror("bind() error: ");
		close(sockfd2);
		return FAILURE;
	}
	
	ret2 = listen(sockfd2, BACKLOG);
	if (ret2 == -1)
	{
		perror("listen() error: ");
		close(sockfd2);
		return FAILURE;
	}
	struct sockaddr_in peer_addr2;
	socklen_t len2 = sizeof(peer_addr2);
	
	
	
	
	int res = 0; //valore di ritorno delle Sockets API
	
	int sockfd = 0; //connection socket: servirà per la comunicazione col server
	
	struct sockaddr_in server; //IPv4 server address, senza hostname resolution 
	socklen_t len = sizeof(server); //dimensione della struttura di indirizzo
	
	memset(&server, 0, sizeof(server)); //azzero la struttura dati
	server.sin_family = AF_INET; //specifico l'Address FAmily IPv4
	
	/*
	Specifico la porta del server da contattare, verso cui avviare il 3WHS
	*/
	server.sin_port = htons(PORT2);
	
	if (argc == 1)
	{
		//non è stato indicato un indirizzo IPv4 per il server, usiamo localhost
		printf("\tTCP Client app connecting to localhost TCP server...\n");
		
		/* Specifico l'indirizzo del server usando 
		il wildcard address IPv4 INADDR_LOOPBACK per specificare 
		l'indirizzo di loopback 127.0.0.1 in Network Byte Order
		*/
		server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	} 
	else 
	{
		printf("\tTCP Client app connecting to TCP server at '%s'\n", argv[1]);
		
		/* Utilizzo inet_pton() per convertire l'indirizzo dotted decimal */
		res = inet_pton(AF_INET, argv[1], &(server.sin_addr));
		if (res == 1){
			printf("Memorizzato l'indirizzo IPv4 del server\n");
		}
		else if (res == -1)
		{
			perror("Errore inet_pton: ");
			return FAILURE;
		}
		else if (res == 0)
		{
			printf("The input value is not a valid IPv4 dotted-decimal string\n");
			return FAILURE;
		}
	}
	
	//connessione al server
	/* open socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("socket error: ");
		return FAILURE;
	}
	
	
	
    
   
   
   	ssize_t n = 0;
   	
   	
   	peerfd2 = accept(sockfd2, (struct sockaddr *)&peer_addr2, &len2);
		if (peerfd2 == -1)
		{
			perror("accept() error: ");
			close(sockfd2);
			return FAILURE;
		}
		
		char clientaddr[INET_ADDRSTRLEN] = "";
		inet_ntop(AF_INET, &(peer_addr2.sin_addr), clientaddr, INET_ADDRSTRLEN);
		printf("\tAccepted a new TCP connection from %s:%d\n", clientaddr, ntohs(peer_addr2.sin_port));
		char buf[BUFSIZE];
		
		n = recv(peerfd2, buf, BUFSIZE-1, 0);
		check++;
		printf("\nCheck:%d\n", check);
			if (n == -1)
			{
				perror("recv() error: ");
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
	printSockaddr_in((struct sockaddr *) &sa);
	
		
		n = send(sockfd, buf, strlen(buf), 0);
		check--;
		printf("\nCheck:%d\n", check);
		if (n == -1)
		{
			perror("send() error: ");
			close(sockfd);
			return FAILURE;
		}
		printf("Client queued %d bytes to server\n", (int) n);
		
		
	//4-way teardown
	close(sockfd);

return 0;
}

/** @} */
