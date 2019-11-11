#define _GNU_SOURCE

#include<fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

int pid_pool[10] = {0,0,0,0,0,0,0,0,0,0};
int been_caught[10] = {0,0,0,0,0,0,0,0,0,0};

int pool_index = 0;
int running_child = 0;

//-----------------------------------------------------------------------------

void alarm_handler(int signal){

  printf("Alarm signal received, stopping all children...\n");

  for (size_t i = 0; i < pool_index; i++) {
    printf("SIGSTOP: stopping child[%d]\n", pid_pool[i]);
    kill(pid_pool[i], SIGSTOP);
  }

  int flag_boi = pool_index;
  while (flag_boi > 0){

    if(running_child + 1 >= pool_index ){
      running_child = 0;
    }else{
      running_child += 1;
    }

    pid_t w;
    int wstatus;
    if (w = waitpid(pid_pool[running_child], &wstatus, WNOHANG) != 0){
      printf("Cannot SIGCONT, process [%d] exited\n", pid_pool[running_child]);
    }
    else {
      printf("SIGCONT: continuing child[%d]\n", pid_pool[running_child]);
      kill(pid_pool[running_child], SIGCONT);
      sleep(4);
    }

    if (w = waitpid(pid_pool[running_child], &wstatus, WNOHANG) != 0){
      printf("Cannot SIGSTOP, process [%d] exited\n", pid_pool[running_child]);

      if(been_caught[running_child] == 0){
        flag_boi -= 1;
        been_caught[running_child] = 1;
      }

    }
    else {
      printf("SIGSTOP: stopping child[%d]\n", pid_pool[running_child]);
      kill(pid_pool[running_child], SIGSTOP);
    }

  }//end of while()

  printf("End of alarm_handler\n");

}//end of alarm_handler()

//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  // TODO - Put alarm signal handler here???
  signal(SIGALRM, alarm_handler);
  // TODO - Put alarm signal handler here???

  FILE *input;
  char *cBuffer;
  size_t bufferSize = 2048;
	size_t inputSize;

  int num_lines = 0;

  if (argc == 1){
    printf("Missing input file!\n");
    return 0;
  }

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

    while(token != NULL) {

      int length = strlen(token);
      if (length > 0 && token[length - 1] == '\n') token[length-1] = '\0';

      args[index] = token;
      index += 1;

      token = strtok(NULL, " ");

    }//end of while(token!=null)

    child = fork();
    pid_array[line] = child;

    //set global pool
    pid_pool[pool_index] = child;
    pool_index += 1;
    //set global pool

    if (child < 0){
      perror("fork");
      exit(EXIT_FAILURE);
    }
    //CHILD
    else if (child == 0){

      if (execvp(args[0], args) < 0){
        perror("Exec");
        exit(-1);
      }

    }//end of if pid==0

  }//end of for num lines

  alarm(1);

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
