#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <semaphore.h>
#include <unistd.h>


#define BUF_SIZE 124
sem_t *sem1;
struct user{
    int id;
    char nome[BUF_SIZE];
    char cognome[BUF_SIZE];
    int eta;
};
void menu() {
    printf("Benvenuto, inserisci un comando :\n1. Inserisci\n2. Modifica\n3. Leggi file\n-1. Esci\n");
}

int
main(int argc, char *argv[])
{	

	int s;
	if ((sem1 = sem_open("/semaph", O_CREAT, 0644, 1)) == SEM_FAILED ) {
        printf("errore sem1_open");
        exit(EXIT_FAILURE);
    }
    char buf[BUF_SIZE];
    int command;
    
    
    FILE * pFile;
	//sem_post(sem1);
    
    while(1) {
        printf("\nEnter a command(0-3,-1 to quit):");
        menu();
        scanf("%d",&command);
        if(command == -1)
            break;
        switch(command)
        {
            case 0:
                menu();
                break;
            case 1: {
            	printf("Occupo semaforo\n");
                sem_wait(sem1);
                printf("Mi metto in pausa:\n");
                sleep(20);
                pFile = fopen("registro.txt", "a");
        
                if (pFile == NULL){
                    perror("opening file");
                    exit(1);
                }
            
                struct user new;
                printf("Inserisci il nome:\n");
                scanf("%s",new.nome);
                printf("Inserisci il cognome:\n");
                scanf("%s",new.cognome);
                printf("Inserisci l'età:\n");
                scanf("%d",&new.eta);
                //printf("%s %s %d\n",new.nome,new.cognome,new.eta);
                if(fwrite (&new, sizeof(struct user), 1, pFile) != 0) 
                    printf("Utente inserito con successo!\n");
                
                if (fclose(pFile) == -1) {
                    perror("close input"); 
                    exit(1);
                }
                sem_post(sem1);
                break;
            }
            case 2: {
                sem_wait(sem1);
                pFile = fopen("registro.txt", "r+");
        
                if (pFile == NULL){
                    perror("opening file");
                    exit(1);
                }
                
                int id;
                struct user new;
                printf("Inserisci l'ID del utente che vuoi modificare\n");
                scanf("%d",&id);
                printf("Inserisci il nome:\n");
                scanf("%s",new.nome);
                printf("Inserisci il cognome:\n");
                scanf("%s",new.cognome);
                printf("Inserisci l'età:\n");
                scanf("%d",&new.eta);
                
                id=id-1; /*-1, perché il primo elemeto della struttura si trova a offset 0 e il secondo a offset 1 volta la struc*/
                fseek(pFile, id*sizeof(struct user), SEEK_SET); //places the pointer at the struct I want
                
                if(fwrite (&new, sizeof(struct user), 1, pFile) != 0) 
                    printf("Utente modificato con successo!\n");
    
                if (fclose(pFile) == -1) {
                    perror("close input"); 
                    exit(1);
                }
                sem_post(sem1);
                break;
            }
            case 3: {
            	sem_wait(sem1);
                pFile = fopen("registro.txt", "r");
        
                if (pFile == NULL){
                    perror("opening file");
                    exit(1);     
                }
                    
                struct user read;
                int i = 1;
                while(fread(&read, sizeof(struct user), 1, pFile)) {
                    printf ("ID: %d nome: %s cognome: %s età: %d\n", i, read.nome, read.cognome, read.eta);
                    i++;
                }
                if (fclose(pFile) == -1) {
                    perror("close input"); 
                    exit(1);
                }
                sem_post(sem1);
                break;
            }
            case -1: {
                printf("Arrivederci\n");
                exit(0);
                break;
            }
        }
    }
}
