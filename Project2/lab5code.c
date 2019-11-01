#define _GNU_SOURCE

#include<fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

#include <signal.h>

//---------------------------------------------------------------------------

void handler(int signal){

  printf("Child process: <%d> received signal: <%d>\n", getpid(), signal);
  //write(STDOUT_FILENO, "Child received signal!\n", strlen("Child received signal!\n"));
}

//---------------------------------------------------------------------------

void signaler(pid_t arr[], int signal){

  for (int i = 0; i < 5; i++) {
    kill(arr[i], signal);
  }

}//end of signaler

//---------------------------------------------------------------------------

int main() {

  struct sigaction sa;
  sa.sa_handler = handler;
  sigaction(SIGUSR1, &sa, NULL);

  pid_t pid[5];
  //pid_t pid;

  for (size_t i = 0; i < 5; i++) {

    pid[i] = fork();

    if (pid[i] < 0){perror("fork");exit(EXIT_FAILURE);}

    else if (pid[i] == 0){

      sigset_t sigset;
      sigemptyset(&sigset);
      sigaddset(&sigset, SIGUSR1);
      sigprocmask(SIG_BLOCK, &sigset, NULL);

      int sig;
      int result = sigwait(&sigset, &sig);
      if(result == 0){

        while(1) {
          printf("Child Process: %i - Running infinite loop...\n", getpid());
          sleep(1);
        }

      }

    }//end of if pid == 0

  }//end of for(range 5)

  printf("Fire one!\n");
  signaler(pid, SIGUSR1);

  sleep(5);

  printf("Fire two!\n");
  signaler(pid, SIGUSR1);

  sleep(5);

  printf("Fire three!\n");
  signaler(pid, SIGUSR1);

  sleep(5);

  printf("Fire four!\n");
  signaler(pid, SIGUSR1);

  sleep(5);

  printf("Fire SIGINT!\n");
  signaler(pid, SIGINT);

  int status;
  pid_t temp_p;
  while ((temp_p = wait(&status)) > 0){}

  return 0;
}//end of main()
