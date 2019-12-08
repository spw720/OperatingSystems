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
/*---------------------------------------------------------------------------*/

/*----------------------------Function Definitions---------------------------*/

/*----------------------------------LS---------------------------------------*/

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

/*----------------------------------PWD--------------------------------------*/

void showCurrentDir(){ /*for the pwd command*/

	char cwd[100]; //path to current working dir
	if (getcwd(cwd, sizeof(cwd)) != NULL) {

		write(1, cwd, strlen(cwd));
		write(1, "\n", strlen("\n"));
  }
	else {perror("getcwd() error");}

}// end of showCurrentDir()

/*----------------------------------MKDIR------------------------------------*/

void makeDir(char *dirName){ /*for the mkdir command*/

	if (mkdir(dirName, 0777) == -1){
		write(1, "Error! [mkdir]\n", strlen("Error! [mkdir]\n"));
	}

}// end of makeDir()

/*----------------------------------CD---------------------------------------*/

void changeDir(char *dirName){ /*for the cd command*/

	if (chdir(dirName) != 0){
		write(1, "Error! [cd]\n", strlen("Error! [cd]\n"));
	}

}// end of changeDir()

/*----------------------------------CP---------------------------------------*/

void copyFile(char *sourcePath, char *destinationPath){ /*for the cp command*/

	char *buffy = (char *) calloc(1024, sizeof(char));

	int og_file = open(sourcePath, O_RDONLY);
	//int new_file = open(destinationPath, O_WRONLY|O_CREAT, 0777);
	int new_file = open(destinationPath, O_WRONLY | O_CREAT , 0777);

	if (og_file >= 0) {

		int sz = read(og_file, buffy, 1024);

		write(new_file, buffy, strlen(buffy));

	}
	else {write(1, "Error! [cp]\n", strlen("Error! [cp]\n"));}

	free(buffy);
	close(og_file);
	close(new_file);

}// end of copyFile()

/*----------------------------------MV---------------------------------------*/

void moveFile(char *sourcePath, char *destinationPath){ /*for the mv command*/

	rename(sourcePath, destinationPath);

}// end of moveFile()

/*----------------------------------RM---------------------------------------*/

void deleteFile(char *filename){ /*for the rm command*/

	int rem = unlink(filename);
	if (rem == -1) {
		write(1, "Error! [rm]\n", strlen("Error! [rm]\n"));
	}

}// end of deleteFile()

/*----------------------------------CAT--------------------------------------*/

void displayFile(char *filename){ /*for the cat command*/

	char *buffy = (char *) calloc(1024, sizeof(char));

	int open_file = open(filename, O_RDONLY);
	if (open_file >= 0) {

		int sz = read(open_file, buffy, 1024);

		write(1, buffy, strlen(buffy));

	}
	else {write(1, "Error! [cat]\n", strlen("Error! [cat]\n"));}

	free(buffy);
	close(open_file);

}// end of displayFile()

/*--------------------------------------------------------------------------*/








/*-----------------------------Program Main----------------------------------*/

int main(int argc, char *argv[]) {
	setbuf(stdout, NULL);

	// TODO Figure this out...Null pointers being freed could cause this error?
	FILE *output = NULL;

	// TODO Figure this out...Null pointers being freed could cause this error?
	FILE *input = NULL;

	/*function vars */

	//TODO set to NULL
	char *cBuffer = NULL;

	size_t bufferSize = 32;
	size_t inputSize;

	char *token = NULL;//TODO set to NULL;
	char *token2 = NULL;//TODO set to NULL;

	char *token3 = NULL;//TODO set to NULL;

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
					if (strcmp(token3, ";") == 0){}
				}
				listDir();
			}//end of ls


			else if(strcmp(token, "pwd\n") == 0) {
				showCurrentDir();
			}//end of pwd

			else if(strncmp(token, "pwd", 3) == 0) {
				token3 = strtok(NULL, " \n");
				if (token3 != NULL){
					if (strcmp(token3, ";") == 0){}
				}
				showCurrentDir();
			}//end of pwd

/*-----------------------------1-Argument-----------------------------------*/

			else if(strncmp(token, "mkdir", 5) == 0) {

				token = strtok(NULL, " \n");

				token3 = strtok(NULL, " \n");
				if (token3 != NULL){
					if (strncmp(token3, ";", 1) == 0){}
				}

				makeDir(token);
			}//end of mkdir


			else if(strncmp(token, "cd", 2) == 0) {

				token = strtok(NULL, " \n");


				char *temp_token = token;
				//temp_token[strlen(temp_token)-1] = '\0';

				token3 = strtok(NULL, " \n");
				if (token3 != NULL){
					if (strncmp(token3, ";", 1) == 0){}
				}

				changeDir(token);
			}//end of cd


			else if(strncmp(token, "rm", 2) == 0) {

				token = strtok(NULL, " \n");

				token3 = strtok(NULL, " \n");
				if (token3 != NULL){
					if (strncmp(token3, ";", 1) == 0){}
				}

				deleteFile(token);
			}//end of rm

			else if(strncmp(token, "cat", 3) == 0) {

				token = strtok(NULL, " \n");

				token3 = strtok(NULL, " \n");
				if (token3 != NULL){
					if (strncmp(token3, ";", 1) == 0){}
				}

				displayFile(token);
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
					if (strncmp(token3, ";", 1) == 0){}
				}

				moveFile(token, token2);
			}//end of mv

/*--------------------------------------------------------------------------*/

			else {
				printf("Error: Unrecognized command!\n");
				break;
			}

			token = strtok(NULL, " "); //TODO set to ;

		}// end of while(token not null)

		if(token != NULL) {
			if(strcmp(token, "exit\n") == 0 || strcmp(token, "exit") == 0) { break; }
		}

	} while (1); // end of do-while loop

	/*Free the allocated memory*/
	free(cBuffer);

	// TODO Figure this out...Null pointers being freed could cause this error?
	//fclose(input);
	//fclose(output);

	return 0;

}// end of main()

/*-----------------------------Program End-----------------------------------*/
