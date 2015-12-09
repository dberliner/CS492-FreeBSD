/* Should induce high memory conditions */

#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#define SIGTEST 44

volatile int pausevar = 0;
sem_t sem;

/* Handle the memory signal */
void receiveData(int n, siginfo_t *info, void *unused) {
	printf("Recived Signal 44... I'll lower memory usage.\n");
	sem_wait (&sem);
	pausevar=1;
	sem_post (&sem);
}

int main(int argc, char **argv){
	//Init the semaphore
    sem_init(&sem,1,1) ;

    //Assign the handler for SIGTEST
	struct sigaction sig;
	sig.sa_sigaction = receiveData;
	sig.sa_flags = SA_SIGINFO;
	sigaction(SIGTEST, &sig, NULL);

	//Get ready to randomly access a large block of memory
	srand(time(NULL));
	int *mem;
	int memsize=1024;
	mem=malloc(memsize);

	//Endlessly write to memory. 
	while(1){
		mem=realloc(mem,memsize=memsize+1024*1024*10);
		//If we have been told to lower consumption
		if(pausevar){
			mem = realloc(mem,memsize = memsize/8);
			sem_wait (&sem);
			pausevar=0;
			sem_post (&sem);
		}
		sleep(1);
	}
	return 0;
}