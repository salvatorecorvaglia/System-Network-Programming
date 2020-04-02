#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <pthread.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#define KEY 123
#define KEY2 1234
#define MUTEX "/mutex_lock"
int mode = S_IRWXU | S_IRWXG;
// The program checks whether a mutex can be shared among processes
pthread_mutex_t* mutex;
int main(int argc, const char* argv[]) 
{	

	int    ShmID, ShmID2;
    int    *var;
    int    status;
    
    
    ShmID = shmget(KEY, 4*sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }
     
	var = (int *) shmat(ShmID, NULL, 0);
    if ((int) var == -1) {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
     
     ShmID2 = shm_open("/mutex_lock", O_CREAT | O_RDWR, mode);

	if (ShmID2 < 0) {
    	perror("failure on shm_open on des_mutex");
    	exit(1);
	}
	/*if (ftruncate(ShmID2, sizeof(pthread_mutex_t)) == -1) {
    perror("Error on ftruncate to sizeof pthread_cond_t\n");
    exit(-1);
	}*/
	mutex = (pthread_mutex_t*) mmap(NULL, sizeof(pthread_mutex_t),
        PROT_READ | PROT_WRITE, MAP_SHARED, ShmID2, 0);
    if (mutex == MAP_FAILED ) {
    perror("Error on mmap on mutex\n");
    exit(1);
}

// initialize the attribute
	pthread_mutexattr_t attr;
	pthread_mutexattr_init (&attr);
	pthread_mutexattr_setpshared (&attr, PTHREAD_PROCESS_SHARED); // this is what we're testing

// initialize the mutex
	pthread_mutex_init (mutex, &attr);
	pthread_mutexattr_destroy (&attr);

// acquire the lock 
	pthread_mutex_lock (mutex);
	fprintf (stderr, "main: took the lock...\n");
	var[0]=getpid();
	printf("\n  glob: %d\n", var[0]);
	sleep(10);
  	pthread_mutex_unlock (mutex);
	

return 0;
} 
