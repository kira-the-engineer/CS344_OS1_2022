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
int bgAllowed = 1; /* Variable to be modified by SIGSTP. If bg is allowed = 0, if it isn't 1 */
pid_t fgpid;


//Function defs (not builtins)
void redirect(struct command *cmd); /* function for redirection */
void catchSIGSTP(int sig); /* function to handle ctrl-z */
void catchSIGINT(int sig); /* function to handle ctrl-c */

int main() {
memset(cwd, '\0', 256);
memset(userCMD, '\0', 2048);
//int pid = (int)getpid(); /* int to store the smallsh pid for testing purposes */
//printf("The PID of smallsh is: %d\n", pid);

/* Signal handling */
//handle sigstp
struct sigaction sigstpact = {0};
sigstpact.sa_handler = catchSIGSTP;
sigfillset(&sigstpact.sa_mask);
sigstpact.sa_flags = SA_RESTART;
sigaction(SIGTSTP, &sigstpact, NULL);

//handle sigint
struct sigaction sigintact = {0};
sigintact.sa_handler = catchSIGINT;
sigfillset(&sigintact.sa_mask);
sigintact.sa_flags = SA_RESTART;
sigaction(SIGINT, &sigintact, NULL);

do{
	printf(": ");
	fflush(stdout);
	fgets(userCMD, sizeof(userCMD), stdin); /* get user input */

	struct command *ucmd = processUserCmd(userCMD);



//test prints
/*
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
			fgpid = spawnPID;
			procs[numProc] = spawnPID; //add new pid to processes array
			numProc++;
			
			switch(spawnPID) {
				case -1: //spawn fails
					perror("fork() failed \n");
					exit(1);
					break;
				case 0: //spawn that child!!
					//Pass sig handling to child
					
					//child handling of sigint
					if(bgAllowed && ucmd->background) { //ignore ctrl+c if bg
						struct sigaction sigintHandle = {0};
						sigintHandle.sa_handler = SIG_IGN;
						sigaction(SIGINT, &sigintHandle, NULL);
					}
					else {
						struct sigaction sigintHandle = {0};
						sigintHandle.sa_handler = SIG_DFL; //allow ctrl+c to kill fg
						sigaction(SIGINT, &sigintHandle, NULL);
					}

					//Children ignore sigstp
					struct sigaction ignoreSIGSTP = {0};
					ignoreSIGSTP.sa_handler = SIG_IGN;
					sigaction(SIGTSTP, &ignoreSIGSTP, NULL);

					redirect(ucmd); //preform redirection
					//Code below based on info in the Exploration: Executing an New Program module
					execvp(ucmd->cmd, ucmd->args);
					perror("execvp failed");
					exit(1); 
					break;
				default: //parent functions
					if(bgAllowed && ucmd->background) {//if bg, don't wait
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
	if(cmd->isInput && strcmp(cmd->inputFile, "")){
	//check bg enable flag- if proc is bg set inputFile to /dev/null
		if(bgAllowed && cmd->background) {
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
			exit(1);
		}	

		fcntl(source, F_SETFD, FD_CLOEXEC); //close on exec			
	}
	if(cmd->isOutput && strcmp(cmd->outputFile, "")){
	//check bg enable flag - if proc is bg set outputFile to /dev/null
		int target;
		if(bgAllowed && cmd->background) {
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
			exit(1);
		}

		fcntl(target, F_SETFD, FD_CLOEXEC); //close on exec
	}
}


/************************************************************************
 * This function catches and handles SIGSTP. Instead of suspending
 * the program, this function toggles foreground only mode by
 * setting the bgAllow flag
 * Resources used:
 * Canvas module on Signal handling API
*************************************************************************/	
void catchSIGSTP(int sig) {
	if(bgAllowed) {
		write(STDOUT_FILENO, "\nEntering foreground - only mode (&is now ignored)\n", 50);
		fflush(stdout);
		bgAllowed = 0;
	}
	else {
		write(STDOUT_FILENO, "\nExiting foreground-only mode\n", 30);
		fflush(stdout);
		bgAllowed = 1;
	}
	fflush(stdout);
}

/************************************************************************
 * This function catches and handles SIGINT. It will terminate fg child
 * procs if they are running
 * Resources used:
 * Canvas module on Signal handling API
*************************************************************************/
void catchSIGINT(int sig) {
	int fgstatus;
	if(fgpid != 0) { //if theres a fg process running
		waitpid(fgpid, &fgstatus, 0);
		printf("terminated by signal %d\n", sig);
		fflush(stdout);		
	}
}
