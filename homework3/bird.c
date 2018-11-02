/*(one producer - multiple consumers)
Semaphores does not have a queue, so there is no fair share, no ordering, the OS decides the order
Semaphores do not make any guarantee of order. They don't have to actually use a queue or other FIFO structure
usage under Linux:
	gcc bird.c -o bird -lpthread
	./bird numBirds*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#define WORM 7
#define NUM_BIRDS 5

sem_t empty;
sem_t wake;
sem_t mutex;

int worms = WORM;
int numBirds;

void *parent_bird(void *arg){
	while(1){
		//parent waits for the bowl to be empty
		sem_wait(&empty);
		//sem_wait(&mutex);
		printf(" Parent leaves to find worms\n");
		//parent found a random worm
		worms = 1 + rand()%WORM;
		printf(" Parent found %d worms\n\n", worms);
		sem_post(&mutex);

		//wake as manybirds as the number of worms found
		//for (int i = 0; i < worms; i++) {
			//sem_post(&wake);
	//	}
	}
}

void *baby_bird(void *arg){
	while(1){

		sleep(1);
		//the birds wait to wake up till parent fills the bowl with worm
		//sem_wait(&wake);

		//Grab a lock so that only one bird can eat at a time.
		sem_wait(&mutex);
		worms--;
		if (worms > 0) {
			printf(" -Baby bird %d ate worm and fell asleep, %d worms left in the bowl\n", (int)arg, worms);
			//Release the lock so that the other birds can eat too
			sem_post(&mutex);
			usleep(1000 * 300);
		}
		else{
			printf(" -Baby bird %d ate the last worm and woke up their parent\n\n", (int)arg);
			usleep(1000*300);
			//release the lock
			//sem_post(&mutex);
			//send signal to parent that the bowl is empty
			sem_post(&empty);

		}
	}
}

int main(int argc, char* argv[]){

	numBirds = (argc > 1)? atoi(argv[1]) : NUM_BIRDS;
	if (numBirds > NUM_BIRDS) numBirds = NUM_BIRDS;

	sem_init(&empty, 0, 0);
	sem_init(&wake, 0, WORM);
	sem_init(&mutex, 0, 1);

	pthread_t parent;
	//creates numBirds pieces of threads in an array
	pthread_t baby[numBirds];

	pthread_create(&parent, NULL, (void *)parent_bird, NULL);

	printf("\n\n");

	for (int i = 0; i < numBirds; i++) {
		pthread_create(&baby[i], NULL, (void *)baby_bird,(void *)i);
	}

	pthread_join(parent, NULL);
	for (int j = 0; j < numBirds; j++) {
		pthread_join(baby[j], NULL);
	}
	return 0;
}
