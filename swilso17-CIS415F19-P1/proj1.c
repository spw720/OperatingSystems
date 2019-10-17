/*
* Description: CLI program that implements ls, cwd, mkdir, cd,
*
* Author: Sean Wilson
* Date: 10/15/19
*
* Notes:
* 1. Make sure output.txt is clean
*/

/*-------------------------Preprocessor Directives---------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include "command.h"
/*---------------------------------------------------------------------------*/

/*----------------------------Function Definitions---------------------------*/

void lfcat()
{
	/* Define your variables here */
	FILE *fp; //output.txt file pointer
	FILE *input; //input file pointer
	char cwd[100]; //path to current working dir
	struct dirent *read_dir; //directory details
  DIR *directory; //directory
	const char* file_name; //name of file
	/* Open the file output.txt for writing */
	fp = fopen("output.txt", "w");
	/* Get the current directory*/
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
  	printf("Current working dir: %s\n", cwd);
  }
	else {
  	perror("getcwd() error");
  }
	/* Open the dir using opendir() */
	directory = opendir (cwd);
	/* use a while loop to read the dir */
	while ((read_dir = readdir(directory)) != NULL) {
			file_name = read_dir->d_name;
			/* Open the file */
			input = fopen(file_name, "r");
			if (input == NULL) {
          perror("Unable to open file!");
      }
			if ( strcmp(file_name, ".") != 0 && strcmp(file_name, "..") != 0 && strcmp(file_name, "lab3.c") != 0 && strcmp(file_name, "lab3") != 0 && strcmp(file_name, "output.txt") != 0 ) {
				char *line = NULL;
     		size_t len = 0;
				printf("*** File: %s ***\n\n", file_name);
				/* Read in each line using getline() */
     		while(getline(&line, &len, input) != -1) {
					/* Write each line to output.txt */
					fprintf(fp, "%s", line);
				} //end of while(theres a line in file) loop
				free(line);
			/* print 80 "-" characters to output.txt */
			int counter = 80;
			fprintf(fp, "\n", "");
			while (counter > 0) {
				fprintf(fp, "-", "");
				counter -= 1;
			}
			fprintf(fp, "\n", "");
			} //end of if(file human-readable)
			/* close the read file and free/null assign your line buffer */
			fclose(input);
  } //end of while(theres a file in directory) loop
	/*close both the output file and the directory you were reading from using closedir() and fclose() */
	closedir(directory);
	fclose(fp);
} //end of lfcat()

/*---------------------------------------------------------------------------*/

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
			printf("%s ", file_name);
	}
	printf("\n", "");

	closedir(directory);

}// end of listDir()

/*---------------------------------------------------------------------------*/

void showCurrentDir(){ /*for the pwd command*/

	char cwd[100]; //path to current working dir
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
  	printf("%s\n", cwd);
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
	printf("Source: %s Destination: %s\n", sourcePath, destinationPath);
}// end of moveFile()

/*---------------------------------------------------------------------------*/

void deleteFile(char *filename){ /*for the rm command*/
	printf("Filename: %s\n", filename);
}// end of deleteFile()

/*---------------------------------------------------------------------------*/

void displayFile(char *filename){ /*for the cat command*/

	FILE *input; //input file pointer
	ssize_t read;
	char * line = NULL;
	size_t len = 0;

	printf("filename: %s\n", filename);

	input = fopen(filename,"r");

	if (input){

		while ((read = getline(&line, &len, input)) != -1) {
					printf("%s", line);
		}

	}
	else {printf("Invalid Filename!\n", "");}
	
	free(line);
	fclose(input);

}// end of displayFile()

/*-----------------------------Program Main----------------------------------*/
int main(int argc, char *argv[]) {
	setbuf(stdout, NULL);

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

	/*main run cycle*/
	do {

		printf( ">>> ");

		inputSize = getline(&cBuffer, &bufferSize, stdin);

		/* tokenize the input string */
		token = strtok(cBuffer, " ");

		while(token != NULL && strcmp(token, "\n")) {

			/*if the command is 'exit then close the program*/
			if(strcmp(token, "exit\n") == 0 || strcmp(token, "exit") == 0) { break; }

			/*Display any commands */
			if(strcmp(token, "lfcat\n") == 0) {lfcat();}

/*-----------------------------No-Argument-----------------------------------*/

			else if(strcmp(token, "ls\n") == 0) {listDir();}

			else if(strcmp(token, "pwd\n") == 0) {showCurrentDir();}

/*-----------------------------1-Argument-----------------------------------*/

			else if(strcmp(token, "mkdir") == 0) {
				token = strtok(NULL, " ");
				makeDir(token);
			}

			else if(strcmp(token, "cd") == 0) {
				token = strtok(NULL, " ");
				changeDir(token);
			}

			else if(strcmp(token, "rm") == 0) {
				token = strtok(NULL, " ");
				deleteFile(token);
			}

			else if(strcmp(token, "cat") == 0) {

				token = strtok(NULL, " \n");

				displayFile(token);
			}

/*-----------------------------2-Argument-----------------------------------*/

			else if(strcmp(token, "cp") == 0) {
				token = strtok(NULL, " ");
				token2 = strtok(NULL, " ");
				copyFile(token, token2);
			}

			else if(strcmp(token, "mv") == 0) {
				token = strtok(NULL, " ");
				token2 = strtok(NULL, " ");
				moveFile(token, token2);
			}

			else {printf("Error: Unrecognized command! \n"); break;}

			token = strtok(NULL, " ");
		}

		if(token != NULL) {
			if(strcmp(token, "exit\n") == 0 || strcmp(token, "exit") == 0) { break; }
		}

	} while (1);

	/*Free the allocated memory*/
	free(cBuffer);
	return 0;

}// end of main()








/*-----------------------------Program End-----------------------------------*/
