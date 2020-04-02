#include <sys/types.h>
#include <unistd.h>
#include "Header.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "Utility.h"

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

int join_group(int sockfd, struct sockaddr *multicastAddr, socklen_t addrlen){

	//address-family-specific code for Join Multicast Groups
  	if (multicastAddr->sa_family == AF_INET6) 
  	{
    	// join the multicast v6 "group" (address)
    	struct ipv6_mreq joinRequest;
    	
    	memcpy(&joinRequest.ipv6mr_multiaddr, &((struct sockaddr_in6 *) multicastAddr)->sin6_addr,  
    	sizeof(struct in6_addr));
     	
     	//index=0: Let the system choose the ingoing interface
    	joinRequest.ipv6mr_interface = 0; 
    	
    	printf("Joining IPv6 multicast group...\n");
    
    	if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_JOIN_GROUP, &joinRequest, sizeof(joinRequest)) < 0)
    	{
    		perror("setsockopt(IPV6_JOIN_GROUP) failed: ");
    		return FAILURE;
		}
		return SUCCESS;
  	} 
  	else if (multicastAddr->sa_family == AF_INET) 
  	{
    	//join the multicast v4 "group" (address)
    	struct ip_mreq joinRequest;
    	//set the multicast IPv4 address
    	joinRequest.imr_multiaddr = ((struct sockaddr_in *) multicastAddr)->sin_addr;
    	
    	//the system choose the ingoing interface - wildcard IPv4 address
    	joinRequest.imr_interface.s_addr = htonl(INADDR_ANY);
    	
    	printf("Joining IPv4 multicast group...\n");
    	
    	if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &joinRequest, sizeof(joinRequest)) < 0)
    	{
    		perror("setsockopt(IPV4_ADD_MEMBERSHIP) failed: ");
    		return FAILURE;
		}
      return SUCCESS;
  	} 

//se siamo qui, qualcosa è andato male...
return FAILURE;  
}

int leave_group(int sockfd, struct sockaddr *multicastAddr, socklen_t addrlen){
	
	if (multicastAddr->sa_family == AF_INET)
	{
		// Leave the multicast v4 "group"
		struct ip_mreq leaveRequest;
		leaveRequest.imr_multiaddr = ((struct sockaddr_in *) multicastAddr)->sin_addr;
    	leaveRequest.imr_interface.s_addr = htonl(INADDR_ANY);
    	
    	return (setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &leaveRequest, sizeof(leaveRequest)));
	}
	else if (multicastAddr->sa_family == AF_INET6)
	{
		// Leave the multicast v6 "group"
		struct ipv6_mreq leaveRequest;
		leaveRequest.ipv6mr_multiaddr = ((struct sockaddr_in6 *) multicastAddr)->sin6_addr;
    	leaveRequest.ipv6mr_interface =0;
    	
    	return (setsockopt(sockfd, IPPROTO_IPV6, IPV6_LEAVE_GROUP, &leaveRequest, sizeof(leaveRequest))); 
	}
	
return FAILURE;
}

static void * multicastReceiver(void *arg){
	
	char multicastAddrString[INET6_ADDRSTRLEN] = ""; //multicast address
	char multicastPortString[INET6_ADDRSTRLEN] = ""; //UDP port
	

		//multicast group address
		memcpy(multicastAddrString, "224.0.0.1", INET6_ADDRSTRLEN);
		
		//multicast port 
		memcpy(multicastPortString, "48150", INET6_ADDRSTRLEN);
	

	printf("Multicast Receiver for <%s:%s>\n", multicastAddrString, multicastPortString); 
	
	int result = 0;
	int sockfd = -1;
	
	struct addrinfo hints, *res, *ptr;

	//Zero out hints
	memset(&hints, 0, sizeof(hints)); 

  	//resolution hints:
	hints.ai_family = AF_UNSPEC;      // v4 or v6 is OK
  	hints.ai_socktype = SOCK_DGRAM;   // Only datagram sockets
  	hints.ai_protocol = IPPROTO_UDP;  // Only UDP protocol
  	hints.ai_flags |= AI_NUMERICHOST; // Don't try to resolve address
	hints.ai_flags |= AI_NUMERICSERV; // Don't try to resolve port

	int ecode = getaddrinfo(multicastAddrString, multicastPortString, &hints, &res);
  	if (ecode != 0)
  	{
    	printf("getaddrinfo() failed: %s", gai_strerror(ecode));
    	return FAILURE;
	}
	
	for (ptr = res; ptr != NULL; ptr=ptr->ai_next)
	{
		//open UDP socket
		sockfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (sockfd == -1)
		{	
			perror("socket() error: ");
			continue;	
		}
		
		//opzione per il riuso dell'indirizzo
		int reuse_on = 1;
	
		//allow address reuse
		result = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_on, sizeof(reuse_on));
		if (result == -1)
		{
			perror("setsockopt() error: ");
			close(sockfd);
			continue;
		}
			
		//bind socket to multicast address and port
		result = bind(sockfd, ptr->ai_addr, ptr->ai_addrlen);
		if (result < 0)
		{
			perror("bind() error: ");
			close(sockfd);
			continue;
		}
		break;
	}//for
	
	if (ptr == NULL)
	{	/* errno set from final socket() */
		printf("receiver error for %s, %s\n",  multicastAddrString, multicastPortString);
		freeaddrinfo(res);
		return FAILURE;
	}

	//sockfd is binded to the multicast address and port
	//now we must join the group
	
	result = join_group(sockfd, ptr->ai_addr, ptr->ai_addrlen);
	if (result == FAILURE)
	{
		//chiudo e dealloco
		close(sockfd);
		freeaddrinfo(res);
		return FAILURE;
	}
	
	
	//handle_multicast_session(sockfd);
	int quit = 0;
	ssize_t nbytes = 0;
	char MultiMessage[BUFSIZE] = "";

	struct sockaddr_storage source;
	socklen_t source_len = sizeof(source);

	//printf("Ready for receiving multicast traffic\n");
	int niflags = NI_NUMERICHOST | NI_NUMERICSERV;
	char ip[INET6_ADDRSTRLEN] = "";
	char port[INET6_ADDRSTRLEN] = "";
	
	while (!quit)
	{
		nbytes = recvfrom(sockfd, MultiMessage, BUFSIZE-1, 0,\
						 (struct sockaddr *)&source, &source_len);
		if (nbytes < 0)
		{
			perror("recvfrom() error: ");
			quit = 1;
		}
		else
		{
			MultiMessage[nbytes]=0;
			ecode = getnameinfo((struct sockaddr *)&source, source_len, \
					ip, INET6_ADDRSTRLEN, port,INET6_ADDRSTRLEN, niflags);
			if (ecode)
			{
				printf("getnameinfo error: %s\n", gai_strerror(ecode));
			}
			else
			{
				printf("Multicast Message from %s:%s\n", ip, port);
			}
	
			printf("\t%s\n", MultiMessage);		
		 
			/*if (strncmp(MultiMessage, QUIT, strlen(MultiMessage)) == 0)
			{
				printf("Quit received\n..closing\n");
				quit = 1;
			}*/
		}
	}//wend
		
	
	result = leave_group(sockfd, ptr->ai_addr, ptr->ai_addrlen);
	if (result == -1)
	{
		perror("setsockopt error: ");
	}
	
	
	//chiudo e dealloco
	close(sockfd);
	freeaddrinfo(res);
	
	return NULL;
}

