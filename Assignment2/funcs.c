#include "funcs.h"

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


    printf("Now processing the chosen file named %s \n", filePath);
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

/* Function that creates a directory and returns its name. Name is in format <ONID>.movies.<randomnumber> */
char* createRandomDir(mode_t mode) {
	char* directory_name = calloc(256, sizeof(char)); /* allocate memory for a string to name of created dir */
	/* Use string concatenation to add to the char array for the directory path */
	strcat(directory_name, ONID); /* append ONID */
	strcat(directory_name, ".movies."); /* append '.movies.' */
	
	/* random number generation */
	int random = (rand() % (MAX_RAND - MIN_RAND + 1)) + MIN_RAND; /* generates random number in 0 - 99999 inclusive */
	char randstr[sizeof(int) * 4 + 1];  /* create 16 byte (plus null terminator) string in order to store integer */
					    /* size of int returns 4 */
	sprintf(randstr, "%d", random); /* convert random integer to a string using sprintf */
	strcat(directory_name, randstr); /* append converted integer to directory name */
	
	if((mkdir(directory_name, mode)) == 0) { /* mkdir returns 0 on success */
		printf("Create a directory with name %s \n", directory_name);
		return directory_name;
	}	
	else {
	  	printf("Problem creating directory \n");
		return NULL;
	}
}


/* Create search function for specific file in a given directory based around the example on 
 * this replit: https://replit.com/@cs344/35statexamplec#main.c. Returns 1 if desired file
 * is found, otherwise returns 0 */
int searchFile(char *directory, char *filename) {
    DIR *currDir = opendir(directory); /* open directory passed to function */
    struct dirent *aDir; /* initialize struct for storing directory entries returned by readdir */
 
    while((aDir = readdir(currDir)) != NULL) { /* loops through entries in directory */
 	if(strcmp(filename, aDir->d_name) == 0){ /* if match is found */
		closedir(currDir); /* close current directory */ 
		return 1;
	}
   }
   /* if not found */
   closedir(currDir); /* close directory */
   return 0;
}



/* Function that returns the filepath of the CSV file in the given directory with the smallest file size. Expects a path to the directory to 
 * look in. Uses stat function to get metadata about each CSV like shown in the replit here: https://replit.com/@cs344/35statexamplec#main.c.
 * Also references code on how to get characters at end of string on this forum post: https://cboard.cprogramming.com/c-programming/371-how-do
 * -i-get-last-character-string.html#post3525 */
char* getSmallestCSV(char* directory) {
	DIR *currDir = opendir(directory); /* Opens passed in directory */
	struct dirent *aDir; /* create struct to store directory entries returned by readdir */
	struct stat dirStats; /* create struct to store data returned by stat */
	int size = 0; /* create int to store size of file */ 
	int count = 0; /* store count of csvs in dir */
	int prefix, suffix; /* create ints to store return value of strncmp */
	char getsuffix[5]; /* create temp string to store file extension */
	char *smallCSV = calloc(256, sizeof(char)); /* create pointer to filename of smallest csv to return */


	while((aDir = readdir(currDir)) != NULL) { /* loop through entries in directory */
		prefix = strncmp("movies_", aDir->d_name, strlen("movies_")); /* compare prefix of file to see if it matches the required prefix */
		sprintf(getsuffix, "%s", aDir->d_name + (strlen(aDir->d_name) - strlen(".csv"))); /* Get characters at end of string by setting pointer
											           * to end of string and subtracting off the size of ".csv") */
		suffix = strncmp(".csv", getsuffix, strlen(".csv"));

		if(prefix == 0 && suffix == 0){ /* if both the prefix and suffix match */
			stat(aDir->d_name, &dirStats); /* get stats of entry in directory */
			if(count == 0) { /* if this is the first csv encountered */		
				size = dirStats.st_size;
				strcpy(smallCSV, aDir->d_name);
				count++;
			}
			else {
				if(dirStats.st_size < size){ /* Only update smallest size when new smallest csv is encountered */
 					size = dirStats.st_size;
					strcpy(smallCSV, aDir->d_name);
					count++;
				}
			}
		}

	}
	closedir(currDir);
	return smallCSV;	
}

/* Essentially clone of getSmallestCSV function, except comparison is whether the size of the current entry in the directory is greater
 * than the current stored size */
