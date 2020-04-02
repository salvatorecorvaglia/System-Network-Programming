#include <pthread.h>
#include <stdio.h>
#include <stdlib.h> 
#include <fcntl.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

static volatile int glob = 0;
static volatile int glob2 = 0;
static volatile int indice = 0;
static volatile int max = 100;
static volatile int array[1000000];
static volatile int noprimi[1];
static volatile int fattori[1];
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
#define	BUFFSIZE	4096

char * myfifo = "/tmp/myfifo";
char * myfifo2 = "/tmp/fifo2";




static void
sig_int(int signo)	/* one signal handler for SIGINT and SIGQUIT */
{
	if (signo == SIGINT) {
		printf("\ninterrupt\n");
		printf("\nTrovati %d numeri primi\n", glob2);
		for(int j=0; j<glob2; j++)
			printf("\n %d° numero primo: %d\n", j+1,array[j]);
			
	}
	kill(getpid(), 9);
}



int primo(int num){
	
	printf("\n Controllo 	%d\n", num);
	int i;
	int max = num/2+1;
	if(num % 2 == 0){
		fattori[indice]=2;
		return 0;
	}
	for(i=3; i<=max; i++){
		if(num%i == 0) {
			fattori[0]=i;
			return 0;
		}
	}
	return 1;
	
}

static void *                  
threadFunc(void *arg)
{	
	
	int fd, n, i, res;
	fd = open(myfifo, O_WRONLY);
	char buf[i+1];
	for(int j=0; j<max; j++) {
		i = rand();
		printf("\nGenerato: %d\n", i);
		snprintf(buf, i, "%d", i);
		if (write(fd, buf, BUFFSIZE) != BUFFSIZE)
			printf("\n errore invio \n");
		pthread_cond_wait( & cond, & mtx); 
	}
	
	close(fd);
    return NULL;
}


static void *                  
threadFunc2(void *arg)
{	
	int i, x, fd, n, fd2;
	char buf[BUFFSIZE];
	
	fd = open(myfifo, O_RDONLY);
	fd2 = open(myfifo2, O_WRONLY);
	while ((n = read(fd, buf, BUFFSIZE)) > 0){
		indice++;
		x=atoi(buf);
		if(primo(x)){
			pthread_cond_signal( &cond ); 
			glob2++;
			if (write(fd2, buf, BUFFSIZE) != BUFFSIZE)
				printf("\n errore invio \n");
		} else {
			noprimi[0]=x;
			pthread_cond_signal( &cond2 ); 
		}
		pthread_cond_signal( &cond ); 
		
	}
	
	printf("\n Inviati %d primi\n", glob2);
	close(fd2);
    return NULL;
}


static void *                  
threadFunc3(void *arg)
{	
	
	int fd3, n, x, i=0;
	char buf[BUFFSIZE];
	fd3 = open(myfifo2, O_RDONLY);
	while ((n = read(fd3, buf, BUFFSIZE) ) > 0){
		//if(glob==0)
			//pthread_cond_wait( & cond, & mtx); 
		x=atoi(buf);
		printf("\n Thread 3 legge il numero primo: 	%d\n", x);
		array[i]=x;
		i++;
	}
	
	
    return NULL;
}

static void *                  
threadFunc4(void *arg)
{	
	
	for (;;) {
		pthread_cond_wait( & cond2, & mtx); 
		printf("\n Trovato numero : %d divisibile per: %d\n", noprimi[0],fattori[0]);
	}
    return NULL;
}
int main(int argc, char *argv[]){

 pthread_t t1, t2, t3, t4;
    int  s;
    

	if (signal(SIGINT, sig_int) == SIG_ERR)
		printf("signal(SIGINT) error");



    if(mkfifo(myfifo, 0666))
    	printf("FIFO1 creata");
    if(mkfifo(myfifo2, 0666))
    	printf("\nFIFO2 creata");
   
    

    
    s = pthread_create(&t1, NULL, threadFunc, NULL);
		if (s!=0){
		printf("Errore creazione thread");
		}
	s = pthread_create(&t2, NULL, threadFunc2, NULL);
		if (s!=0){
		printf("Errore creazione thread");
		}
	s = pthread_create(&t3, NULL, threadFunc3, NULL);
		if (s!=0){
		printf("Errore creazione thread");
		}
		
	s = pthread_create(&t4, NULL, threadFunc4, NULL);
		if (s!=0){
		printf("Errore creazione thread");
		}
	s = pthread_join(t1, NULL);
	s = pthread_join(t2, NULL);
	s = pthread_join(t3, NULL);
	s = pthread_join(t4, NULL);

	
	
return 0;
}

/** @} */


