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

/* struct for storing the pairs of years and rankings.
* Used to create a second linked list solely for storing
* The highest ranked movies
*/
struct rankings
{
    int year;
    char* title;
    double rank;
    struct rankings *next;
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
  int eqcnt = 0; // count of matches in a given list
  int equal;

  // begin loop through list
  while(list != NULL) { // while not at end of list
       for(int i = 0; i < 5; i++){
	   if(list->languages[i] != NULL){
	      equal = strcmp(language, list->languages[i]);
	   }
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
* This function takes in a user input for a given year, 
* searches the linked list for movies released in a given year
* and displays their title. This is done by converting the year
* string stored in each struct to an int and then directly comparing
* the user input year to that int. If the ints are equal, print the 
* information. 
*/

void searchYear(struct movie *list)
{
   int uYear = 0; // create int to store user input year
   int convert = 0; // create int to store converted string for year in list
   int eqcnt = 0; // create counter for equal values
   printf("Please enter the year you want to see movies for: \n");
   scanf("%d", &uYear);


   while(list != NULL) {
	if(list->year != NULL){
            convert = atoi(list->year);
	    if(convert == uYear) {
	       printf("%s \n", list->title);
	       eqcnt++;
            }
	}
 	list = list->next;
   }
   if(eqcnt == 0) {
	printf("There is no data about movies released in %d \n", uYear);
   }
}

/*
* This function copies the year, ranking, and title data from
* the existing movies linked list into a struct for a linked
* list of the highest ranked movies 
*/

struct rankings* createRankedMovie(struct movie *list) {
    char *tptr; //temp pointer for use with strtod

    struct rankings* tempRank = malloc(sizeof(struct rankings)); // allocate memory for a new rankings struct

    tempRank->year = atoi(list->year); // get year converted to int
    tempRank->rank = strtod(list->rank, &tptr); // get rank converted to double
    tempRank->title = calloc(strlen(list->title) + 1, sizeof(char));
    strcpy(tempRank->title, list->title); // copy title to ranked movie title string
    tempRank->next = NULL;
    return tempRank;
}

/*
* This function creates a linked list of the highest ranked movies by iterating through
* the already existing movies list. It compares if the years are equal, and if an entry
* all ready exists with the same year, it will compare the two and add the highest ranked
* movie to the list. Otherwise, if no year entry exists, it will just add a new node to the
* list. 
*/

struct rankings* rankedMovies(struct movie *list){
    struct rankings* head = createRankedMovie(list); // create first movie in ranked movies by using head of movies list
    struct rankings* tail = head; // set tail to the first node initially
    list = list->next; // initial increment to movies list

    int year;
    double rank; 
    char *ptr;

    while(list != NULL) {
	int year = atoi(list->year);
	double rank = strtod(list->rank, &ptr);

	/* The while loop below allows for multiple loops through the list to check and replace
	 * values up until NULL
         */
	while(tail->next) {
	    if(year == tail->year) {
	      if(rank > tail->rank){
		    tail->title = realloc(tail->title, strlen(list->title));
		    strcpy(tail->title, list->title);
		    tail->rank = rank;
		    tail->year = year;
	       }
	       break;
	    }
	    tail = tail->next;
	}

	/* Check the last link in the list for rank comparison */
	if(year == tail->year) {
	   if(rank > tail->rank){
		tail->title = realloc(tail->title, strlen(list->title));
		strcpy(tail->title, list->title);
		tail->rank = rank;
		tail->year = year;	
	   }
	   tail = tail->next;
	}
	else{ /* if the year does not already exist, add it to the list */
	    struct rankings* newNode = createRankedMovie(list);
	    tail->next = newNode;
	}
	tail = head; /* reset for next loop */
	list = list->next;
    }

    return head;
}

/* simple print function to print data from a movie in the ranked movies list */
void printRankedMovie(struct rankings* rm) {
     printf("%d %.1f %s \n", rm->year, rm->rank, rm->title);
}

/* function for printing the whole list of ranked movies */
void printRMList(struct rankings *list) {
   while(list != NULL) {
	printRankedMovie(list);
	list = list->next;
   }
}

/* This function frees the memory associated with the
 * linked list of ranked movies. Mostly the title 
 * strings
 */

void cleanRankedMovies(struct rankings* rm)
{
    struct rankings* temp; // create temp node ptr 
    while(rm != NULL) {
	temp = rm; // set head of rm to temp
	rm = rm->next; // iterated to next link
	free(temp->title); // free data in current link
	free(temp); // free temp ptr
    }
    rm = NULL; // set rm back to NULL data
}

/*
* This function creates a highest ranked movies list, prints the information
* (year, rank, title) of the highest ranked movies, and then frees the list
*/
void displayRankedMovies(struct movie *list){
     struct rankings *rm = rankedMovies(list); // create ranked movies list
     printRMList(rm); 
     cleanRankedMovies(rm);
}

/*
* This function provides the main user input to interact with the list of movies. It loops
* until the user chooses to exit and also will reprompt the user if they do not enter a 
* valid input.
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
		case 1: searchYear(list); reloop = 1; break;
		case 2: displayRankedMovies(list); reloop = 1; break;
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
