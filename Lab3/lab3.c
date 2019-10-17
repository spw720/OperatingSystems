/*
* Description: <write a brief description of your lab>
*
* Author: Sean Wilson
* Date: 10/15/19
*
* Notes:
* 1. <add notes we should consider when grading>
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

			/* Hint: use an if statement to skip any names that are not readable files (e.g. ".", "..", "lab-3.c", "lab3.exe", "output.txt" */
			//if (input != "." & input != ".." & input != "lab-3.c" & input != "lab3.exe" & input != "output.txt") {
			if ( strcmp(file_name, ".") != 0
				&& strcmp(file_name, "..") != 0
				&& strcmp(file_name, "lab3.c") != 0
				&& strcmp(file_name, "lab3") != 0
				&& strcmp(file_name, "output.txt") != 0 ) {

				fprintf(fp, "\n\nFILE: %s\n", file_name);

				char *line = NULL;
     		size_t len = 0;

				printf("\n\n*** File: %s ***\n\n", file_name);

				/* Read in each line using getline() */
     		while(getline(&line, &len, input) != -1) {

					printf("line: %s", line);

					/* Write each line to output.txt */
					//fwrite(line , 1 , sizeof(line) , fp );
					fprintf(fp, "%s", line);
					//fprintf(fp, "%s\n", line_break);

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
			//free(line);
			fclose(input);

  } //end of while(theres a file in directory) loop

	/*close both the output file and the directory you were reading from using closedir() and fclose() */
	closedir(directory);
	fclose(fp);

} //end of lfcat()

/*---------------------------------------------------------------------------*/
/*-----------------------------Program Main----------------------------------*/
int main() {
	setbuf(stdout, NULL);

	/*function vars */
	char *cBuffer;
	size_t bufferSize;
	size_t inputSize;
	char *token;

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
		else {printf("Error: Unrecognized command! \n"); break;}
		token = strtok(NULL, " ");
	}
	if(token != NULL) {if(strcmp(token, "exit\n") == 0 || strcmp(token, "exit") == 0) { break; }}
	} while (1);
	/*Free the allocated memory*/
	free(cBuffer);
	return 0;
}
/*-----------------------------Program End-----------------------------------*/
