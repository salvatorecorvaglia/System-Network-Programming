#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <pthread.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>

// The program checks whether a mutex can be shared among processes

int main(int argc, const char* argv[]) 
{	

	int    ShmID;
    char    *var;
    int    status;
    
    
    ShmID = shmget(IPC_PRIVATE, 4*sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }
     
	var = (char *) shmat(ShmID, NULL, 0);
    if ((int) var == -1) {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
	void* mmap_ptr = mmap (NULL, sizeof (pthread_mutex_t), 
                       PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
	if (mmap_ptr == MAP_FAILED) {
  		perror ("mmap failed");
  	return -1;
	}

	fprintf (stderr, "mmaped at: %p\n", mmap_ptr);

	pthread_mutex_t* mutp = (pthread_mutex_t*)mmap_ptr;

// initialize the attribute
	pthread_mutexattr_t attr;
	pthread_mutexattr_init (&attr);
	pthread_mutexattr_setpshared (&attr, PTHREAD_PROCESS_SHARED); // this is what we're testing

// initialize the mutex
	pthread_mutex_init (mutp, &attr);
	pthread_mutexattr_destroy (&attr);

// acquire the lock before fork
	pthread_mutex_lock (mutp);
	fprintf (stderr, "main: took the lock...\n");

	pid_t chld = fork ();
	if (chld != 0) { // parent
  		sprintf(var,"Parent");
  		sleep(1);
  		printf("\n La variabile vale: %s\n", var);
 	 	fprintf (stderr, "parent: released the lock.\n");
  		pthread_mutex_unlock (mutp);
	} else { // child
  		fprintf (stderr, "child: going to acquire the lock ...\n");
  		pthread_mutex_lock (mutp);
  		fprintf (stderr, "child: acquired the lock.\n");
  		sprintf(var,"Child");
  		printf("\n La variabile vale: %s\n", var);
  		
  		
  		fprintf (stderr, "child: released the lock.\n");
  		pthread_mutex_unlock (mutp);
}
return 0;
} 
