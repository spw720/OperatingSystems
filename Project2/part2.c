#define _GNU_SOURCE

#include<fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

void handler(int signal){

  printf("Child process: <%d> received signal: <%d>\n", getpid(), signal);
  //write(STDOUT_FILENO, "Child received signal!\n", strlen("Child received signal!\n"));
}

//-----------------------------------------------------------------------------

void signaler(pid_t arr[], int signal){

  for (int i = 0; i < 5; i++) {
    kill(arr[i], signal);
  }

}//end of signaler

//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  if (argc == 1){
    printf("Missing input file!\n");
    return 0;
  }

  struct sigaction sa;
  sa.sa_handler = handler;
  sigaction(SIGUSR1, &sa, NULL);

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

  //printf("NumLines: %d\n", num_lines);

  // Close the file
  fclose(input);
  free(cBuffer);

  //***********************

  pid_t pid_array[num_lines];

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
    //printf("Token1: [%s]\n", token);

    while(token != NULL) {

      //printf("1: [%s]\n", token);
      int length = strlen(token);
      if (length > 0 && token[length - 1] == '\n') token[length-1] = '\0';

      args[index] = token;
      //printf("args[%d] = [%s]\n", index, token);
      index += 1;

      token = strtok(NULL, " ");

    }

    child = fork();
    pid_array[line] = child;

    if (child < 0){
      perror("fork");
      exit(EXIT_FAILURE);
    }
    //CHILD
    else if (child == 0){
      //free/close bc child process terminates here?
      //free(buffy);
      //fclose(fp);

      sigset_t sigset;
      sigemptyset(&sigset);
      sigaddset(&sigset, SIGUSR1);
      sigprocmask(SIG_BLOCK, &sigset, NULL);
      int sig;
      int result = sigwait(&sigset, &sig);

      while(result != 0){
        printf("Waiting on SIGUSR1 for pid[%d]\n", getpid());
        result = sigwait(&sigset, &sig);
        sleep(1);
      }

      printf("Executing pid[%d]\n", getpid());

      if (execvp(args[0], args) < 0){
        perror("Exec");
        exit(-1);
      }

    }//end of if pid==0

  }//end of for num lines

  // signaler(pid_array, SIGUSR1);
  // sleep(5);
  // signaler(pid_array, SIGUSR1);

  for (size_t i = 0; i < num_lines; i++) {
    printf("Fire 1: Sending SIGUSR1 to pid[%d]\n", pid_array[i]);
    kill(pid_array[i], SIGUSR1);
  }

  sleep(5);

  for (size_t j = 0; j < num_lines; j++) {
    printf("Fire 2: Sending SIGUSR1 to pid[%d]\n", pid_array[j]);
    kill(pid_array[j], SIGUSR1);
  }

  sleep(2);

  for (size_t i = 0; i < num_lines; i++) {
    printf("Fire 3: Sending SIGSTOP to pid[%d]\n", pid_array[i]);
    kill(pid_array[i], SIGSTOP);
  }

  sleep(1);
  for (size_t i = 0; i < num_lines; i++) {
    printf("\npid[%d] paused execution...\n\n", pid_array[i]);
  }
  sleep(1);

  for (size_t j = 0; j < num_lines; j++) {
    printf("Fire 4: Sending SIGCONT to pid[%d]\n", pid_array[j]);
    kill(pid_array[j], SIGCONT);
  }

  int status;
  pid_t temp_p;

  //May need to change to waitpid(...,...,0)
  while ((temp_p = wait(&status)) > 0){
    printf("Waiting for children...\n");
    sleep(1);
  }

  free(buffy);
  fclose(fp);

  return 0;
}//end of main()

  //************************************************************
