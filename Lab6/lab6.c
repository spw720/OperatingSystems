/*-----------------------------------------------------------------------------
File name: Lab5.c
Description: A simple lab showing the use of signals.
Author: Jared Hall
-----------------------------------------------------------------------------*/

/*---------------------------Preprocessor Directives-------------------------*/
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
/*---------------------------------------------------------------------------*/

/*---------------------------------Functions---------------------------------*/
/*---------------------------------------------------------------------------*/

/*--------------------------------Program Main-------------------------------*/
int main(void)
{
  //variable declarations
  pid_t pid, w;
  int wstatus, eStatus;

  //create a child process
  pid = fork();
  if(pid < 0) {
    perror("Error! could not create a new process.\n");
    exit(EXIT_FAILURE);
  }
  else if(pid == 0) {
    //This code runs in the child process only
    printf("Child process: %d - Starting...\n", getpid());

    //Add your while loop here
    int time = 1;
    while(time <= 10){
      sleep(1);
      printf("Child process <%d>- Still alive after for <%d> seconds\n", getpid(), time);
      time += 1;
    }

    exit(EXIT_SUCCESS);

  }
  else if(pid > 0) {
    //This code runs in the parent process only
    printf("Parent process: %d - Sending signals to child...\n", getpid());
    //Add code to send your signals  in a loop here

    sleep(1);

    while (w = waitpid(pid, &wstatus, WNOHANG) == 0){
      printf("SIGSTOP\n");
      kill(pid, SIGSTOP);
      sleep(3);
      printf("SIGCONT\n");
      kill(pid, SIGCONT);
      sleep(1);
    }

  }

  //exit out of program
  return EXIT_SUCCESS;
}
/*----------------------------------------------------------------------------*/
