#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>   
key_t key=12345;   
#define SHM_KEY 0x1234    
#define	BUFFSIZE	4096
#define BUFSIZE 4096           /* Size of transfer buffer */
#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

int semid; 
struct sembuf sb[3];  /* set to allocate resource */ 
union semun arg;


struct shmseg {                 /* Defines structure of shared memory segment */
    int cnt;                    /* Number of bytes used in 'buf' */
    char buf[BUFSIZE];         /* Data being transferred */
};

int main(int argc,char *argv[]){

	int fd[2], n;
	int res;
	pid_t pid, pid2;
	char buf[BUFFSIZE];
	int shmid;
    int *shmp;
	struct timeval start;
	struct timeval end;
	double secs;
	printf("Creating pipe... \n");

	res = pipe(fd);
	if(res != 0)
		perror("Pipe creating...");
	
	
	snprintf(buf, BUFFSIZE, "token");
	
	shmid = shmget(SHM_KEY, sizeof(char), IPC_CREAT | OBJ_PERMS);
    if (shmid == -1)
        printf("shmget");

    shmp = (int *)shmat(shmid, NULL, 0);
    if (shmp == (void *) -1)
        printf("shmat");
	
	shmp[0]=0;
	shmp[1]=0;
	printf("\nVariabile globale: %d\n",shmp[0]);
	
	
	if ((semid = semget(key, 3, 0666 | IPC_CREAT)) == -1) {             
    	perror("semget");             
    	exit(1);         
    } 
	
	arg.val = 1;         
	if (semctl(semid, 0, SETVAL, arg) == -1) {             
		perror("semctl");             
		exit(1);         
	} 
	printf("1");
	
	arg.val = 0;         
	if (semctl(semid, 1, SETVAL, arg) == -1) {             
		perror("semctl");             
		exit(1);         
	}
	printf("2"); 
	arg.val = 0;         
	if (semctl(semid, 2, SETVAL, arg) == -1) {             
		perror("semctl");             
		exit(1);         
	} 
	printf("3");   
	sb[0].sem_num = 0;
  	sb[1].sem_num = 1;
  	sb[2].sem_num = 2;
	
	if((pid = fork()) ==0) {       //child 1
		printf("\n Im child 1 with pid: %d\n", getpid());
		for(;;) {
				sb[1].sem_op = -1;	
				if (semop(semid, &sb[1], 1) == -1) {             
					perror("semop");             
					exit(1);         
    			} 
				shmp[1]++;
				n = read(fd[0], buf,	BUFFSIZE);
				printf("\n control: %d\n",shmp[1]);
				printf("\nchild 1 reads: %s\n",buf);
				shmp[1]--;
				sb[2].sem_op = 1;
				if (semop(semid, &sb[2], 1) == -1) {             
					perror("semop");             
					exit(1);         
				}
				write(fd[1], buf, BUFFSIZE);
				
			
			
		}
	
	
	}//end child 1
	if(pid >1) {            //parent
		if((pid2 = fork()) ==0) {				//child 2
			printf("\n Im  with pid: %d\n", getpid());
			for(;;) {
					sb[2].sem_op = -1;	
					if (semop(semid, &sb[2], 1) == -1) {             
						perror("semop");             
						exit(1);         
    				} 
					shmp[1]++;
					n = read(fd[0], buf,	BUFFSIZE);
					printf("\n control: %d\n",shmp[1]);
					
					printf("\nchild 2 reads: %s\n",buf);
					shmp[1]--;
					shmp[0]=0;
					sb[0].sem_op = 1;
					if (semop(semid, &sb[0], 1) == -1) {             
						perror("semop");             
						exit(1);         
					}
					write(fd[1], buf, BUFFSIZE);
					
				
			
			}
		}//end child 2
		
		
		
		printf("\n Im parent with pid: %d\n", getpid());
		
		shmp[0]=1;
		gettimeofday(&start, NULL);
		write(fd[1], buf, BUFFSIZE);
		for(;;) {
				sb[0].sem_op = -1;	
				if (semop(semid, &sb[0], 1) == -1) {             
					perror("semop");             
					exit(1);         
    			} 
				shmp[1]++;
				n = read(fd[0], buf,	BUFFSIZE);
				printf("\n control: %d\n",shmp[1]);
				
				gettimeofday(&end, NULL);
				secs = (double)(end.tv_usec - start.tv_usec) / 1000000 + (double)(end.tv_sec - start.tv_sec);
				printf("\ntime taken %f seconds\n",secs);
				
				printf("\nparent reads: %s\n",buf);
				shmp[1]--;
				shmp[0]=1;
				sb[1].sem_op = 1;
				if (semop(semid, &sb[1], 1) == -1) {             
					perror("semop");             
					exit(1);         
				}
				write(fd[1], buf, BUFFSIZE);
				gettimeofday(&start, NULL);
			
			
		}
		
		
		
		
	}//end parent

	
	
	exit(0);

}