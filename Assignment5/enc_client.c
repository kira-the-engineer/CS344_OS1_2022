#include "funcs.h"


int main(int argc, char* argv) {
	int socket, chars_rd, char_wr;
	struct sockaddr_in servAddr;
	//create arrays to store key and plain text from files
	char* plaintext[MAX_BUFFER];
	char* keytext[MAX_BUFFER];
	char *buffer; //have buffer for proc data
	char *ciphertext; //string to store result cipher from server
	int reuse = 1; //create reusable socket

	// Check usage & args
  	if (argc < 3) { 
    		fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); 
   	 exit(0); 
  	}

	// Create a socket
  	socket = socket(AF_INET, SOCK_STREAM, 0); 
  	if (socketFD < 0){
    		error("CLIENT: ERROR opening socket");
		exit(1);
  	}

	// Set up the address struct at localhost
        setupAddressStruct(&servAddr, atoi(argv[3]), "localhost");

	setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)); //reuse sockets

	// Connect to server
    	if (connect(socket, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        	error("CLIENT: ERROR connecting\n");
        	close(socket);
        	exit(1);
    	}
}
