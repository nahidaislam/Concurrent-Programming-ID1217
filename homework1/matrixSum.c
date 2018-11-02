/* matrix summation using pthreads

   features: uses a barrier; the Worker[0] computes
             the total sum from partial sums computed by Workers
             and prints the total sum to the standard output

   usage under Linux:
     gcc matrixSum.c -lpthread
     ./a.out size numWorkers

*/
#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */
#define DEBUG


pthread_mutex_t barrier;  /* mutex lock for the barrier */
pthread_cond_t go;        /* condition variable for leaving */
int numWorkers;           /* number of workers */
int numArrived = 0;       /* number who have arrived */


/* a reusable counter barrier */
void Barrier() {
  /*initierar ett lås*/
  pthread_mutex_lock(&barrier);
  numArrived++;
  if (numArrived == numWorkers) {
    numArrived = 0;
    /*väcker alla andra trådar*/
    pthread_cond_broadcast(&go);
  } else
  /*trådar sover tills den sista tråden kommer*/
  pthread_cond_wait(&go, &barrier);
  pthread_mutex_unlock(&barrier);
}

/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

double start_time, end_time; /* start and end times */
int size, stripSize;  /* assume size is multiple of numWorkers */
int sums[MAXWORKERS]; /* partial sums */
int matrix[MAXSIZE][MAXSIZE]; /* matrix */

struct maxminpos{
  int value;
  int xpos;
  int ypos;
};

struct maxminpos min[MAXWORKERS];
struct maxminpos max[MAXWORKERS];

void *Worker(void *);

/* read command line, initialize, and create threads */
/*argc är antalet argument och argv skapar en array med dessa argument*/
int main(int argc, char *argv[]) {
  int i, j;
  long l; /* use long in case of a 64-bit system */
  pthread_attr_t attr;
  pthread_t workerid[MAXWORKERS];

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* initialize mutex and condition variable */
  pthread_mutex_init(&barrier, NULL);
  pthread_cond_init(&go, NULL);

  /* read command line args if any */
  /*atoi converts a string to an integer*/
  /*x?y:z om x stämmer gör y annars gör z*/
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
  /*delen av matrisen tråden jobbar med*/
  stripSize = size/numWorkers;

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
	  for (j = 0; j < size; j++) {
      /*returnerar en pseudo-random nummer inom 0 till 99.*/
          matrix[i][j] = rand()%99;
	  }
  }

  /* print the matrix */
#ifdef DEBUG
  for (i = 0; i < size; i++) {
	  printf("[ ");
	  for (j = 0; j < size; j++) {
	    printf(" %d", matrix[i][j]);
	  }
	  printf(" ]\n");
  }
#endif

  /* do the parallel work: create the workers */
  start_time = read_timer();
  for (l = 0; l < numWorkers; l++)
  pthread_create(&workerid[l], &attr, Worker, (void *) l);
  pthread_exit(NULL);
}

/* Each worker sums the values in one strip of the matrix.
   After a barrier, worker(0) computes and prints the total */
void *Worker(void *arg) {
  long myid = (long) arg;
  int total, i, j, first, last;

#ifdef DEBUG
  printf("worker %zu(pthread id %zu) has started\n", myid, pthread_self());
#endif

  /* determine first and last rows of my strip */
  first = myid*stripSize;
  last = (myid == numWorkers - 1) ? (size - 1) : (first + stripSize - 1);

  /* sum values in my strip */
  total = 0;
  struct maxminpos minpos;
  struct maxminpos maxpos;
  minpos.value = 99;
  maxpos.value = 0;

  for (i = first; i <= last; i++){
    for (j = 0; j < size; j++){
      total += matrix[i][j];
      if(matrix[i][j]< minpos.value){
        minpos.value = matrix[i][j];
        minpos.xpos = j;
        minpos.ypos = i;
      }
      if(matrix[i][j]> maxpos.value){
        maxpos.value = matrix[i][j];
        maxpos.xpos = j;
        maxpos.ypos = i;
      }
    }
  }
  sums[myid] = total;
  min[myid] = minpos;
  max[myid] = maxpos;

  Barrier();

  if (myid == 0) {
    total = 0;
    for (i = 0; i < numWorkers; i++){
      total += sums[i];
  }
  int minimum = 99;
  int maximum = 0;
  int xposmin;
  int yposmin;
  int xposmax;
  int yposmax;

  for (j= 0; j< numWorkers; j++) {
    if(min[j].value < minimum){
      minimum = min[j].value;
      xposmin = min[j].xpos;
      yposmin = min[j].ypos;
   }
    if (max[j].value > maximum){
      maximum = max[j].value;
      xposmax = max[j].xpos;
      yposmax = max[j].ypos;
    }
  }
    /* get end time */
    end_time = read_timer();
    /* print results */
    printf("The total is %d\n", total);
    printf("The execution time is %g sec\n", end_time - start_time);
    printf("Minimum: %d at position x = %d, y= %d \n", minimum, xposmin, yposmin);
    printf("Maximum: %d at position x = %d, y= %d \n", maximum, xposmax, yposmax);

  }
}
