/**
@addtogroup Group11
@{
*/
/**
@file 	Header.h
@author Catiuscia Melle

@brief 	Header comune al client e server dell'esempio

L'header definisce costanti comuni al client e server.
*/

#ifndef __HEADER_H__
#define __HEADER_H__

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> //resolver related functions

#include <sys/types.h> //necessario su Mac
#include <stdlib.h>

#include <ctype.h> //toupper()
#include <stdbool.h>

#define PORT 		48150 		/**< TCP listening port, decimal */
#define PORT2 		48252 		/**< TCP listening port, decimal */
#define PORT3 		48252 		/**< TCP listening port, decimal */
#define PORT4 		48252 		/**< TCP listening port, decimal */
#define PORT5 		48252 		/**< TCP listening port, decimal */
#define PORT6 		48252 		/**< TCP listening port, decimal */
#define PORT7 		48252 		/**< TCP listening port, decimal */
#define PORT8 		48252 		/**< TCP listening port, decimal */
#define PORT9 		48252 		/**< TCP listening port, decimal */
#define PORT10 		48252 		/**< TCP listening port, decimal */


#define PORTNUMBER0 		48150 		/**< TCP listening port, decimal */
#define SERVICEPORT0 	"48150" 	/**< TCP listening port, name */

#define PORTNUMBER 		48150 		/**< TCP listening port, decimal */
#define SERVICEPORT	"48150" 	/**< TCP listening port, name */

#define PORTNUMBER2 		48252 		/**< TCP listening port, decimal */
#define SERVICEPORT2 	"48252" 	/**< TCP listening port, name */

#define PORTNUMBER3 		48353 		/**< TCP listening port, decimal */
#define SERVICEPORT3 	"48353" 	/**< TCP listening port, name */

#define PORTNUMBER4		48154 		/**< TCP listening port, decimal */
#define SERVICEPORT4	"48154" 	/**< TCP listening port, name */

#define PORTNUMBER5 		48155 		/**< TCP listening port, decimal */
#define SERVICEPORT5 	"48155" 	/**< TCP listening port, name */

#define PORTNUMBER6 		48156 		/**< TCP listening port, decimal */
#define SERVICEPORT6 	"48156" 	/**< TCP listening port, name */

#define PORTNUMBER7 		48457 		/**< TCP listening port, decimal */
#define SERVICEPORT7 	"48457" 	/**< TCP listening port, name */

#define PORTNUMBER8 		48958 		/**< TCP listening port, decimal */
#define SERVICEPORT8 	"48958" 	/**< TCP listening port, name */

#define PORTNUMBER9 		48159 		/**< TCP listening port, decimal */
#define SERVICEPORT9 	"48159" 	/**< TCP listening port, name */

#define PORTNUMBER10 		48160 		/**< TCP listening port, decimal */
#define SERVICEPORT10 	"48160" 	/**< TCP listening port, name */

#define PORT_STRLEN 6 /**< lunghezza di una stringa rappresentativa di un indirizzo di trasporto */

#define BACKLOG 10 	/**< dimensione della coda di connessioni */

#define SIZE 15 /**< dimensione della coda di connessioni concorrenti gestite dal server */

#define BUFSIZE 512 /**< dimensione del buffer di messaggio */

#define FAILURE 3 	/**< definizione del valore di errore di ritorno del processo in caso di errori delle Sockets API */

#define INVALID 5 	/**< se i due processi non sono avviati col giusto numero di parametri */

#define ERROR 1 /**<  valore di errore */

#define INSUCCESS -1 /**< valore di ritorno non valido */


#endif /* __HEADER_H__ */

/** @} */