char *getLargestCSV(char *directory) {
	DIR *currDir = opendir(directory); /* Opens passed in directory */
	struct dirent *aDir; /* create struct to store directory entries returned by readdir */
	struct stat dirStats; /* create struct to store data returned by stat */
	int size = 0; /* create int to store size of file */ 
	int count = 0; /* store count of csvs in dir */
	int prefix, suffix; /* create ints to store return value of strncmp */
	char getsuffix[5]; /* create temp string to store file extension */
	char *largeCSV = calloc(256, sizeof(char)); /* create pointer to filename of largest csv to return */


	while((aDir = readdir(currDir)) != NULL) { /* loop through entries in directory */
		prefix = strncmp("movies_", aDir->d_name, strlen("movies_")); /* compare prefix of file to see if it matches the required prefix */
		sprintf(getsuffix, "%s", aDir->d_name + (strlen(aDir->d_name) - strlen(".csv"))); /* Get characters at end of string by setting pointer
											           * to end of string and subtracting off the size of ".csv") */
		suffix = strncmp(".csv", getsuffix, strlen(".csv"));

		if(prefix == 0 && suffix == 0){ /* if both the prefix and suffix match */
			stat(aDir->d_name, &dirStats); /* get stats of entry in directory */
			if(count == 0) { /* if this is the first csv encountered */		
				size = dirStats.st_size;
				strcpy(largeCSV, aDir->d_name);
				count++;
			}
			else {
				if(dirStats.st_size > size){ /* Only update smallest size when new largest csv is encountered */
 					size = dirStats.st_size;
					strcpy(largeCSV, aDir->d_name);
					count++;
				}
			}
		}

	}
	closedir(currDir);
	return largeCSV;

}

/* Function that creates text files in a given directory from a given list of movies. Add movies to files based on their year, 
 * and create files if the year hasn't already been made */
void createMoviesTxt(char* directory, struct movie *list) {
	char *filename = calloc(256, sizeof(char)); /* UNIX has max char limit for filenames of 255, allocate space */
	strcat(filename, "./"); /* append path to filename */
	strcat(filename, directory); /* append directory name to path */
	strcat(filename, "/"); /* append seperator after directory name */
	struct movie *tmp = list; /* create temporary pointer to keep track of one ahead current in list of movies */
	int fd; /* file descriptor */
	
	while(list != NULL) { /* loop through csv files to compare years and create files for each */
 		strcat(filename, list->year); /* Append year to filename */
		strcat(filename, ".txt"); /* Append ".txt" to filename */
		fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0640); /* Opens file to write append to */
		/* Used cheatsheet for perms octal from here: https://github.com/lansana/unix-permissions-cheat-sheet */
			
		/* error check file open */
		if(fd == -1){
			 printf("Error on file creation \n");
			 return 1;
		}

		/* write movie title to newly created file */
		write(fd, list->title, strlen(list->title) + 1); /* Write title of movie to newly created file */

		while(tmp->next) {
			if(tmp->year == list->year){ /* if year existed at previous node */
				write(fd, tmp->title, strlen(tmp->title) + 1); /* write title of movie to already created file */
				list = list->next; /* increment prev pointer */
				tmp = tmp->next; /* increment current pointer */
			}

		}
		tmp = tmp->next;
		list = list->next;
	}	
}


/* A slightly modified version of the UI created for Assignment 1. Takes a user input for which file they
 * want to process, except for the exiting loop is controlled by main.c. Expects integer inputs for 
 * user choice */
int mainUI(){ 
   int user_choice = 0; /* create variable to store user choice */
   int reloop = 1;	/* create int to keep user in this loop until a correct choice is selected */
   char* cwd = ".";      /* create string to keep track of current working directory */   
  
   while(reloop) {
	printf("Which file would you like to process? \n");
        printf("1: Process the largest file in the directory \n");
  	printf("2: Process the smallest file in the directory \n");
   	printf("3: Process a specific file \n");
   	printf("Please enter a choice 1-3: \n");

	scanf("%d", &user_choice); /* get a user choice in an integer format */
	switch(user_choice){
		case 1:
		{
			char* dirname = createRandomDir(0755); /* creates directory, returns its name */
			if(dirname != NULL) { /* make sure directory was created successfully */
				char* large = getLargestCSV(cwd); /* search for largest csv in current dir */
				struct movie *list = processFile(large); /* process smallest file */
				printList(list); //test print
			}
			reloop = 0;
			break;
		}
		case 2:
		{
			char* dirname = createRandomDir(0755); /* creates directory, returns its name */
			if(dirname != NULL) { /* make sure directory was properly created */
				char* smallest = getSmallestCSV(cwd); /* search for smallest csv in current dir */
				struct movie *list = processFile(smallest); /* process smallest file */
				printList(list); //test print
			}
			reloop = 0; 
			break;
		} 
		case 3:
		{
			char* filename = calloc(256, sizeof(char)); /* Allocate space for filename string. UNIX has max char limit for filnames of 255 */
			printf("Enter the complete file name: \n");
			scanf("%s", filename); /* save user defined filename. Expects file extension */

			if(searchFile(cwd, filename)) { /* if the searchFile function returned 1, file was found */
				char* dirname = createRandomDir(0755); /* creates directory, returns its name */
				if(dirname != NULL) { /* make sure directory was properly created */
					struct movie *list = processFile(filename); /* process found file */
					printList(list); // test print
					createMoviesTxt(dirname, list);
				}
				reloop = 0;
			}  
			else {
				printf("The file %s was not found. Try again \n", filename);
				reloop = 1;
			}
		free(filename); /* clean up memory allocated for filename string */
		break;
		}  
		default: printf("Invalid choice entered, reprompting... \n"); reloop = 1; break;
   	}
   }
   return 0;
}
