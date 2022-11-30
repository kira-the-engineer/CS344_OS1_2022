#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

//The purpose of this script is to generate a string of random characters equivalent to the length input by the user
//uses the rand() function to create a random index of in range 0 - 27 (bc the alphabet is 26 letters) to loop through a string of the alphabet
int main(int argc, char* argv[]){
	srand(time(NULL)); //seed random number generator


	if(argc < 2) { //if len of argv is less than 2 print error
		printf("Please enter desired key length. Usage: ./keygen length \n");
		exit(0);
	}
	
	int len = atoi(argv[1]); //get length of string;
	char key[len + 1]; //create key str of len + 1 to inclue null terminator
	char *alpha = " ABCDEFGHIJKLMNOPQRSTUVWXYZ"; //create array of 27 allowed chars

	
	int i;
	for(i = 0; i < len; ++i){
		int idx = random() % 27;
		key[i] = alpha[idx]; //get letter at random idx in alpha array
	}
	key[len] = '\0'; //set last char as null term

	printf("key: %s \n", key);
	return 0;
	
}
