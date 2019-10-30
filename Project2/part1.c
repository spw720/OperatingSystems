#include<fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

int main(int argc, char *argv[]) {

  FILE *input;
  char *cBuffer;
  size_t bufferSize = 2048;
	size_t inputSize;
  char *token;

  input = fopen(argv[1], "r");

  cBuffer = (char *)malloc(bufferSize * sizeof(char));
  if(cBuffer == NULL){printf("Error! Unable to allocate input buffer. \n");exit(1);}

  do {
    inputSize = getline(&cBuffer, &bufferSize, input);

    //Check line for whitspace (hint from piazza)
    int spaces = 0;
    int tokens = 0;
    int arguments = 0;

    for (int i = 0; i < inputSize; i++) {
      if (cBuffer[i] == ' '){spaces += 1;}
    }

    tokens = spaces + 1;
    arguments = tokens - 1;

    //char *args[]={command, comm[0], comm[1], comm[2], NULL};
    char *args[tokens+1];
    args[tokens+1] = NULL;

    int index = 0;

    token = strtok(cBuffer, " \n");

    if (token == NULL){break;}

    while(token != NULL && strcmp(token, "\n")) {

      args[index] = token;
      index += 1;

      token = strtok(NULL, " \n");
    }

    printf("ARGS: ");
    for (int i = 0; i < tokens; i++) {
      printf("%s ", args[i]);
    }
    printf("\n");

    //char *args[]={command, comm[0], comm[1], comm[2], NULL};
    //execvp(args[0], args);

  } while(!feof(input)); //end of do while(not end of file)

  free(cBuffer);
  fclose(input);






  return 0;

}//end of main()
