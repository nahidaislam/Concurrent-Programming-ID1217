/*(multiple producers - one consumer)
Semaphores does not have a queue, so there is no fair share, no ordering, the OS decides the order
Semaphores do not make any guarantee of order. They don't have to actually use a queue or other FIFO structure.
usage under Linux:
	gcc honey.c -o honey -lpthread
	./honey numBees*/

#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

#define n 10
#define portion 7

sem_t full;
sem_t empty;
sem_t mutex;

int numBees;
int honeyPortion = 0;


void *bear (void *arg){
  while(1){
    //the bear wait to wake up till bees fill the bowl with honey
    sem_wait(&full);

    printf(" Bear wakes up\n");

    while(honeyPortion > 0){

      honeyPortion--;
      printf(" -Bear eats honey, %d unit of honey is left\n", honeyPortion);

      usleep(1000 * 300);
    }

    printf(" -Bear finished all the honey and went to sleep\n\n");

    //till the bowl is empty
    for(int i = 0; i < portion; ++i){
      sem_post(&empty);
    }
  }
}

void *bee(void *arg){
  while(1){

    usleep(1000*300);
    //wait to bowl become empty
    sem_wait(&empty);

    //lock so that only one bee can fill the bowl at a time
    sem_wait(&mutex);

    honeyPortion++;

    if(honeyPortion == portion){
      printf("*Bee %d fills the bowl and wake up the bear*\n\n", (int)arg);
      //Release the lock
      sem_post(&mutex);
      //wake the bear up
      sem_post(&full);
    }
    else{
      printf("-Bee %d puts honey in the bowl, %d units of honey in bowl\n", (int)arg, honeyPortion);

      usleep(1000*300);
      //release lock to let another bee to fill the bowl
      sem_post(&mutex);

      usleep(1000*300);
    }
  }
}
int main(int argc, char *argv[]){

  numBees = (argc > 1)? atoi(argv[1]) : n;
  if (numBees > n) numBees = n;

  sem_init(&empty, 0, portion);
  sem_init(&full, 0, 0);
  sem_init(&mutex, 0, 1);

  pthread_t bear_thread;
  pthread_t bee_thread[numBees];

  pthread_create(&bear_thread, NULL, (void *)bear, NULL);

  for (int i = 0; i < numBees; i++) {
    pthread_create(&bee_thread[i], NULL, (void *)bee, (void *)i);
  }

  pthread_join(bear_thread, NULL);
  for (int j = 0; j < numBees; j++) {
    pthread_join(bee_thread[j], NULL);
  }

  return 0;
}
