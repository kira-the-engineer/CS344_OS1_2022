#include "funcs.h"

//error function from example server on CS 344 replit
//Just prints error msg its passed
void error(const char *msg) {
	perror(msg);
	exit(1);
}


//takes in a result string and file name, opens file for read, reads char by char to result string
//ref: https://stackoverflow.com/questions/4823177/reading-a-file-character-by-character-in-c
void charbychar(char* result, const char* filename){
	char c;
	int count = 0;

	if(filename == NULL){
		fprintf(stderr, "Invalid filename \n");
		exit(1);
	}

	FILE* fp = fopen(filename, "r"); //open file for read

	if(fp == NULL){
		fprintf(stderr, "Cannot open file for read\n");
		exit(1);
	}

	for(c = getc(fp); c != EOF && c != '\n'; c = getc(fp)){ //use for loop instead to get through data
		result[count++] = c;
	}
	result[count] = '\0'; //add null term on end
}

//Send all function from Beej's socket tuts

int sendall(int s, char *buf, long *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
}

//modify sendall to work with reading
int readall(int s, char* buf, long len)
{
    int total = 0;        // how many bytes we've received
    int n = 0;
    char tmp_buf[MAX_BUFFER];
    //Read until same number of bytes read as sent. Result is the full cypher message returned in buf
    while (total < len) {
        // Read data from the socket, leaving \0 at end
        n = recv(s, tmp_buf, MAX_BUFFER - 1, 0);
        if (n == -1) {break; }
        strcat(buf, tmp_buf);
        memset(tmp_buf, 0, MAX_BUFFER);
        total += n;
    }
    return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
} 

// func for setting up addr struct from example server/cli on 344 replit
// modified to check whether we need to create addr struct for serv
// or client
void setupAddressStruct(struct sockaddr_in* address, int portNumber, char *hostname){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);

  if (strcmp(hostname, "INADDR_ANY") == 0) { //client at any address can connect to serv
        address->sin_addr.s_addr = INADDR_ANY;
  }
  else { //if this is for the client side
	//get the DNS info on localhost for client
  	struct hostent* hostInfo = gethostbyname(hostname); 
  	if (hostInfo == NULL) { 
    		fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
    		exit(0); 
  	}
  	// Copy the first IP address from the DNS entry to sin_addr.s_addr
 	memcpy((char*) &address->sin_addr.s_addr, 
        	hostInfo->h_addr_list[0],
        	hostInfo->h_length);
  	}
}

/****************************************************************************
 * little conversion functions to convert to and from A-Z in ASCII with the
 * corresponding integer values 
 ****************************************************************************/
int char_2_int(char c) {
	if(c == ' ') { //if we run across a space char
		return 26;
	}
	else {
		return (c - 'A'); //subtract ASCII A from char to get correct int
	}
	return -1; //return -1 if error
}

char int_2_char(int i) {
	if(i == 26){
		return ' ';
	}
	else {
		return (i + 'A'); //add ASCII A to get correct char 
	}
}

/**************************************************************************** 
 * Function that takes in message and key. Uses key to generate cipher using 
 * OTP method of encryption as described in the wikipedia page
 ****************************************************************************/
void encrypt(char* result, char* plaintxt, char* key){
	int i = 0;
	int enc;

	while(plaintxt[i] != '\0'){ //loop for len of plaintxt
		enc = (char_2_int(plaintxt[i]) + char_2_int(key[i])) % 27; //get new int for encoded char
		result[i++] = int_2_char(enc); //place encoded char in result message
	}
	result[i++] = '\n'; //add newline
	result[i] = '\0'; //add null term to end
}

/**************************************************************************** 
 * Function for decrypting encrypted message using key
 ****************************************************************************/
void decrypt(char* dmsg, char* emsg, char* key) {
    int dec, i = 0;
    while (emsg[i] != '\0') {
        // Convert characters to integers
        dec = char_2_int(emsg[i]) - char_2_int(key[i]); //get decoded char
        if (dec < 0) { // Handle negative case
            dec += 27;
        }
        dmsg[i++] = int_2_char(dec);
    }
    dmsg[i++] = '\n';
    dmsg[i] = '\0';
}


/**************************************************************************** 
 * Function that counts number of chars in file and also checks to see if
 * they are ASCII A-Z or the space character
 * Partially based on this ex here:
 * https://www.geeksforgeeks.org/c-program-to-count-the-number-of-characters-in-a-file/
 ****************************************************************************/
long validCount(const char* fn){
	long count = 0;
	
	FILE* file = fopen(fn, "r"); //open for read

	if(file == NULL){ //verify file exists
		error("CLIENT: ERROR could not open file for read \n");
		return count;
	}

	char c = fgetc(file); //gets first char

	while(!(c == EOF || c == '\n')){ //loop to get and check rest of chars in file
		if(!isupper(c) && (c != ' ')){ //if char is not valid call error and exit
			fclose(file);
			error("CLIENT: File has bad characters \n");
		}
		c = fgetc(file);
		count++;
	}

	fclose(file);
	return count;
}
