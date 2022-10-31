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

fgets(userCMD, sizeof(userCMD), stdin); /* get user input */

struct command *cmd = processUserCmd(userCMD);


return 0;
}
