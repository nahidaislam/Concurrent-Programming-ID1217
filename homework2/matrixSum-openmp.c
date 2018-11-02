/* matrix summation using OpenMP
   usage with gcc (version 4.2 or higher required):
       gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c
     ./matrixSum-openmp size numWorkers

*/
#include <omp.h>

double start_time, end_time;

#include <stdio.h>
#include <stdlib.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */
//#define DEBUG

int numWorkers;
int size;
int matrix[MAXSIZE][MAXSIZE];
void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j, max, min, maxX, maxY, minX, minY, total=0;

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  omp_set_num_threads(numWorkers);

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
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

  min = max = matrix[0][0];

  start_time = omp_get_wtime();
  //i is private automatically as we use an omp parallel for
#pragma omp parallel for reduction (+:total) private(j)
    for (i = 0; i < size; i++){
      for (j = 0; j < size; j++){
        total += matrix[i][j];

        /*Check of statement is true outside the loop so that we don't enter the critical section
        when we don't have to. This increases performance considerably.*/
        if (matrix[i][j] < min) {
          //Specifies that code is only be executed on one thread at a time.
          #pragma omp critical
          {
            if (matrix[i][j] < min) {
              min = matrix[i][j];
              minX= j;
              minY = i;
            }
          }
        }
        if (matrix[i][j] > max){
          #pragma omp critical
          {
            if (matrix[i][j] > max) {
              max = matrix[i][j];
              maxX = j;
              maxY = i;
            }
          }
        }
      }
    }

// implicit barrier

  end_time = omp_get_wtime();
  printf("\n");
    /* print results */
  printf("The total is %d\n", total);
  printf("The minimum is %d, x = %d, y = %d\n", min, minX, minY);
  printf("The minimum is %d, x = %d, y = %d\n", max, maxX, maxY);
  printf("The execution time is %g sec\n", end_time - start_time);
  printf("\n");
}
