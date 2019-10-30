#define _GNU_SOURCE

#include<fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

//Dynamic Array
void push(int *arr, int index, pid_t value, int *size, int *capacity){
     if(*size > *capacity){
          realloc(arr, sizeof(arr) * 2);
          *capacity = sizeof(arr) * 2;
     }
     arr[index] = value;
     *size = *size + 1;
}
//Dynamic Array

int main(int argc, char *argv[]) {

  //Dynamic Array
  int size = 0;
  int capacity = 2;
  int* arr = malloc(2 * sizeof(pid_t));
  //Dynamic Array

  FILE *input;
  char *cBuffer;
  size_t bufferSize = 2048;
	size_t inputSize;
  char *token;

  //Dynamic Array
  int lines = 0;
  //Dynamic Array

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

    //char *args[]={command, comm[0], comm[1], comm[2], NULL};
    char *args[tokens+1];
    args[tokens+1] = NULL;

    int index = 0;

    token = strtok(cBuffer, " \n");

    if (token == NULL){break;}

    while(token != NULL && strcmp(token, "\n")) {

      args[index] = token;
      index += 1;

      token = strtok(NULL, " \n");
    }

    pid_t pid = fork();

    //Dynamic Array
    push(arr, lines, pid, &size, &capacity);
    lines += 1;
    //Dynamic Array

    //FORK ERROR
    if (pid < 0){
      perror("fork");
      exit(EXIT_FAILURE);
    }
    //CHILD
    if (pid == 0){
      //free/close bc child process terminates here
      free(cBuffer);

      //Dynamic Array
      free(arr);
      //Dynamic Array

      fclose(input);

      printf("CHILD\n");
      execvp(args[0], args);
      exit(-1);
    }

  } while(!feof(input)); //end of do while(not end of file)


  //Dynamic Array
  int status;
  for (int proc = 0; proc < capacity; proc++) {
    waitpid(arr[proc], &status, 0);
    printf("Done waiting\n");
  }


  free(arr);
  //Dynamic Array

  free(cBuffer);
  fclose(input);

  return 0;

}//end of main()
