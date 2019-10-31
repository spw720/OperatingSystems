#define _GNU_SOURCE

#include<fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>


int main() {

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
      /*
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
      */

      if(i == 1){
        printf("Fire: sleep\n");
        execvp(args9[0], args9);
        printf("ERROR: sleep\n");
        exit(-1);
      }
      else if(i == 2){
        printf("Fire: sleep\n");
        execvp(args8[0], args8);
        printf("ERROR: sleep\n");
        exit(-1);
      }
      else if(i == 3){
        printf("Fire: sleep\n");
        execvp(args7[0], args7);
        printf("ERROR: sleep\n");
        exit(-1);
      }
      else if(i == 4){
        printf("Fire: sleep\n");
        execvp(args7[0], args7);
        printf("ERROR: sleep\n");
        exit(-1);
      }
      else if(i == 5){
        printf("Fire: sleep\n");
        execvp(args7[0], args7);
        printf("ERROR: sleep\n");
        exit(-1);
      }

    }//end of if pid==0

  }//end of for(numprograms)

  int status;
  pid_t temp_p;

  //while ((temp_p = wait(&status)) > 0){}

  for (int j = 0; j < 5; j++) {
    waitpid(array[j], &status, 0);
  }

  return 0;
}
