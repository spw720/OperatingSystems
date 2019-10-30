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
  char *token = NULL;

  input = fopen(argv[1], "r");

  cBuffer = (char *)malloc(bufferSize * sizeof(char));
  if(cBuffer == NULL){printf("Error! Unable to allocate input buffer. \n");exit(1);}

  do {
    inputSize = getline(&cBuffer, &bufferSize, input);

    //Check line for whitspace (hint from piazza)
    int spaces = 0;
    int tokens = 0;
    int arguments = 0;

    for (int i = 0; i < inputSize; i++) {
      if (cBuffer[i] == ' '){spaces += 1;}
    }

    tokens = spaces + 1;
    arguments = tokens - 1;

    char *args[tokens+1];
    args[tokens+1] = NULL;

    int index = 0;

    token = strtok(cBuffer, " \n");

    if (token == NULL){break;}

    while(token != NULL) {

      args[index] = token;
      printf("Args[%d] = (%s)\n", index, token);
      index += 1;

      token = strtok(NULL, " \n");
    }

    pid_t pid = fork();

    //FORK ERROR
    if (pid < 0){
      perror("fork");
      exit(EXIT_FAILURE);
    }
    //CHILD
    else if (pid == 0){
      //free/close bc child process terminates here
      free(cBuffer);
      fclose(input);

      //printf("Fire! %s %s\n", args[0], args[1]);

      if (execvp(args[0], args) < 0){
        printf("*** ERROR: exec failed\n");
        exit(-1);
      }
    }//end of if pid==0

  } while (!feof(input));

  //***TODO
  int status;
  pid_t temp_p;

  while ((temp_p = wait(&status)) > 0){
    //printf("Waiting for children...\n");
    //sleep(1);
  }

  free(cBuffer);
  fclose(input);

  return 0;

}//end of main()
