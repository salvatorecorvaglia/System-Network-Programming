
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
int main()
{

	char command[50];

   	strcpy( command, "ps x -o res,vsz,command | grep main" );
   	system(command);
	
    return 0;
}