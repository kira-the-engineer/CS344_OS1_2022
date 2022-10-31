/*************************************************************************************
 * Author: K. Kopcho
 * Assignment 3: Smallsh
 * CS 344: OS1 Fall 2022
 *************************************************************************************/
#include "builtins.h"

int main() {

//use do while for actual main structure

char userCMD[2048];
memset(userCMD, '\0', 2048); /* fully initialize user string before writing to it */
char cwd[256];
memset(cwd, '\0', 256);

printf(": ");
fflush(stdout);
fgets(userCMD, sizeof(userCMD), stdin); /* get user input */

//printf("User input: %s \n", userCMD);

struct command *ucmd = processUserCmd(userCMD);


if(ucmd != NULL) {
	printf("Command successfully created \n");
	printStruct(ucmd);
	fflush(stdout);
}
else {
	printf("Command parsing failed \n");
	fflush(stdout);
}

changeUserDir(ucmd->args, cwd);
printf("Current directory %s\n", getcwd(cwd, 256));

return 0;
}
