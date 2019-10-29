#include<fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  FILE *input;
  char *cBuffer;
  size_t bufferSize;
	size_t inputSize;
  char *token;

  input = fopen(argv[1], "r");

  cBuffer = (char *)malloc(bufferSize * sizeof(char));
  if( cBuffer == NULL){
    printf("Error! Unable to allocate input buffer. \n");
	  exit(1);}

  do {
    inputSize = getline(&cBuffer, &bufferSize, input);

    token = strtok(cBuffer, " ");

    if (token == NULL){break;}

    while(token != NULL) {
        printf("Token: %s\n", token);
        token = strtok(NULL, " ");
    }
  } while(1);

  free(cBuffer);
  fclose(input);
  return 0;

}//end of main()
