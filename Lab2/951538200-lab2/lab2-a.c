/*
* Description: <write a brief description of your lab>
*
* Author: Sean Wilson
*
* Date: 10/08/19
*
* Notes:
* 1. <add notes we should consider when grading>
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

	/* Allocate memory for the input buffer. */

	/*main run loop*/
		/* Print >>> then get the input string */

		/* Tokenize the input string */

		/* Display each token */

		/* If the user entered <exit> then exit the loop */


	/*Free the allocated memory*/

	char *buffer;
  size_t bufsize = 32;
  size_t characters;

  buffer = (char *)malloc(bufsize * sizeof(char));

  if( buffer == NULL){
  	perror("Unable to allocate buffer");
  	exit(1);
  }

	int catch = 0;

	while (catch == 0) {

  	printf(">>> ");

  	characters = getline(&buffer, &bufsize, stdin);

		char* token = strtok(buffer, " ");

		if (token == "exit") catch = 1;
		//if(strncmp(token, "exit", 4) == 0){
		//	catch = 1;
		//}

		int toke_id = 0;

		while (token != NULL) {

        	printf("T%d: %s\n", toke_id, token);

					//printf("&token: %c", *token);

        	token = strtok(NULL, " ");
					
					if (token == "exit") catch = 1;
					//if(strncmp(token, "exit", 4) == 0){
					//	catch = 1;}

					toke_id++;
    }
	}
  //printf("%zu characters were read.\n", characters);
  //printf("You typed: '%s'\n", buffer);

	return 1;
}








/*-----------------------------Program End-----------------------------------*/
