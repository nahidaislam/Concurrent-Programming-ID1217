/* gcc qsort.c -o qsort -lpthread
      ./qsort lengthOfArray numWorkers*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>
#define MAXSIZE 2000

struct parameters{
  int *array;
  int first;
  int last;
};

void quicksort(int *array, int first, int last);
int partition(int *array, int first, int last);
void* qSortThread(void *init);
int size;
int array[MAXSIZE];
double start_time, end_time; /* start and end times */
double read_timer();


int partition(int *array, int first, int last){

  int pivot, index, p, q;

  pivot = array[last];
  /*set index to first*/
  index = first;

  for(int i = first; i < last; i++){
    /*if element i is less than pivot*/
    if(array[i] <= pivot){
      /*swap element i with element at partition index*/
      p = array[i];
      array[i] = array[index];
      array[index] = p;
      /*increament partition index*/
      index++;
    }
  }
/*put the pivot between the to sublists of the array
  swap(array[pIndex], array[end])*/
  q = array[index];
  array[index] = array[last];
  array[last] = q;

  return index;
}

void quicksort(int *array, int first, int last){
  int j;

  if(first < last)
  {

    j = partition(array, first, last);

    struct parameters arg1 = {array, first, j-1};
    struct parameters arg2 = {array, j+1, last};

    /*two threads to sort the sublists
    skapar för många trådar för onödan*/
    pthread_t thread1;
    pthread_t thread2;
    pthread_create(&thread1, 0, qSortThread, &arg1);
    pthread_create(&thread2, 0, qSortThread, &arg2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
  }
}

/*function that takes and returns a void pointer*/
void* qSortThread(void *init){

  struct parameters *params = init;
  quicksort(params->array, params->first, params->last);
  return NULL;
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

int main(int argc, char *argv[]){
  /* read command line args if any */
  //atoi converts string argument to an integer
  //if argc > 1 -> atoi, else -> MAXSIZE (sets )size
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  //size cannot be larger than the MAXSIZE
  if (size > MAXSIZE)
  size = MAXSIZE;

  /*to get different values*/
  srand(time(NULL));

  //create array with random numbers
  for (int i = 0; i < size; i++) {
    array[i] = rand()%99;
  }

  printf("\n\nUnsorted array is:  ");
  for (int i = 0; i < size; i++) {
    printf("%d ", array[i]);
  }

  printf("\n\n");
  start_time = read_timer();

  quicksort(array, 0, size-1);

  end_time = read_timer();

  printf("Sorted array is: ");
  for(int i = 0; i < size; i++)
    printf("%d ", array[i]);

  printf("\n\n");

  printf("The execution time is %g sec\n", end_time - start_time);
  printf("\n\n");

  return 0;
}
