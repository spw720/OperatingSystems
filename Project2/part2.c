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

int main(int argc, char *argv[]) {

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

  printf("NumLines: %d\n", num_lines);

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
      printf("args[%d] = [%s]\n", index, token);
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
      //free/close bc child process terminates here
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

      printf("Executing\n");

      if (execvp(args[0], args) < 0){
        perror("Exec");
        exit(-1);
      }

    }//end of if pid==0

  }//end of for num lines

  for (size_t i = 0; i < num_lines; i++) {
    printf("1: Sending SIGUSR1 to pid[%d]\n", pid_array[i]);
    kill(pid_array[i], SIGUSR1);
  }
  sleep(5);
  for (size_t j = 0; j < num_lines; j++) {
    printf("2: Sending SIGUSR1 to pid[%d]\n", pid_array[j]);
    kill(pid_array[j], SIGUSR1);
  }

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
