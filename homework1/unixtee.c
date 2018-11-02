/* gcc unixtee.c -o unixtee -lpthread
      ./unixtee file.txt createNewFileName */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>


FILE *fp1, *fp2;

/*write to file tee.txt*/
void* writeFile(int buffer){
  /*writes a character in the file*/
  fputc(buffer, fp2);
  return NULL;
}

/*write from buffer to standard outputt*/
void* printText(int buffer){
  printf("%c", buffer);
  return NULL;
}

int main(int argc, char *argv[]) {
  if(argc !=3){
    printf("Specify the file names\n");
    exit(0);
  }

  int buffer;
  /*fopen opens a file pointed to in read or write mode*/
  fp1 = fopen(argv[1], "r");
  fp2 = fopen(argv[2], "w");

  pthread_t thread1;
  pthread_t thread2;

  /*feof ests the end-of-file indicator for the given stream
  As long as we are not at the end of the file*/
  while(!feof(fp1)){

    /*Read from the file and save it in the buffer*/
    buffer = fgetc(fp1);

    /*thread to write to the file*/
    pthread_create(&thread1, 0, writeFile, (void*)buffer);

    /*thread to write to standard output*/
    pthread_create(&thread2, 0, printText, (void*)buffer);
  }

  printf("\n");

  fclose(fp1);
  fclose(fp2);
}
