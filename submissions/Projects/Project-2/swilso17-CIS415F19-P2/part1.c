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

  if (argc == 1){
    printf("Missing input file!\n");
    return 0;
  }

  FILE *input;
  char *cBuffer;
  size_t bufferSize = 2048;
	size_t inputSize;

  int num_lines = 0;

  input = fopen(argv[1], "r");

  cBuffer = (char *)malloc(bufferSize * sizeof(char));
  if(cBuffer == NULL){printf("Error! Unable to allocate input buffer. \n");exit(1);}

  while( (inputSize = getline(&cBuffer, &bufferSize, input) ) != -1){
    num_lines += 1;
  }

  // Close the file
  fclose(input);
  free(cBuffer);

  //***********************

  pid_t array[num_lines];

  FILE *fp;
  char *buffy;
	size_t file_size;

  char *token;

  pid_t child;

  fp = fopen(argv[1], "r");

  buffy = (char *)malloc(bufferSize * sizeof(char));
  if(buffy == NULL){printf("Error! Unable to allocate input buffer. \n");exit(1);}

  for (int line = 0; line < num_lines; line++) {

    int spaces = 0;
    int tokens = 0;
    int arguments = 0;

    file_size = getline(&buffy, &bufferSize, fp);

    for (int i = 0; i < file_size; i++) {
      if (buffy[i] == ' '){spaces += 1;}
    }

    tokens = spaces + 1;
    arguments = tokens - 1;

    char *args[tokens+1];
    args[tokens] = NULL;

    int index = 0;

    token = strtok(buffy, " ");

    while(token != NULL) {

      int length = strlen(token);
      if (length > 0 && token[length - 1] == '\n') token[length-1] = '\0';

      args[index] = token;
      index += 1;

      token = strtok(NULL, " ");

    }

    child = fork();

    if (child < 0){
      perror("fork");
      exit(EXIT_FAILURE);
    }
    //CHILD
    else if (child == 0){
      //free/close bc child process terminates here
      //free(buffy);
      //fclose(fp);

      if (execvp(args[0], args) < 0){
        perror("Exec");
        exit(-1);
      }
    }//end of if pid==0

  }//end of for num lines

  int status;
  pid_t temp_p;

  while ((temp_p = wait(&status)) > 0){
    printf("Waiting for children...\n");
    sleep(1);
  }

  free(buffy);
  fclose(fp);

  return 0;
}//end of main()
