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

void handler(int signal){

  pid_t pid = getpid();
  printf("Child process: <%i> received signal: <%d>\n", pid, signal);

  if (signal == 0){
    int sig;
    sigset_t sigset;
    int result = sigwait(&sigset, &sig);
    if(result == 0){
      printf("sigwait got signal: %d\n", sig);
      kill(pid, SIGINT);
    }
  }

  //if signal == 0: sigwait()
  //else do sigaction? With passed in signal??

}

void signaler(pid_t arr){
  kill(arr, SIGUSR1);
  kill(arr, SIGUSR1);
  kill(arr, SIGINT);
  /*
  for (int i = 0; i < 5; i++) {
    kill(arr[i], SIGUSR1);
  }
  for (int x = 0; x < 5; x++) {
    kill(arr[x], SIGUSR1);
  }
  for (int j = 0; j < 5; j++) {
    kill(arr[j], SIGINT);
  }
  */
}//end of signaler


int main(int argc, char *argv[]) {

  signal(SIGUSR1, &handler);

  //pid_t pid[5];
  pid_t pid;

  //for (size_t i = 0; i < 5; i++) {

    pid = fork();
    //kill(pid[i], SIGUSR1);

    if (pid < 0){perror("fork");exit(EXIT_FAILURE);}

    else if (pid == 0){

      kill(pid, SIGUSR1);
      kill(pid, SIGUSR1);

      while(1) {
  			printf("	Child Process: %i - Running infinite loop...\n", getpid());
        sleep(1);
  		}

    }//end of if pid == 0

  //}//end of for(range 5)

  signaler(pid);

  int status;
  pid_t temp_p;
  while ((temp_p = wait(&status)) > 0){}

  return 0;
}//end of main()
