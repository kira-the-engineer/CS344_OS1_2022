#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

//define globals
#define MAX_CONNS 5

//error function from example server on replit
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


int main(int argc, char *argv[]){
	int connectSock, listenSock, chars_rd = 0; /* listen on listenSock, new conn on connectSock */
	int reuse_yes = 1; //from Beej's socket programming tutorials - simple stream server example
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t size_of_cli_info;

	// Check usage & args
        if (argc < 2) { 
           fprintf(stderr,"USAGE: %s port\n", argv[0]); 
           exit(1);
        } 

	//setup server address struct, argv[1] is port num
	setupAddressStruct(&serv_addr, atoi(argv[1]));

	//create socket to listen for conns
	listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if(listenSock < 0) {
		error("SERVER: ERROR Opening Socket \n");
	}

	//if sock is successfully created, bind to port and begin to listen
	if(bind(listenSock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
		close(listenSock);
		error("SERVER: ERROR on binding \n");
	}
	listen(listenSock, MAX_CONNS); //allow up to 5 concurrent conns


	//where the magic happens, actually accept connections, block if not available
	 
}
