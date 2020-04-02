#include <sys/types.h> /* For portability */ 
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>

static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

#define MAX_MTEXT 1024
#define SHM_KEY 123  
#define SHM_KEY2 1234     
#define BUF_SIZE 4096           /* Size of transfer buffer */
#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

struct shmseg {                 /* Defines structure of shared memory segment */
    int cnt;                    /* Number of bytes used in 'buf' */
    char buf[BUF_SIZE];         /* Data being transferred */
};

static void *threadFunc(void *arg)
{
    printf("\nThread Creata\n");
    
   
	
	
	sigset_t *set = arg;
    int s, sig;    /*storage for symbol */

    
    for (;;) {
        s = sigwait(set, &sig);
        if (s!=0){
			printf("Errore sigwait");
		}
        //printf("Signal handling thread got signal %d\n", sig);
        //psignal(sig, "Segnale interrotto");
        printf("Segnale catturato: %s\n", strsignal(sig));
        pthread_cond_signal( &cond ); 
    }

    return NULL;
}

int
main(int argc, char *argv[])
{
   
 	int shmid, shmid2, pid_ext;
 	char *shmp2;
    int *shmp, pid;
	pthread_t t1;
    int s;
	sigset_t set;
	char buf[1024];
	sigaddset( &set, SIGUSR1 );
	
	s = pthread_sigmask(SIG_BLOCK, &set, NULL);
    if (s!=0){
		printf("Errore sigmask");
	}

    
    s = pthread_create(&t1, NULL, threadFunc, (void *) &set);
	if (s!=0){
		printf("Errore creazione thread");
	}
	
	
	shmid = shmget(SHM_KEY, sizeof(struct shmseg), IPC_CREAT | OBJ_PERMS);
    if (shmid == -1)
        printf("shmget");
    shmp = (int *)shmat(shmid, NULL, 0);
    if (shmp == (void *) -1)
        printf("errore shmat");
	//strncpy(shmp, buf, BUF_SIZE);
	
	
	shmp[0]=getpid();
	printf("\n Pid processo 1:%d\n", shmp[0]);
    for(;;){
    	pid=shmp[1];
    	if (pid>shmp[0])
    		break;
    }
    printf("\n Pid processo 2:%d\n", shmp[1]);
    
    
	shmid2 = shmget(SHM_KEY2, sizeof(struct shmseg), IPC_CREAT | OBJ_PERMS);
    if (shmid2 == -1)
        printf("shmget");
    shmp2 = shmat(shmid2, NULL, 0);
    if (shmp2 == (void *) -1)
        printf("errore shmat");
    shmp[2]=0;
    for(;;){
    	
    	pthread_cond_wait( & cond, & mtx ); 
    	printf("\n Memory shared:%s\n", shmp2);
    	snprintf(buf,1024, "Processo 1");
    	strncpy(shmp2, buf, 1024-1);
    	shmp[2]++;
    	printf("\n Variabile globale:%d\n", shmp[2]);
    	kill(shmp[1],SIGUSR2); 
    }
    sleep(20);
    
    
    
    
    
    
    
  
    exit(0);

}