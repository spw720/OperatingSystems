#include<fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  FILE *input;
  char *cBuffer;
  size_t bufferSize = 2048;
	size_t inputSize;
  char *token;

  char *command;
  char comm[6][20];

  int i = 4; while(i >= 0){strcpy(comm[i], "");i -= 1;}
  strcpy(comm[5], NULL);

  input = fopen(argv[1], "r");

  cBuffer = (char *)malloc(bufferSize * sizeof(char));
  if(cBuffer == NULL){
    printf("Error! Unable to allocate input buffer. \n");
	  exit(1);}

  int temp_index = -1;

  do {
    inputSize = getline(&cBuffer, &bufferSize, input);

    token = strtok(cBuffer, " \n");

    if (token == NULL){break;}

    while(token != NULL && strcmp(token, "\n")) {

        if (temp_index == -1){
          command = token;
        }
        else {
          strcpy(comm[temp_index], token);
        }
        temp_index += 1;
        token = strtok(NULL, " \n");
    }

    printf("%s %s %s %s %s %s\n", command, comm[0], comm[1], comm[2], comm[3], comm[4]);

    execvp(command, comm);

    //reset char array
    int j = 4; while(j >= 0){ strcpy(comm[j], ""); j -= 1;}

    temp_index = -1;

  } while(!feof(input)); //end of do while(not end of file)

  free(cBuffer);
  fclose(input);






  return 0;

}//end of main()
