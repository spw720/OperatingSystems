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

  FILE *input;
  char *cBuffer;
  size_t bufferSize = 2048;
	size_t inputSize;
  char *token;

  //****create array of pid's****
  pid_t my_pids[20];
  int line = 0;

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

    while(token != NULL){// && strcmp(token, "\n")) {

      args[index] = token;
      index += 1;

      token = strtok(NULL, " \n");
    }

    pid_t pid = fork();
    //***TODO
    my_pids[line] = pid;
    line += 1;
    //***TODO

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

      execvp(args[0], args);
      exit(EXIT_FAILURE);
    }
    /*
    else do {
      int status;
      if ((pid = waitpid(pid, &status, WNOHANG)) == -1)
        perror("wait() error");
      else if (pid == 0) {
        printf("child is still running...\n");
        sleep(1);
      }
      else {
        if (WIFEXITED(status))
          printf("child exited with status of %d\n", WEXITSTATUS(status));
        else puts("child did not exit successfully");
      }
    } while (pid == 0);
    */

  } while(!feof(input)); //end of do while(not end of file)

  //***TODO
  int status;
  pid_t temp_p;

  while ((temp_p = wait(&status)) > 0);

  //for (int p = 0; p <= line; p++) {
  //  printf("Waiting for %d\n", my_pids[p]);
  //  temp_p = waitpid(my_pids[p], &status, WNOHANG);
  //}



  free(cBuffer);
  fclose(input);

  return 0;

}//end of main()
