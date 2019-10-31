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

void handler(int signal, pid_t pid){]
  if (signal == SIGUSR1){
    printf("Child process: <%d> received signal: <%d>\n", pid, signal);
    //sigwait()?
  }
}

void signaler(pid_t arr[5]){
  for (int i = 0; i < 5; i++) {
    kill(arr[i], SIGUSR1);
  }
  for (int x = 0; x < 5; x++) {
    kill(arr[x], SIGUSR1);
  }
  for (int j = 0; j < 5; j++) {
    kill(arr[j], SIGINT);
  }
}

int main(int argc, char *argv[]) {

  pid_t pid[5];

  for (size_t i = 0; i < 5; i++) {

    pid[i] = fork();

    if (pid[i] < 0){perror("fork");exit(EXIT_FAILURE);}

    else if (pid[i] == 0){

      signal(SIGUSR1, handler);

      while(1) {
  			i++;
  			if(i%10000) {
  			     printf("	Child Process: %i - Running infinite loop...\n", getpid());
             i=0
  			}
  		}//end of inf loop

    }//end of if pid == 0

  }//end of for(range 5)

  signaler(pid);

  int status;
  pid_t temp_p;
  while ((temp_p = wait(&status)) > 0){}

  return 0;
}//end of main()
