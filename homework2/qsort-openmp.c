/* gcc -O -fopenmp -o qsort-openmp  qsort-openmp.c
./qsort-openmp lengthOfArray numWorkers*/


#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>

#define MAXSIZE 1000000  /* maximum array size */
#define MAXTHREADS 10

//variables for measuring execution time
double start_time, end_time;

void quicksort(int *array,int start,int end);
int partition(int *array,int start,int end);


//varieble to hold the size of the array
int size, numThreads;
int array[MAXSIZE]; /* matrix */

int main(int argc, char *argv[]){

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numThreads = (argc > 2)? atoi(argv[2]) : MAXTHREADS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numThreads > MAXTHREADS) numThreads = MAXTHREADS;

  //create array with random numbers
  for (int i = 0; i < size; i++) {
    array[i] = rand()%99;
  }

/*printf("\n\nUnsorted array is:  ");
  for (int i = 0; i < size; i++) {
    printf("%d ", array[i]);
  }
  printf("\n");*/

omp_set_num_threads(numThreads);

start_time = omp_get_wtime();

#pragma omp parallel
{
  //we only want to sort the array once, only one thread executes and we do not need synchronization
  #pragma omp single nowait
  {
    quicksort(array, 0, size-1);
  }
}

end_time = omp_get_wtime();

/*printf("\n\nSorted array is:  ");
  for (int i = 0; i < size; i++) {
    printf("%d ", array[i]);
  }
  printf("\n\n");*/

  printf("The execution time is %g sec\n\n", end_time - start_time);

  return 0;
}

//recursive function that sorts a segment of the array, with start and end index
void quicksort(int *array, int start, int end) {

  int index;

  //as long as there is more than one element in the array
  if (start < end) {

    //call to the partition function to divide the array into two sublists
    index = partition(array, start, end);

    //create tasks for threads to sort the two different sublists, one thread per task

        quicksort(array, start, index-1);
  

    #pragma omp task
      {
        quicksort(array, index+1, end);
      }
  }
}

//choose a pivot and divide the list into elements lesser than than the pivot and greater than the pivot
//returns the index of the pivot after the rearrangement
int partition(int *array, int start, int end){

  //variables to hold the pivot and partition elements
  int i, pivot, index, p, q;

  //select the lst element in the list as the pivot
  pivot = array[end];

  //set partition index to the start of the array
  index = start;

  //loop through the segment of the array and partition
  for (i = start; i <= end; i++) {

    //check if an element is less than the pivot
    if (array[i] < pivot) {

      //swap element with element at partition index
      p = array[i];
      array[i] = array[index];
      array[index] = p;

      //increment partition index
      index++;
    }
  }
  //put the pivot between the two sublists of the array
  q = array[index];
  array[index] = array[end];
  array[end] = q;

  //return the partition index which is now the pivot element
  return index;
}
