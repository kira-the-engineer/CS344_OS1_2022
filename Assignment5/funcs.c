#include "funcs.h"

//error function from example server on CS 344 replit
//Just prints error msg its passed
void error(const char *msg) {
	perror(msg);
	exit(1);
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
	char c;

	while(plaintxt[i] != '\0'){ //loop for len of plaintxt
		c = plaintxt[i]; //get original char
		enc = (char_2_int(plaintxt[i]) + char_2_int(key[i])) % 27; //get new int for encoded char
		result[i] = int_2_char(enc); //place encoded char in result message
		i++;
	}
	result[i] = '\0'; //add null term to end
}