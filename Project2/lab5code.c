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

  printf("Child process: <%d> received signal: <%d>\n", getpid(), signal);

}

void signaler(pid_t arr[]){

  for (int i = 0; i < 5; i++) {
    kill(arr[i], SIGUSR1);
  }
  for (int x = 0; x < 5; x++) {
    kill(arr[x], SIGUSR1);
  }
  for (int j = 0; j < 5; j++) {
    kill(arr[j], SIGINT);
  }

}//end of signaler


int main() {

  signal(SIGUSR1, handler);

  pid_t pid[5];
  //pid_t pid;

  for (size_t i = 0; i < 5; i++) {

    pid[i] = fork();
    //kill(pid[i], SIGUSR1);

    if (pid[i] < 0){perror("fork");exit(EXIT_FAILURE);}

    else if (pid[i] == 0){

      while(1) {
  			printf("	Child Process: %i - Running infinite loop...\n", getpid());
        sleep(1);
  		}

    }//end of if pid == 0

  }//end of for(range 5)

  sleep(5);
  printf("Parent executing signaler\n");
  signaler(pid);

  int status;
  pid_t temp_p;
  while ((temp_p = wait(&status)) > 0){}

  return 0;
}//end of main()
