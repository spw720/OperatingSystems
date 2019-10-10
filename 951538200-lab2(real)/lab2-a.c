/*
* Description: Takes user input and displays it as tokens
*
* Author: Sean Wilson
*
* Date: 10/08/19
*
* Notes:
* 1. Enter 'exit' to exit the program
*/

/*-------------------------Preprocessor Directives---------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*---------------------------------------------------------------------------*/

/*-----------------------------Program Main----------------------------------*/
int main() {
	setbuf(stdout, NULL);

	/* Main Function Variables */
	char *buffer;
  size_t bufsize = 32;
  size_t characters;
	char *name = "exit";

	/* Allocate memory for the input buffer. */
  buffer = (char *)malloc(bufsize * sizeof(char));

  if( buffer == NULL){
  	perror("Unable to allocate buffer");
  	exit(1);
  }

	/*main run loop*/
	while(1) {

		/* Print >>> then get the input string */
  	printf(">>> ");
  	characters = getline(&buffer, &bufsize, stdin);

		/* Tokenize the input string */
		char* token = strtok(buffer, " \n");

		/* If the user entered <exit> then exit the loop */
		if(strncmp(token, name, strlen(name)) == 0 ){
			break;
    }

		printf("\n");
		int toke_id = 0;

		/* Display each token */
		while (token != NULL) {
        	printf("T%d: %s\n", toke_id, token);
        	token = strtok(NULL, " \n");
					toke_id++;
		}//end of while(token!=NULL)

	}//end of while(1)

	/*Free the allocated memory*/
	free(buffer);
	return 1;

}//end of main()

/*-----------------------------Program End-----------------------------------*/
