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


//Function defs (not builtins)


int main() {
memset(userCMD, '\0', 2048); /* fully initialize user string before writing to it */
memset(cwd, '\0', 256);
int pid = (int)getpid(); /* int to store the smallsh pid for testing purposes */
printf("The PID of smallsh is: %d\n", pid);


do{
	printf(": ");
	fflush(stdout);
	fgets(userCMD, sizeof(userCMD), stdin); /* get user input */

	struct command *ucmd = processUserCmd(userCMD);


/* testing code ********
	if(ucmd != NULL) {
		printf("Command successfully created \n");
		printStruct(ucmd);
		fflush(stdout);
	}
	else {
		printf("Command parsing failed \n");
		fflush(stdout);
	}
*/


 	if(ucmd->cmd != NULL){
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
				case -1:
					perror("fork() failed \n");
					exit(1);
					break;
				case 0: //spawn that child!!
					printf("spawning child.... \n");
					break;
				default:
					printf("Still the parent \n");
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
				exitStatus(estatus);
				fflush(stdout);
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
	printf("The number of processes running before exit are %d \n", numProc);
	int i;
	if(numProc == 0){ //if all processes are exited
		exit(0);
	}
	else { //if processes still remain
		for(i =0; i < numProc; i++){
			kill(procs[i], SIGTERM);
		}		
	}

}
