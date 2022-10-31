/************************************************************************
 * Author: K. Kopcho
 * Smallsh builtin functions
*************************************************************************/

#include "builtins.h"


/********************************************************************************
 * Function that takes in a user input command line and processes it into a
 * struct. The command struct stores the command itself, an integer flag that is
 * set when the command is meant to be run in the background, an array of 512
 * arguments, and strings for input and output file names. Returns pointer to
 * new command struct
 ********************************************************************************/
struct command *processUserCmd(char* input) {

/* First we need to check if this is a comment or blank line */
    if(input[0] = "#" || input == NULL) { /* check if first char is the pound symbol or if input string is empty */
	return NULL; /* if this function returns NULL, main will reprompt user for input */
    }
    else {
         struct command *currCMD = malloc(sizeof(struct command)); /* create a new struct pointer to store a new command*/
	 
	 char *saveptr; /* use for strtok_r */
	 int i; /* create counter for looping through the rest of the input string for args and filenames */
	 int aidx = 0; /* create idx var for array of args */

	/* Find last index in input + check if its "&" */
	int finalidx = strlen(input) - 1;
	if(input[finalidx] = '&'){ /* If & is found at last place in array */
		currCMD->background = 1; /* set flag for bg commands */
	}

	 char *token = strtok_r(input, " ", &saveptr); /* the initial token should be the command itself */
	 currCMD->cmd = calloc(sizeof(token) + 1, sizeof(char)); /* allocate space for command string */
	 strcpy(currCMD->cmd, token); /* Copy token to cmd string in struct */

         for(i = 0; i < strlen(input) - 1, i++;){ 
		if((strcmp(token, "<")) == 0) { /* if char for input file is found */
		    token = (NULL, " ", saveptr); /* move pointer forward 1 */
		    strcpy(currCMD->inputFile, token);
		} 		
		else if((strcmp(token, ">")) == 0) { /* if char for output file is found */
		    token = (NULL, " ", saveptr); /* move pointer fwd 1 */
		    strcpy(currCMD->outputFile, token);
		}
		else { /* add to array of args strings */
		}

		token = (NULL, " "); /* move to next token in input string */
	 }
	
    }
}
