#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* Reuse Movie Struct from Assignment 1 to hold info from CSV */
struct movie
{
    char *title;
    char *year;
    char *languages[5];
    char *rank;
    struct movie *next;
};

/* Reuse Create Movie Function from Assignment 1 to create movie nodes for this program */
struct movie *createMovie(char *currLine);

/* Function that takes care of validating user input for which selection they want to make */
int mainUI();

/* Function that takes care of processing a file at a given path and adding its contents to a 
 * linked list of movies */
struct movie *processFile(char *filePath);

/* Reuse functions for printing movies and printing the linked list of movies for testing purposes.
 * These functions were also adapted from the students.c files on replit
 */
void printMovies(struct movie* singleMovie);
void printList(struct movie *list);

/* Function for searching for a file in the given path with given name 
 * Name char is expected to include the file extension. The searching in this function
 * is based on the strcmp method in example 3_5_stat in Dr. Chaundry's replit here: 
 * https://replit.com/@cs344/35statexamplec#main.c
 */
int searchFile(char* path, char* name);

/* Function that returns the filepath of the CSV file in the current directory with
 * the largest file size. Expects a path to the directory to look in. Uses stat
 * function to get metadata about each CSV like shown in the replit here:
 * https://replit.com/@cs344/35statexamplec#main.c 
 */
char* getlargestCSV(char* dirpath);

/* Function thay returns filepath of the CSV file in the current director with the
 * smallest file size. Expects path to directory to look in. Uses stat function 
 */
char* getsmallestCSV(char* dirpath);

/* Creates random directory within the current directory with specified permissions.
 * References Exploration resource on Canvas about directories. Returns name of
 * directory that is created */
char* createRandomDir(mode_t mode);


