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

  input = fopen(argv[1], "r");

  do {
    cBuffer = (char *)malloc(bufferSize * sizeof(char));
    inputSize = getline(&cBuffer, &bufferSize, input);
    token = strtok(cBuffer, " ");

    if (token == NULL) {break;}

    while(token != NULL && strcmp(token, "\n")) {
        printf("Token: %s\n", token);
        token = strtok(NULL, " ");
    }

    free(cBuffer);
  } while(1);

  if (cBuffer != NULL){
		free(cBuffer);
	}

  // make two process which run same
  // program after this instruction
  //fork();

  fclose(input);
  return 0;
}
