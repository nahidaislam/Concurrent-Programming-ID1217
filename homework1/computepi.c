/* gcc computepi.c -lm -o computepi -lpthread
      ./computepi argument*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>
#define EPSILON 0.00000000001

struct parameters{
  double left, right,fleft, fright, area, result;

};

int numThreads;
void* piThread(void *init);
double f(double x);
double read_timer();
double start_time, end_time; /* start and end times */

/*calculate area of upper right quadrant*/
double quad(double left, double right, double fleft, double fright, double area){

  double m, fm, leftarea, rightarea;
  /*middle point*/
  m = (left + right)/2;
  /*y- value of middle point*/
  fm  = f(m);
  /*left area*/
  leftarea = ((fleft + fm) * (m-left))/2;
  /*right area*/
  rightarea = ((fm + fright) * (right-m))/2;

  if (fabs((leftarea + rightarea) - area) > EPSILON){

    struct parameters arg1 = {left, m, fleft, fm, leftarea, 0.0};

    if (numThreads -1 > 0){
      pthread_t thread1;
      /*creates a thread which calculate leftarea*/
      pthread_create(&thread1, 0, piThread, &arg1);

      rightarea = quad(m, right, fm, fright, rightarea);

      /*waits for the thread to be done*/
      pthread_join(thread1, NULL);

      /*captures the result from the thread*/
      leftarea = arg1.result;

    }
    else{

      leftarea = quad(left, m, fleft, fm, leftarea);
      rightarea = quad(m, right, fm, fright, rightarea);
    }
  }
    return (leftarea + rightarea);
}

/*calculates a point on the unit circle*/
double f(double x){
  /*Pythagorean theorem*/
  return sqrt(1- pow(x, 2));
}

/*function that takes and returns a void pointer*/
void* piThread(void *init){

  struct parameters *params = init;
  params->result = quad(params ->left, params->right, params->fleft, params->fright, params->area);
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

int main(int argc, char *argv[]) {

  if (argc != 2){
    printf("State an argument\n");
    return 1;
  }
  numThreads = atoi(argv[1]);

  start_time = read_timer();

  double pi = 4 * quad(0, 1, f(0), f(1), ((f(1) + f(0))*(1-0))/2);

  end_time = read_timer();

  /*10 decimals prints out*/
  printf("\n\nPi is : %.10f\n",pi);

  printf("\n\nThe execution time is %g sec\n", end_time - start_time);
  printf("\n\n");

  return 0;
}
