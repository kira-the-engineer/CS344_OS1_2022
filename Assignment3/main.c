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

printf(": ");
fflush(stdout);
fgets(userCMD, sizeof(userCMD), stdin); /* get user input */

printf("User input: %s \n", userCMD);

struct command *cmd = processUserCmd(userCMD);

if(cmd != NULL) {
	printf("Command successfully created \n");
	printStruct(cmd);
}
else {
	printf("Command parsing failed \n");
}

return 0;
}
