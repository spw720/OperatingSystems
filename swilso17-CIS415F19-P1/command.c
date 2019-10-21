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
