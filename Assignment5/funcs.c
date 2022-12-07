#include "funcs.h"

//error function from example server on CS 344 replit
//Just prints error msg its passed
void error(const char *msg) {
	perror(msg);
	exit(1);
}


// func for setting up addr struct from example server on 344 replit
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a client at any address to connect to this server
  address->sin_addr.s_addr = INADDR_ANY;
}
