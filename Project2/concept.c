#define _GNU_SOURCE

#include<fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>


int main(int argc, char *argv[]) {

  FILE *input = NULL;
  char *cBuffer = NULL;
  size_t bufferSize = 2048;
	size_t inputSize = 0;

  int num_lines = 0;

  input = fopen(argv[1], "r");

  cBuffer = (char *)malloc(bufferSize * sizeof(char));
  if(cBuffer == NULL){printf("Error! Unable to allocate input buffer. \n");exit(1);}


  while( (inputSize = getline(&cBuffer, &bufferSize, input) ) != -1){
    num_lines += 1;
  }

  printf("NumLines: %d\n", num_lines);

  // Close the file
  fclose(input);
  free(cBuffer);

  //************************************************************
/*
  pid_t array[5];

  char *args0[] = {"ls", "-a", "-r", "-s", NULL};
  char *args1[] = {"sleep", "1", NULL};
  char *args2[] = {"invalid", "name", NULL};
  char *args3[] = {"./iobound", "-minutes", "1", NULL};
  char *args4[] = {"./cpubound", "-minutes", "1", NULL};

  char *args9[] = {"sleep", "5", NULL};
  char *args8[] = {"sleep", "5", NULL};
  char *args7[] = {"sleep", "5", NULL};

  for (int i = 0; i < 5; i++) {

    array[i] = fork();

    if (array[i] < 0){
      printf("FORK ERROR\n");
    }
    if (array[i] == 0){

      if(i == 0){
        printf("Fire: ls\n");
        execvp(args0[0], args0);
        printf("ERROR: ls\n");
        exit(-1);
      }
      else if(i == 1){
        printf("Fire: sleep\n");
        execvp(args1[0], args1);
        printf("ERROR: sleep\n");
        exit(-1);
      }
      else if(i == 2){
        printf("Fire: invalid\n");
        execvp(args2[0], args2);
        printf("ERROR: invalid\n");
        exit(-1);
      }

      else if(i == 3){
        printf("Fire: iobound\n");
        execvp(args3[0], args3);
        printf("ERROR: iobound\n");
        exit(-1);
      }
      else if(i == 4){
        printf("Fire: cpubound\n");
        execvp(args4[0], args4);
        printf("ERROR: cpubound\n");
        exit(-1);
      }

    }//end of if pid==0

  }//end of for(numprograms)

  int status;
  pid_t temp_p;

  while ( (temp_p = wait(&status) ) > 0){}

  //for (int j = 0; j < 5; j++) {
  //  waitpid(array[j], &status, 0);
  //}
*/

  return 0;
}//end of main()
