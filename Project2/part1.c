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
  char comm[5][20];
  for (int i = 0; i < 5; i++) {
    strcpy(comm[i], "");}

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

    for (int i = 0; i < 5; i++) {
      strcpy(comm[i], "");}

    temp_index = -1;

  } while(!feof(input));

  free(cBuffer);
  fclose(input);






  return 0;

}//end of main()
