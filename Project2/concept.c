#define _GNU_SOURCE

#include<fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>


int int main() {

  pid_t array[5];

  char *args0[] = {"ls", "-a", "-r", "-s", NULL};
  char *args1[] = {"sleep", "1", NULL};
  char *args2[] = {"invalid", "name", NULL};
  char *args3[] = {"./iobound", "-minutes", "1", NULL};
  char *args4[] = {"./cpubound", "-minutes", "1", NULL};

  for (int i = 0; i < 5; i++) {

    array[i] = fork();

    if (array[i] < 0){
      printf("FORK ERROR\n");
    }
    if (array[i] == 0){

      if(i == 0){
        execvp(args0[0], args0);
        exit(-1);
      }
      else if(i == 1){
        execvp(args1[0], args1);
        exit(-1);
      }
      else if(i == 2){
        execvp(args2[0], args2);
        exit(-1);
      }
      else if(i == 3){
        execvp(args3[0], args3);
        exit(-1);
      }
      else if(i == 4){
        execvp(args4[0], args4);
        exit(-1);
      }

    }//end of if pid==0

  }//end of for(numprograms)

  int status;
  pid_t temp_p;

  while ((temp_p = wait(&status)) > 0){
    //printf("Waiting for children...\n");
    //sleep(1);
  }



  return 0;
}
