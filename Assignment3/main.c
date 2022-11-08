/*************************************************************************************
 * Author: K. Kopcho
 * Assignment 3: Smallsh
 * CS 344: OS1 Fall 2022
 *************************************************************************************/
#include "builtins.h"

//Global vars
/* def max number of processes that can be started by smallsh */
#define MAX_PROC 1000 
int numProc = 0; /* Number of processes currently running */
pid_t procs[MAX_PROC]; /* array of pids of current running processes  */
char userCMD[2048];
char cwd[256]; /* Create and initialize an array to store the current working directory */
int exitFlag = 0;
int estatus = 0;
int bgAllowed = 0; /* Variable to be modified by SIGSTP. If bg is allowed = 0, if it isn't 1 */


//Function defs (not builtins)
void redirect(struct command *cmd); /* function for redirection */

int main() {
memset(userCMD, '\0', 2048); /* fully initialize user string before writing to it */
memset(cwd, '\0', 256);
//int pid = (int)getpid(); /* int to store the smallsh pid for testing purposes */
//printf("The PID of smallsh is: %d\n", pid);


do{
	printf(": ");
	fflush(stdout);
	fgets(userCMD, sizeof(userCMD), stdin); /* get user input */

	struct command *ucmd = processUserCmd(userCMD);



//test prints
/*
	if(ucmd != NULL) {
		printf("Command successfully created \n");
		fflush(stdout);
	}
	else {
		printf("Command parsing failed \n");
		fflush(stdout);
	}
*/

 	if(ucmd != NULL && ucmd->cmd != NULL){
		if((strcmp(ucmd->cmd, "cd")) == 0){
			changeUserDir(ucmd->args, cwd);
		}
		else if((strcmp(ucmd->cmd, "status")) == 0){
			exitStatus(estatus);
		}
		else if((strcmp(ucmd->cmd, "exit")) == 0){
			exit_smallsh();
			exitFlag = 1;
		}
		else {
			// Code below here is based around the Processes/IO module on Canvas and Explore module of Creating Processes
			pid_t spawnPID = fork();
			procs[numProc] = spawnPID; //add new pid to processes array
			numProc++;
			
			switch(spawnPID) {
				case -1: //spawn fails
					perror("fork() failed \n");
					exit(1);
					break;
				case 0: //spawn that child!!
					redirect(ucmd); //preform redirection
					//Code below based on info in the Exploration: Executing an New Program module
					execvp(ucmd->args[0], ucmd->args);
					perror("execvp failed");
					exit(1); 
					break;
				default: //parent functions
					if(ucmd->background == 1) {//if bg, don't wait
					    waitpid(spawnPID, &estatus, WNOHANG);
					    printf("The background pid is %d\n", spawnPID);
					    fflush(stdout);
					}
					else{ //wait for processes
						waitpid(spawnPID, &estatus, 0);
					}
					break;	
			}

			//check for terminated bg processes
			//Referenced this page: https://www.tutorialspoint.com/unix_system_calls/waitpid.htm
			while((spawnPID = waitpid(-1, &estatus, WNOHANG)) > 0) {
				printf("Background PID %d has finished \n", spawnPID);
				fflush(stdout);
				exitStatus(estatus);
			}
		}
	}
	free(ucmd); /* free struct before proceeding to next command */
	ucmd = NULL;
}while(exitFlag == 0);

return 0;
}


/************************************************************************
 * Function for killing off processes started by smallsh. Since it
 * utilizes global variables, it's fully defined in main, but its
 * prototype is in the "builtins.h" file. Essentially, as processes
 * are created, their PIDS are stored in an array. This function loops
 * through that array and kills each of those processes using SIGTERM
*************************************************************************/
void exit_smallsh() {
	int i;
	if(numProc == 0){ //if all processes are exited
		//printf("All processes done before exit \n");
		exit(0);
	}
	else { //if processes still remain
		for(i =0; i < numProc; i++){
			//printf("The number of processes before exit are %d \n", numProc);
			kill(procs[i], SIGTERM);
		}		
	}

}

/************************************************************************
 * This function handles output and input redirection. The code is based
 * around the dup2 example in the Explore Module about Processes and I/O.
 * Essentially this function checks the flags for redirection in the cmd
 * struct and then uses dup2 to change the targets of stdin/stdout 
*************************************************************************/
void redirect(struct command *cmd) {
	if(cmd->isInput){
	//check bg enable flag- if proc is bg set inputFile to /dev/null
		if(bgAllowed == 0 && cmd->background == 1) {
			strcpy(cmd->inputFile, "/dev/null");
		}
		int source = open(cmd->inputFile, O_RDONLY); //open input for read
		if(source < 0) { //error check
			fprintf(stderr, "Cannot open %s for input \n", cmd->inputFile);
			fflush(stdout);
			exit(1);
		}

		//use dup2 to redir stdin
		if(dup2(source, 0) < 0) { //error check and call dup2
			fprintf(stderr, "dup2 failed \n");
			exit(2);
		}	

		fcntl(source, F_SETFD, FD_CLOEXEC); //close on exec			
	}
	else if(cmd->isOutput){
	//check bg enable flag - if proc is bg set outputFile to /dev/null
		int target;
		if(bgAllowed == 0 && cmd->background == 1) {
			strcpy(cmd->outputFile, "/dev/null");
			target = open(cmd->outputFile, O_WRONLY); //open /dev/null for write
		}
		else { //open specified output file for write
			target = open(cmd->outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		}

		if(target < 0) { //error check
			fprintf(stderr, "Cannot open %s for output \n", cmd->outputFile);
			fflush(stdout);
			exit(1);
		}

		if(dup2(target, 1) < 0) { //call dup2, check for error
			fprintf(stderr, "dup2 failed \n");
			exit(2);
		}

		fcntl(target, F_SETFD, FD_CLOEXEC); //close on exec
	}
}
