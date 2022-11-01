/************************************************************************
 * Author: K. Kopcho
 * Smallsh builtin functions
*************************************************************************/

#include "builtins.h"

/********************************************************************************
 * Function to search the token and replace all instances of $$ with the PID
 * of the smallsh itself.
 * Adapted from this great stackoverflow thread: https://stackoverflow.com/
 * questions/779875/what-function-is-to-replace-a-substring-from-a-string-in-c
 ********************************************************************************/
char* replacePID(char* token) {
	int pid = (int)getpid(); /* gets PID of smallsh */
	char *pidstr = calloc(pid, sizeof(char));
	sprintf(pidstr,"%d", pid);

	char* result; /* result string */
        char *tmp; /* create tmp pointer */
	char *ins; /* char at next insert point */
	int rem = strlen("$$"); /* get length of $$ to remove */
	int pidlen = 0; /* set as 0 for default */
	int lendiff; /* diff btwn first replace and end of last replace */
	int repcnt; /* count number of replacements */ 

	if(pidstr != NULL){ /* update variable for len of pidstr if it isn't empty */
		pidlen = strlen(pidstr);
	}	

	ins = token; /* start at beginning of token */
	for(repcnt = 0; tmp = strstr(ins, "$$"); ++repcnt){
		ins = tmp + rem;
	}

	tmp = result = malloc(strlen(token) + (pidlen - rem) * repcnt + 1); /* allocate space for replacement */
	
	while(repcnt--) {
		ins = strstr(token, "$$");
		lendiff = ins - token;
		tmp = strncpy(tmp, token, lendiff) + lendiff;
		tmp = strcpy(tmp, pidstr) + pidlen;
		token += lendiff + rem;
	}	
	strcpy(tmp, token);

	/* free at end */
	free(pidstr);
	pidstr = NULL;

	return result;
}

/********************************************************************************
 * Function that trims trailing whitespaces off a string by counting the number
 * of nonspace/non NULL chars and moving the null pointer of the string to 
 * the location of the conted chars + 1. Adapted from here:
 * https://codeforwin.org/2016/04/c-program-to-trim-trailing-white-space-characters
 * -in-string.html
 ********************************************************************************/
void trimtrailing(char* cmd) {
	int idx = -1, i = 0;
	
	while(cmd[i] != '\0'){
		if(cmd[i] != ' ' && cmd[i] != '\t' && cmd[i] != '\n'){
                	idx= i;
        	}
        	i++;
	}
	cmd[idx + 1] = '\0';
}

/********************************************************************************
 * Function that takes in a user input command line and processes it into a
 * struct. The command struct stores the command itself, an integer flag that is
 * set when the command is meant to be run in the background, an array of 512
 * arguments, and strings for input and output file names. Returns pointer to
 * new command struct
 ********************************************************************************/
struct command *processUserCmd(char* input) {

/* First we need to check if this is a comment or blank line */
    if(input[0] == '#' || input == NULL) { /* check if first char is the pound symbol or if input string is empty */
	return NULL; /* if this function returns NULL, main will reprompt user for input */
    }
    else {
         struct command *currCMD = malloc(sizeof(struct command)); /* create a new struct pointer to store a new command*/

	 /* fully initialize struct */
	 currCMD->background = 0; /* by default run in foreground */
	 int i;
	 for(i = 0; i < 512; i++) {
		currCMD->args[i] = NULL;
	 }
	 memset(currCMD->inputFile, 256, '\0');
	 memset(currCMD->outputFile, 256, '\0');

	 char *saveptr; /* use for strtok_r */
	 int aidx = 0; /* create counter for number of arguments accessed */
	 int sidx = 0; /* create variable to store size of arg to add to array */

	 char *token = strtok_r(input, " ", &saveptr); /* the initial token should be the command itself */
	 currCMD->cmd = calloc(sizeof(token) + 1, sizeof(char)); /* allocate space for command string */
	 strcpy(currCMD->cmd, token); /* Copy token to cmd string in struct */
	 trimtrailing(currCMD->cmd); /* trim trailing whitespace off cmd */ 

         while(token != NULL){ 
		if((strcmp(token, "<")) == 0) { /* if char for input file is found */
		    token = strtok_r(NULL, " ", &saveptr); /* move pointer forward 1 */
		    strcpy(currCMD->inputFile, token);
		} 		
		else if((strcmp(token, ">")) == 0) { /* if char for output file is found */
		    token = strtok_r(NULL, " ", &saveptr); /* move pointer fwd 1 */
		    strcpy(currCMD->outputFile, token);
		}
		else { /* add to array of args strings */
		   if(aidx < 512){
		       char *replaced = replacePID(token); /* replace $$ in token */
		       currCMD->args[aidx] = replaced;
		       aidx++;
		   }
		}
		
		token = strtok_r(NULL, " ", &saveptr); /* move to next token in input string */
	
	 } //EO while

	/* lastly check if command is run in the background*/
	int fidx=0;
	/* loop through array while pointer at idx isn't null to get actual number of args */
	while(currCMD->args[fidx]) {
		fidx++;
	}

	printf("last arg: %s \n", currCMD->args[fidx-1]);

	if(currCMD->args[fidx-1][0] == '&'){
		currCMD->background = 1; 
	}
	
        return currCMD;
	
    } // EO else
}

/********************************************************************************
 * Function that prints the command struct for testing purposes
 ********************************************************************************/
void printStruct(struct command *currcmd) {
    int i;
    if(currcmd->cmd != NULL) {
        for(i = 0; i < sizeof(currcmd->cmd); i++){
		printf("%c\n", currcmd->cmd[i]);
	}
    }
    printf("args: ");
    for(i = 0; i < 512; i++) {
	if(currcmd->args[i] != NULL) {
	    printf("%s, ", currcmd->args[i]);
	}
    }
    printf("\n");
    printf("background: %d \n", currcmd->background);
    printf("input file: %s \n", currcmd->inputFile);
    printf("output file: %s \n", currcmd->outputFile);
}

/********************************************************************************
 * Function that prints the exit status of a terminating signal or the last 
 * process run in the shell. Based on the example in chapter 26 of the Linux
 * Programming Interface (pages 545-546). Takes an exit status int as argument.
 * Just returns 0 if no commands have been run yet.
 ********************************************************************************/
int exitStatus(int status) {
	if(!(status == -25)) { /* if a process is run it either returns 1 or 0 for an exit code, so set to random nonzero/non-one int for checking */
		if(WIFEXITED(status)) { /* check if termed normally */
			printf("exit value %d\n", WEXITSTATUS(status));
			fflush(stdout);
		}
		else { /* else, must be termed by signal */
			printf("terminated by signal %d\n", WTERMSIG(status));
			fflush(stdout);
		}		
	}
	return 0;
}

/********************************************************************************
 * Function that changes the current directory to an optional argument by the 
 * user. Takes in arguments array as a parameter.
 * If no directory is specified, changes to the user's home directory.
 ********************************************************************************/
void changeUserDir(char** args, char* cwd) {
	/* get current directory before anything else */
	//printf("Prev dir: %s\n", getcwd(cwd, 256));

	/* First check to see if we need to go home */
	if(args[1] == NULL || (strcmp(args[1], "~")) == 0) {
		//printf("We're going home... \n");
		chdir(getenv("HOME")); /* change to home directory */
		//printf("%s\n", getcwd(cwd, 256));
	}
	else {
		//printf("Switching to specified directory \n");
		if(chdir(args[1]) == -1){
		   printf("Directory not found \n");
		}
		//printf("%s\n", getcwd(cwd, 256));
	}
}
