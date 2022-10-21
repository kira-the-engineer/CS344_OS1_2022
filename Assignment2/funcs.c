#include "funcs.h"


/* A slightly modified version of the UI created for Assignment 1. Takes a user input for which file they
 * want to process, except for the exiting loop is controlled by the main function. Expects integer 
 * inputs for user choice */
int mainUI(){ 
   int user_choice = 0; /* create variable to store user choice */
   int reloop = 1;	/* create int to keep user in this loop until a correct choice is selected */  
  
   while(reloop) {
	printf("Which file would you like to process? \n");
        printf("1: Process the largest file in the directory \n");
  	printf("2: Process the smallest file in the directory \n");
   	printf("3: Process a specific file \n");
   	printf("Please enter a choice 1-3: \n");

	scanf("%d", &user_choice); /* get a user choice in an integer format */
	switch(user_choice){
		case 1: printf("largest \n"); reloop = 0; break;
		case 2: printf("smallest \n"); reloop = 0; break;
		case 3: printf("search \n"); reloop = 0; break;
		default: printf("Invalid choice entered, reprompting... \n"); reloop = 1; break;
   	}
   }
   return 0;
}
