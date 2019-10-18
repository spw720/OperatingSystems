/*
* Description: CLI program that implements ls, pwd, mkdir, cd, mv, rm,
*
* Author: Sean Wilson
* Date: 10/15/19
*
* Notes:
* 1. Make sure output.txt is clean
*/

/*-------------------------Preprocessor Directives---------------------------*/
#include<fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include "command.h"
/*---------------------------------------------------------------------------*/

/*----------------------------Function Definitions---------------------------*/

void listDir(){ /*for the ls command*/

	DIR *directory; //directory
	char cwd[100]; //path to current working dir
	struct dirent *read_dir; //directory details
	const char* file_name; //name of file

	if (getcwd(cwd, sizeof(cwd)) != NULL) {}
	else {perror("getcwd() error");}

	/* Open the dir using opendir() */
	directory = opendir (cwd);
	/* use a while loop to read the dir */
	while ((read_dir = readdir(directory)) != NULL) {
			file_name = read_dir->d_name;

			write(1, file_name, strlen(file_name));
			write(1, " ", strlen(" "));
	}

	write(1, "\n", strlen("\n"));

	closedir(directory);

}// end of listDir()

/*---------------------------------------------------------------------------*/

void showCurrentDir(){ /*for the pwd command*/

	char cwd[100]; //path to current working dir
	if (getcwd(cwd, sizeof(cwd)) != NULL) {

		write(1, cwd, strlen(cwd));
		write(1, "\n", strlen("\n"));
  }
	else {perror("getcwd() error");}

}// end of showCurrentDir()

/*---------------------------------------------------------------------------*/

void makeDir(char *dirName){ /*for the mkdir command*/
	printf("Directory: %s\n", dirName);
}// end of makeDir()

/*---------------------------------------------------------------------------*/

void changeDir(char *dirName){ /*for the cd command*/
	printf("Directory: %s\n", dirName);
}// end of changeDir()

/*---------------------------------------------------------------------------*/

void copyFile(char *sourcePath, char *destinationPath){ /*for the cp command*/
	printf("Source: %s Destination: %s\n", sourcePath, destinationPath);
}// end of copyFile()

/*---------------------------------------------------------------------------*/

void moveFile(char *sourcePath, char *destinationPath){ /*for the mv command*/

	rename(sourcePath, destinationPath);

}// end of moveFile()

/*---------------------------------------------------------------------------*/

void deleteFile(char *filename){ /*for the rm command*/
	printf("Filename: %s\n", filename);
}// end of deleteFile()

/*---------------------------------------------------------------------------*/

void displayFile(char *filename){ /*for the cat command*/

	char *buffy = (char *) calloc(1024, sizeof(char));

	int open_file = open(filename, O_RDONLY);
	if (open_file >= 0) {

		int sz = read(open_file, buffy, 1024);

		write(1, buffy, strlen(buffy));

	}
	else {write(1, "No such file!\n", strlen("No such file!\n"));}

	close(open_file);

}// end of displayFile()

/*-----------------------------Program Main----------------------------------*/

int main(int argc, char *argv[]) {
	setbuf(stdout, NULL);

	FILE *output;
	FILE *input;

	/*function vars */
	char *cBuffer;
	size_t bufferSize = 32;
	size_t inputSize;
	char *token;
	char *token2;

	/* Allocate memory to the input buffer. */
	cBuffer = (char *)malloc(bufferSize * sizeof(char));
	if( cBuffer == NULL)
	{
		printf("Error! Unable to allocate input buffer. \n");
		exit(1);
	}

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

		inputSize = getline(&cBuffer, &bufferSize, input);

		/* tokenize the input string */
		token = strtok(cBuffer, " ");

		if (token == NULL) {break;}

		while(token != NULL && strcmp(token, "\n")) {

			/*if the command is 'exit then close the program*/
			if(strcmp(token, "exit\n") == 0 || strcmp(token, "exit") == 0) { break; }

			/*Display any commands */

/*-----------------------------No-Argument-----------------------------------*/

			if(strcmp(token, "ls\n") == 0) {listDir();}

			else if(strcmp(token, "pwd\n") == 0) {showCurrentDir();}

/*-----------------------------1-Argument-----------------------------------*/

			else if(strcmp(token, "mkdir") == 0) {
				token = strtok(NULL, " \n");
				makeDir(token);
			}

			else if(strcmp(token, "cd") == 0) {
				token = strtok(NULL, " \n");
				changeDir(token);
			}

			else if(strcmp(token, "rm") == 0) {
				token = strtok(NULL, " \n");
				deleteFile(token);
			}

			else if(strcmp(token, "cat") == 0) {

				token = strtok(NULL, " \n");

				displayFile(token);
			}

/*-----------------------------2-Argument-----------------------------------*/

			else if(strcmp(token, "cp") == 0) {
				token = strtok(NULL, " ");
				token2 = strtok(NULL, " \n");
				copyFile(token, token2);
			}

			else if(strcmp(token, "mv") == 0) {
				token = strtok(NULL, " ");
				token2 = strtok(NULL, " \n");
				moveFile(token, token2);
			}

/*--------------------------------------------------------------------------*/

			else {printf("Error: Unrecognized command! \n"); break;}

			token = strtok(NULL, " ");
		}

		if(token != NULL) {
			if(strcmp(token, "exit\n") == 0 || strcmp(token, "exit") == 0) { break; }
		}

	} while (1);

	/*Free the allocated memory*/
	free(cBuffer);
	fclose(output);
	return 0;

}// end of main()








/*-----------------------------Program End-----------------------------------*/
