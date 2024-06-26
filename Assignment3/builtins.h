/* Header for built in functions and helpers for those functions. 
 * Also includes input grabbing/parsing functions as well as struct for storing parsed data */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

struct command
{
    char *cmd; /* stores cmd itself */
    char *args[512]; /* stores parsed arguments */
    int background; /* integer that is 0 if command runs in foreground, 1 if run in background */
    int isInput; /* flag for redirecting input */
    int isOutput; /* flag for redirecting output */
    char inputFile[256]; /* filename for input file */
    char outputFile[256]; /* filename for output file */
};

/*********************************************************************************
 * Function prototype for a function that prints the exit status of a terminating
 * signal or the last process run in the shell. Returns 0 if run before first
 * cmd
 *********************************************************************************/
int exitStatus(int status);

/*********************************************************************************
 * Function prototype for a function that takes changes the user's directory
 * to an optionally specified argument after the "cd" command
 *********************************************************************************/
void changeUserDir(char** args, char* cwd);

/********************************************************************************
 * Function prototype for a function that parses user input into individual cmds
 * and arguments. The arguments are saved to a string that is 512 chars long. The
 * max length of the user input as a whole can be 2048 chars. Returns a command
 * struct
 ********************************************************************************/
struct command *processUserCmd(char *input);

/********************************************************************************
 * Called in the parseUserCmd function, function prototype for a function that
 * replaces the "$$" in stings in the arguments array with the PID of the smallsh
 * process
 ********************************************************************************/
char* replacePID(char* token); 

/********************************************************************************
 * Function prototype for function that prints members of command struct for 
 * testing purposes
 ********************************************************************************/
void printStruct(struct command *command);

/********************************************************************************
 * Function that trims the trailing whitespace of the command before it's stored
 * in the struct
 ********************************************************************************/
void trimtrailing(char* cmd);

/********************************************************************************
 * every time a new process is started, it's PID is added to a global array. 
 *This function loops through that array and kills each process.
*********************************************************************************/
void exit_smallsh();
