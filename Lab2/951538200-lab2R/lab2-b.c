/*
* Description: Takes file input and displays it as tokens
*
* Author: Sean Wilson
*
* Date: 10/08/19
*
* Notes:
* 1.
*/

/*-------------------------Preprocessor Directives---------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*---------------------------------------------------------------------------*/

/*-----------------------------Program Main----------------------------------*/

//int main() {
//	setbuf(stdout, NULL);
//	/* Main Function Variables */
//	char *buffer;
//  size_t bufsize = 32;
//  size_t characters;
//	char *name = "exit";
//	/* Allocate memory for the input buffer. */
//  buffer = (char *)malloc(bufsize * sizeof(char));
//  if( buffer == NULL){
//  	perror("Unable to allocate buffer");
//  	exit(1);
//  }
//	/*main run loop*/
//	while(1) {
//		/* Print >>> then get the input string */
//  	printf(">>> ");
//  	characters = getline(&buffer, &bufsize, stdin);
//		/* Tokenize the input string */
//		char* token = strtok(buffer, " \n");
//		/* If the user entered <exit> then exit the loop */
//		if(strncmp(token, name, strlen(name)) == 0 ){
//			break;
//    }
//		printf("\n");
//		int toke_id = 0;
//		/* Display each token */
//		while (token != NULL) {
//        	printf("T%d: %s\n", toke_id, token);
//        	token = strtok(NULL, " \n");
//					toke_id++;
//		}//end of while(token!=NULL)
//	}//end of while(1)
//	/*Free the allocated memory*/
//	free(buffer);
//	return 1;
//}//end of main()


int main(int argc, char *argv[])
{
 	FILE *fp;
 	char *filename;
 	char ch;
	char *name = "exit";

	char * line = NULL;
	size_t len = 0;
	ssize_t read;

  // Check if a filename has been specified in the command
  if (argc < 2){
        printf("Missing Filename\n");
        return(1);
   }
   else{
        filename = argv[1];
        printf("Filename : %s\n", filename);
   }

   // Open file in read-only mode
   fp = fopen(filename,"r");

   // If file opened successfully, then print the contents
   if ( fp ){
        //printf("File contents:\n");
        //while ( (ch = fgetc(fp)) != EOF )
        //   {
        //        printf("%c",ch);
        //   }
				while ((read = getline(&line, &len, fp)) != -1) {
			        printf("Retrieved line of length %zu:\n", read);
			        printf("%s", line);

							/* Tokenize the input string */
							char* token = strtok(line, " \n");

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


			    }

			    fclose(fp);
			    if (line)
			        free(line);
			    exit(EXIT_SUCCESS);

      }
   else{
         printf("Failed to open the file\n");
        }

return(0);
}

/*-----------------------------Program End-----------------------------------*/