int main(int argc, char *argv[]){
	
	pthread_t t1;
    int  t;
    
	t = pthread_create(&t1, NULL, multicastReceiver, NULL);
		if (t!=0){
		printf("Errore creazione thread");
		}
		
		
	int res = 0; //valore di ritorno delle Sockets API
	const char s[2] = " ";
	int sockfd = 0; //connection socket: servirà per la comunicazione col server
	int x, y, pid;
	pid=getpid();
	char msg[BUFSIZE] = "";
	char buf[BUFSIZE] = "";
	x=rand()%100;
    y=rand()%100;
    int row, columns;
    int matrix[100][100];
    snprintf(msg, BUFSIZE, "%d %d %d", x, y, getpid());
    
    
    for (int row=0; row<100; row++) {
			for(int columns=0; columns<100; columns++) {
         		matrix[row][columns]=0;
         		
       		}
       }
 
   
	struct sockaddr_in server; //IPv4 server address, senza hostname resolution 
	socklen_t len = sizeof(server); //dimensione della struttura di indirizzo
	
	memset(&server, 0, sizeof(server)); //azzero la struttura dati
	server.sin_family = AF_INET; //specifico l'Address FAmily IPv4
	
	/*
	Specifico la porta del server da contattare, verso cui avviare il 3WHS
	*/
	server.sin_port = htons(PORT);
	
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
   	
	//messaggio predefinito da inviare al server
	
	int counter = 0;
	ssize_t n = 0;
	char c;
	printf("\nPosizione di partenza: X= %d, Y= %d\n", x, y);
	
	
	for(;;){
	counter++;
	printf("\n%d\n",counter);
		//snprintf(msg, BUFSIZE-1, "msg number %d", ++counter);
		
		printf("Inserire movimento oppure premere c per confermare l'attuale posizione\n U: alto\n H: sinistra\n J: destra\n N: basso\n");
		c=getchar();
		getchar();
		if (c == 'u' || c == 'U') {
			if (x==0)
				printf("\nmossa non consentita\n");
			else
				x--; 
		}
		if (c == 'n' || c == 'N') {
			if (x==199)
				printf("\nmossa non consentita\n");
			else
				x++; 
		}
		if (c == 'h' || c == 'H') {
			if (y==0)
				printf("\nmossa non consentita\n");
			else
				y--; 
		}
		if (c == 'j' || c == 'J') {
			if (y==199)
				printf("\nmossa non consentita\n");
			else
				y++; 
		}
		snprintf(msg, BUFSIZE, "%d %d %d", x, y, getpid());
		printf("\nNuova posizione: X= %d, Y= %d\n", x, y);
		
		//sleep(5);
		
		n = send(sockfd, msg, BUFSIZE-1, 0);
		if (n == -1)
		{
			perror("send() error: ");
			close(sockfd);
			return FAILURE;
		}
		printf("Client queued %d bytes to server\n", (int) n);
		
		n = recv(sockfd, buf, BUFSIZE, 0);
		if (n == -1) {
			perror("recv() error: ");
			close(sockfd);
			return FAILURE;
		} else if (n == 0) {
			printf("server closed connetion\n");
			break;
		}
		if (n > 0) {
			msg[n] = 0;
			printf("server reply: '%s'\n", buf);
		
		}
		matrix[x][y]=1;
		for (int row=0; row<99; row++) {
			for(int columns=0; columns<99; columns++) {
         		printf("%d ", matrix[row][columns]);
         		
       		}
       		printf("\n");
       }
		
		
		
		
		
	}//while
	
	//4-way teardown
	close(sockfd);

return 0;
}

/** @} */
