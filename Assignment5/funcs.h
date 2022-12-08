#include <unistd.h>
#include <string.h>
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netdb.h>      
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>

//Define variables
#define MAX_CONNS 5
#define MAX_BUFFER 100000 //max plaintext is around 70000 chars, buffer needs to fit it
#define ENC_CONFIRM_MSG "enc_cli" //message sent from enc_client to enc_server
#define DEC_CONFIRM_MSG "dec_cli" //message sent from dec_client to dec_server
#define BAD_SERV "reject"
#define CORRECT_SERV "accept"

//function prototyes
void setupAddressStruct(struct sockaddr_in*, int, char*); 
void error(const char*);
void encrypt(char* result, char* plaintext, char* keytext);
int char_2_int(char c);
char int_2_char(int i);
