#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <semaphore.h>
#define	BUFFSIZE	4096
sem_t *sem1;
struct node{
	char data[50];
	int last;
};

int append_node(struct node *head, char buf[50]) {
	struct node *pointer;
	pointer=head;
	while(1) {
		if(pointer->last) {
			pointer->last=0;
			pointer = (void*)((char*)pointer + sizeof(struct node));
			break;
		}
	 	pointer = (void*)((char*)pointer + sizeof(struct node));
	 }
	 pointer->last=1;
	 sprintf(pointer->data, "%s",buf);
	return 1;
}
int pop_node(struct node *head) {
	struct node *pointer;
	pointer=head;
	while(1) {
		if(pointer->last) {
			pointer->last=NULL;
			pointer = (void*)((char*)pointer - sizeof(struct node));
			break;
		}
	 	pointer = (void*)((char*)pointer + sizeof(struct node));
	 }
	pointer->last=1;
	return 1;
}
int update_node(struct node *head, char buf[50], int max) {
	int i=0;
	struct node *pointer;
	pointer=head;
	while(i<max-1){
		pointer = (void*)((char*)pointer + sizeof(struct node));
		i++;
	}
	sprintf(pointer->data, "%s",buf);
	return 1;
}

int read_list(struct node *head) {
	struct node *pointer;
	pointer=head;
	while(1) {
		printf("\nLeggo: %s %d\n",pointer->data, pointer->last);
		if(pointer->last) 
			break;
	 	pointer = (void*)((char*)pointer + sizeof(struct node));
	 }
	return 1;
}

int menu() {
	
	printf("Inserire comando:\n \n1 per inserire nuovo elemento\n2 per eliminare ultimo elemento\n3 per aggiornare elemento\n4 leggi lista\n-1 per uscire\n"); 
	
	return 1;
}
int main(int argc, char *argv[])
{	


	int s;
	if ((sem1 = sem_open("/semaph", O_CREAT, 0644, 1)) == SEM_FAILED ) {
        printf("errore sem1_open");
        exit(EXIT_FAILURE);
    }
    int fd, fd2, fdin, n;
    struct node *head, *pointer;
    char buf[BUFFSIZE];
	int command;
   int num;
   
   
    fd = shm_open("/myshmlist", O_RDWR | O_CREAT, 0666);      /* Open existing object */
    if (fd == -1)
        printf("error shm_open");  


	ftruncate(fd, BUFFSIZE);
    head = mmap(NULL, sizeof(struct node), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (head == MAP_FAILED)
          fprintf( stderr, "mmap failed: %s\n",
            strerror( errno ) );
    head->last=1;
    strcpy(head->data, "Head");
        while(1){
        	menu();  
        	 scanf("%d",&command);
        	if(command == -1)
            	break;
        	switch(command)	{
            	case 0:
					menu();
					break;
				case 1:
					printf("Occupo semaforo\n");
					sem_wait(sem1);
			
					printf("Inserisci stringa:\n");
					scanf("%s",buf);
					append_node(head,buf);
					sem_post(sem1);
					break;
				case 2:
					sem_wait(sem1);
					pop_node(head);
					sem_post(sem1);
					break;
				case 3:
					sem_wait(sem1);
					printf("Inserisci numero elemento da modificare:\n");
					scanf("%d",&num);
					printf("Inserisci stringa:\n");
					scanf("%s",buf);
					update_node(head, buf,num);
					sem_post(sem1);
					break;
				case 4:
					sem_wait(sem1);
					read_list(head);
					sem_post(sem1);
					break;
				case -1:
					printf("Arrivederci\n");
					exit(0);
					break;
        	}
		}
	
}
