/*************************************************************************************
 * Author: K. Kopcho
 * Assignment 3: Smallsh
 * CS 344: OS1 Fall 2022
 *************************************************************************************/
#include "builtins.h"

int main() {
char userCMD[2048];
memset(userCMD, '\0', 2048); /* fully initialize user string before writing to it */
char cwd[256]; /* Create and initialize an array to store the current working directory */
memset(cwd, '\0', 256);
int exitFlag = 0;
int estatus = 0;


do{
	printf(": ");
	fflush(stdout);
	fgets(userCMD, sizeof(userCMD), stdin); /* get user input */

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


 	if(ucmd->cmd != NULL){
		if((strcmp(ucmd->cmd, "cd")) == 0){
			changeUserDir(ucmd->args, cwd);
		}
		else if((strcmp(ucmd->cmd, "status")) == 0){
			exitStatus(estatus);
		}
		else if((strcmp(ucmd->cmd, "exit")) == 0){
			printf("Call exit handler \n");
			exitFlag = 1;
		}
		else {
			printf("Not a builtin... \n");
		}
	}


}while(exitFlag == 0);

return 0;
}
