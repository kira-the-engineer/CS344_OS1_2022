// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* struct for storing data from movies spreadsheet*/
struct movie
{
    char *title;
    char *year;
    char *languages[5];
    char *rank;
    struct movie *next;
};

/* Parse the current line which is comma delimited and create a
*  movie struct with the data in this line
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
    for(int i = 0; i < 5; i++) { // A movie can have at most 5 languages
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


/*
* Return a linked list of students by parsing data from
* each line of the specified file.
*/


struct movie *processFile(char *filePath)
{
    // Open the specified file for reading only
    FILE *moviesFile = fopen(filePath, "r");

    int line_count = 0;  // Counter for how many lines have been read
    int movie_count = 0; // Counter for number of movies processed
    char *currLine = NULL;
    size_t len = 0;
    ssize_t nread;
    char *token;

    // The head of the linked list
    struct movie *head = NULL;
    // The tail of the linked list
    struct movie *tail = NULL;

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
    movie_count = line_count - 1;
    printf("Processed file %s and parsed data for %d Movies \n", filePath, movie_count); 
    free(currLine);
    fclose(moviesFile);
    return head;
}


/*
* Print data for the given movie
*/

void printMovies(struct movie* singleMovie){
	printf("%s %s ", singleMovie->title, singleMovie->year);
	for(int i = 0; i < 5; i++){
	    if(singleMovie->languages[i] != NULL){
		printf("%s ", singleMovie->languages[i]);
	    }
	}  
	printf("%s \n", singleMovie->rank);
}


/* Print the linked list of movies*/

void printList(struct movie *list)
{
    while (list != NULL)
    {
        printMovies(list);
        list = list->next;
    }
}

/* Functions for interacting with the movies list */

/*
* This function takes in a user input for a given language, searches 
* the LL for movies with that specific language, and then displays 
* their year and title if they were released in the given language the
* user input. Languages are case sensitive!! 
*/

void searchLanguages(struct movie *list)
{
  char language[20]; // create a string pointer for storing a user input string
  memset(language, '\0', 20); // initialize string with null pointers 
  printf("Please enter the language for which you wish to see movies: \n");
  scanf("%s", &language); //take user input string and save it
  int equal; // storage for return value of strcmp - if 0 strings are equal
  int eqcnt = 0; // count of matches in a given list

  // begin loop through list
  while(list != NULL) { // while not at end of list
	for(int i = 0; i < 5; i++) { // loop through languages array
	   equal = strcmp(language, list->languages[i]);
	   if(equal == 0){
		printf("%s %s \n", list->year, list->title);
		eqcnt++;
		break;
	   }
        }  
	list = list->next;
  }
  
  if(eqcnt == 0){
 	printf("No data about movies released in %s \n", language);
  }

}



/*
*   Insert witty header here :DDDDD
*/

int mainUI(struct movie *list){
    int user_choice = 0; /* create variable to store user choice */ 
    int reloop = 1;	/* create int to keep user in main loop. 1 stays in loop, 0 exits */
   
    while(reloop){
   	printf("Please select one of the following operations to preform on the list of movies \n");
   	printf("1: Show movies released in a specified year \n");
   	printf("2: Show the highest rated movie for each year \n");
   	printf("3: Show title and year of release of all movies in a specified language \n");
   	printf("4: Exit the program \n");
   	printf("Please enter a choice 1-4: \n");
   
   	scanf("%d", &user_choice); /* get a user choice in an integer format */
   	switch(user_choice){
		case 1: /* call year search */ reloop = 1; break;
		case 2: /* call rank func */ reloop = 1; break;
		case 3: searchLanguages(list); reloop = 1; break;
		case 4: printf("Exiting ...\n"); reloop = 0; break;
		default: printf("Invalid choice entered, reprompting... \n"); reloop = 1; break;
   	}
    }
    return 0;
}

int main(int argc, char *argv[]) 
{
    if (argc < 2)
    {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./students student_info1.txt\n");
        return EXIT_FAILURE;
   }
   struct movie *list = processFile(argv[1]);
  
   /* begin UI for selecting movie functions */
   mainUI(list);

   return EXIT_SUCCESS;
}
