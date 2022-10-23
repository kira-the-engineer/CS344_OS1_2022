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

/* Parse the current line which is comma delimited and create a movie struct with the data in the 
 * line. Based on the code in the replit here: https://replit.com/@cs344/studentsc#main.c. Reused
 * from assignment 1
 */

struct movie *createMovie(char *currLine)
{
    struct movie *currMovie = malloc(sizeof(struct movie));

    // For use with strtok_r
    char *saveptr;
    char *lptr; // seperate pointer for strtok_r with languages str
    char* languages; // string to save all languages from a movie in
    char* lang_str; // string for tokenized languages

    // The first token is the Movie title
    char *token = strtok_r(currLine, ",", &saveptr);
    currMovie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->title, token);

    // The next token is the Year the movie released in
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->year = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->year, token);

    // The next token is the languages
    token = strtok_r(NULL, ",", &saveptr); //get initial string of all languages to work on
    languages = calloc(strlen(token) + 1, sizeof(char)); // Allocate memory for string of languages
    strcpy(languages, token); // copy tokenized string to language_string

    //Get initial token of languages
    token = strtok_r(languages, "[;],", &lptr);
    int i = 0; 
    for(i < 5; i++;) { // A movie can have at most 5 languages
	if(token != NULL){
 	    lang_str = calloc(strlen(token) + 1, sizeof(char));
    	    strcpy(lang_str, token);
	    currMovie->languages[i] = lang_str;
	    token = strtok_r(NULL, ";],", &lptr);
	}	
    }

    // The last token is the Rating/Rank
    token = strtok_r(NULL, "\n", &saveptr);
    currMovie->rank = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->rank, token);

    // Set the next node to NULL in the newly created movie entry
    currMovie->next = NULL;

    return currMovie;
}


/* Print all data for a given movie. Reused from Assignment 1 */
void printMovies(struct movie* singleMovie){
	printf("%s %s ", singleMovie->title, singleMovie->year);
	int i = 0;
	for(i < 5; i++;){
	    if(singleMovie->languages[i] != NULL){
		printf("%s ", singleMovie->languages[i]);
	    }
	}  
	printf("%s \n", singleMovie->rank);
}

/* Print linked list of movie structs. Reused from Assignment 1 */
void printList(struct movie *list)
{
    while (list != NULL)
    {
        printMovies(list);
        list = list->next;
    }
}

/* Reused from Assignment 1. Takes a given filepath for a movies file and processes
 * its content to create a linked list of movie structs. Added error checking :) */
struct movie *processFile(char *filePath)
{
    int line_count = 0;  // Counter for how many lines have been read
    char *currLine = NULL;
    size_t len = 0;
    ssize_t nread;
    char *token;

    // The head of the linked list
    struct movie *head = NULL;
    // The tail of the linked list
    struct movie *tail = NULL;


    printf("Now processing the chosen file named %s \n, filePath");
    // Open the specified file for reading only
    FILE *moviesFile = fopen(filePath, "r");
    
    //Error checking for fopen failure
    if(moviesFile == NULL){
	printf("Error opening file for read mode. \n");
	return head;
    }
	
    //Read first line of the file and discard
    nread = getline(&currLine, &len, moviesFile);

    // Read the file line by line
    while ((nread = getline(&currLine, &len, moviesFile)) != -1)
    {
        // Get a new node corresponding to the current line
        struct movie *newNode = createMovie(currLine);

        // Is this the first node in the linked list?
        if (head == NULL)
        {
           // This is the first node in the linked link
            // Set the head and the tail to this node
            head = newNode;
            tail = newNode;
        }
        else
        {
            // This is not the first node.
            // Add this node to the list and advance the tail
            tail->next = newNode;
            tail = newNode;
        }
	line_count++;
    } 
    free(currLine);
    fclose(moviesFile);
    return head;
}
