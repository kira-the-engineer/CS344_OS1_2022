#include "funcs.h"

int main() {

int user_choice = 0; /* create var to store user choice in */

do{
    printf("1. Select a file to process \n2. Exit the program\n");
    scanf("%d", &user_choice); /* get user choice */

    switch(user_choice){
	  case 1: mainUI(); /* call UI for choosing files to process */ break;
	  case 2: printf("Exiting... \n"); break;
	  default: printf("Invalid input, please select 1 or 2. Retrying...\n"); break; 
    }	

}
while(user_choice !=2); /* stay in loop until user chooses to exit */

return 0;

}
