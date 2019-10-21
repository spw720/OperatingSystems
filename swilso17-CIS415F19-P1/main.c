/*
* Description: CLI program that implements ls, pwd, mkdir, cd, mv, cp, rm
*
* Author: Sean Wilson
* Date: 10/15/19
*
* Notes:
* 1.
*/

/*-------------------------Preprocessor Directives---------------------------*/
#include<fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#include "command.h"


/*-----------------------------Program Main----------------------------------*/

int main(int argc, char *argv[]) {
	setbuf(stdout, NULL);

	// TODO Figure this out...Null pointers being freed could cause this error?
	FILE *output = NULL;

	// TODO Figure this out...Null pointers being freed could cause this error?
	FILE *input = NULL;

	/*function vars */

	//TODO set to NULL
	//char *cBuffer = NULL;
	char *cBuffer = NULL;

	size_t bufferSize = 2048;
	size_t inputSize;

	char *token = NULL;//TODO set to NULL;
	char *token2 = NULL;//TODO set to NULL;

	char *token3 = NULL;//TODO set to NULL;

	/* Allocate memory to the input buffer. */

	//cBuffer = (char *)malloc(bufferSize * sizeof(char));


	//if( cBuffer == NULL)
	//{
	//	printf("Error! Unable to allocate input buffer. \n");
	//	exit(1);
	//}

	//File mode check
	if (argc >= 3 && (strncmp(argv[1], "-f", 2) == 0)){

		input = fopen(argv[2], "r");

		output = freopen("output.txt", "w+", stdout);

	}

	if (input == NULL){
		input = stdin;
	}

	/*main run cycle*/
	do {

		if (input == stdin) {
			printf( ">>> ");
		}

//**************
		cBuffer = (char *)malloc(bufferSize * sizeof(char));
		//**************

		inputSize = getline(&cBuffer, &bufferSize, input);

		/* tokenize the input string */
		token = strtok(cBuffer, " ");

		if (token == NULL) {break;}

		/*while token not null and token = newline*/
		while(token != NULL && strcmp(token, "\n")) {

			/*if the command is 'exit then close the program*/
			if(strcmp(token, "exit\n") == 0 || strcmp(token, "exit") == 0) { break; }

/*-----------------------------No-Argument-----------------------------------*/

			if(strcmp(token, "ls\n") == 0) {
				listDir();
			}//end of ls

			else if(strncmp(token, "ls", 2) == 0) {
				token3 = strtok(NULL, " \n");
				if (token3 != NULL){
					if (strcmp(token3, ";") == 0){
						listDir();
					}
					else {
						printf("Error! Incorrect syntax. No control code found.\n");
					}
				}
				else{listDir();}
			}//end of ls



			else if(strcmp(token, "pwd\n") == 0) {
				showCurrentDir();
			}//end of pwd

			else if(strncmp(token, "pwd", 3) == 0) {
				token3 = strtok(NULL, " \n");
				if (token3 != NULL){
					if (strcmp(token3, ";") == 0){
						showCurrentDir();
					}
					else {
						printf("Error! Incorrect syntax. No control code found.\n");
					}
				}
				else{showCurrentDir();}
			}//end of pwd

/*-----------------------------1-Argument-----------------------------------*/

			else if(strncmp(token, "mkdir", 5) == 0) {
				token = strtok(NULL, " \n");
				token3 = strtok(NULL, " \n");
				if (token3 != NULL){
					if (strncmp(token3, ";", 1) == 0){
						makeDir(token);
					}
					else {
						printf("Error! Incorrect syntax. No control code found.\n");
					}
				}
				else {makeDir(token);}
			}//end of mkdir



			else if(strncmp(token, "cd", 2) == 0) {
				token = strtok(NULL, " \n");
				token3 = strtok(NULL, " \n");
				if (token3 != NULL){
					if (strcmp(token3, ";") == 0){
						changeDir(token);
					}
					else {
						printf("Error! Incorrect syntax. No control code found.\n");
					}
				}
				else{changeDir(token);}
			}//end of cd



			else if(strncmp(token, "rm", 2) == 0) {
				token = strtok(NULL, " \n");
				token3 = strtok(NULL, " \n");
				if (token3 != NULL){
					if (strcmp(token3, ";") == 0){
						deleteFile(token);
					}
					else {
						printf("Error! Incorrect syntax. No control code found.\n");
					}
				}
				else{deleteFile(token);}
			}//end of rm



			else if(strncmp(token, "cat", 3) == 0) {
				token = strtok(NULL, " \n");
				token3 = strtok(NULL, " \n");
				if (token3 != NULL){
					if (strcmp(token3, ";") == 0){
						displayFile(token);
					}
					else {
						printf("Error! Incorrect syntax. No control code found.\n");
					}
				}
				else{displayFile(token);}
			}//end of cat

/*-----------------------------2-Argument-----------------------------------*/

			else if(strncmp(token, "cp", 2) == 0) {
				token = strtok(NULL, " ");
				token2 = strtok(NULL, " \n");
				token3 = strtok(NULL, " \n");
				if (token3 != NULL){
					if (strncmp(token3, ";", 1) == 0){}
				}
				if (strncmp(token2, ".", 1) == 0){
					char *temp_token = token + 3;
					copyFile(token, temp_token);
				}
				else{copyFile(token, token2);}
			}//end of cp



			else if(strncmp(token, "mv", 2) == 0) {
				token = strtok(NULL, " ");
				token2 = strtok(NULL, " \n");
				token3 = strtok(NULL, " \n");
				if (token3 != NULL){
					if (strcmp(token3, ";") == 0){
						moveFile(token, token2);
					}
					else {
						printf("Error! Incorrect syntax. No control code found.\n");
					}
				}
				else {moveFile(token, token2);}
			}//end of mv

/*--------------------------------------------------------------------------*/

			else {
				printf("Error: Unrecognized command: %s\n", token);
				break;
			}

			token = strtok(NULL, " "); //TODO set to ;

		}// end of while(token not null)



		if(token != NULL) {
			if(strcmp(token, "exit\n") == 0 || strcmp(token, "exit") == 0) {

				//**************
						free(cBuffer);
				//**************


				break;
			}
		}

		//**************
				free(cBuffer);
		//**************


	} while (1); // end of do-while loop

	/*Free the allocated memory*/


	//**************
	if (cBuffer != NULL){
		free(cBuffer);
	}

//**************


	// TODO Figure this out...Null pointers being freed could cause this error?
	if (input != stdin){
		fclose(input);
	}
	if (output != NULL){
		fclose(output);
	}

	return 0;

}// end of main()

/*-----------------------------Program End-----------------------------------*/
