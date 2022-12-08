#include "funcs.h"


int main(int argc, const char* argv[]) {
	int socketFD, chars_rd, chars_wr;
	struct sockaddr_in servAddr;
	char buffer[MAX_BUFFER]; //have buffer for proc data
	char ciphertext[MAX_BUFFER]; //string to store result cipher from server
	int reuse = 1; //create reusable socket

	// Check usage & args
  	if (argc < 3) { 
    		fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); 
   		exit(0); 
  	}

	// Create a socket
  	socketFD = socket(AF_INET, SOCK_STREAM, 0); 
  	if (socketFD < 0){
    		error("CLIENT: ERROR opening socket");
  	}

	// Set up the address struct at localhost
        setupAddressStruct(&servAddr, atoi(argv[3]), "localhost");

	setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)); //reuse sockets

	// Connect to server
    	if (connect(socketFD, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        	close(socketFD);
        	error("CLIENT: ERROR connecting\n");
    	}

	//first validate input
	long pt_len = validCount(argv[1]); //get length of plaintext file
	long kt_len = validCount(argv[2]); //get length of key file

	//check to see if plaintext is longer than key
	if(pt_len > kt_len) {
		error("CLIENT: key is too short!\n");
	}

	//send handshake to server
	char *msg = ENC_CONFIRM_MSG;
	chars_wr = send(socketFD, msg, strlen(msg), 0);
	if(chars_wr < 0){
		error("CLIENT: ERROR cannot write to server \n");
	}
	
	//recv handshake confirmation from server
	memset(buffer, '\0', sizeof(buffer)); //clr buf
	chars_rd = 0; 

	while(chars_rd == 0){
		chars_rd = recv(socketFD, buffer, sizeof(buffer) - 1, 0); //get buffer with null term
	}

	//check to see if we recieved a rejection/bad client
	if(strcmp(buffer, BAD_SERV) == 0){
		error("CLIENT: Bad handshake \n");
	}

	//if we successfully connected, prepare to write file length to server
	memset(buffer, '\0', sizeof(buffer)); //clr buffer again
	chars_wr = 0; //set written char counter back to 0

	//wait for signal from server
	chars_rd = 0;
	while(chars_rd == 0){
		chars_rd = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
	}

	if(strcmp(buffer, "start") == 0){
		//store plaintext length in buffer (since key and plaintext should be same len)
		sprintf(buffer, "%d", pt_len); //use sprintf to copy len to buffer, since strcpy won't work on a cleared string
	
		//send length to server
		chars_wr = send(socketFD, buffer, sizeof(buffer) - 1, 0);
		if(chars_wr < 0){
			error("CLIENT: ERROR cannot write to server \n");
		}

		memset(buffer, '\0', sizeof(buffer)); //clear again
		chars_wr = 0;
		chars_rd = 0;

		//Once file length is sent, send plaintext to server
		int fd = open(argv[1], 'r');
		if(fd == -1){
			error("CLIENT: ERROR cannot open plaintext for read\n");
		}

		while(chars_wr <= pt_len){ //keep in a loop to get all chars sent
			chars_rd = read(fd, buffer, strlen(buffer) -1);
			chars_wr += send(socketFD, buffer, strlen(buffer), 0); //send data to serv
			if(chars_wr < 0){
				error("CLIENT: ERROR cannot write to server \n");
			}
		}

		memset(buffer, '\0', sizeof(buffer)); //you know the drill, clear the buffer and counters
		chars_wr = 0;
		chars_rd = 0;

		fd = open(argv[2], 'r'); //open key for reading this time
		if(fd == -1){
			error("CLIENT: ERROR cannot open key for read\n");
		}
		while(chars_wr <= kt_len){
			chars_rd = read(fd, buffer, strlen(buffer) - 1); //read data into buffer
			chars_wr += send(socketFD, buffer, strlen(buffer), 0); //send data
			if(chars_wr < 0){
				error("CLIENT: ERROR cannot write to server \n");
			}
		}

	}

	//clear!
	memset(buffer, '\0', sizeof(buffer));
	chars_wr = 0;
	chars_rd = 0;

	//Last thing the client needs to do, get ciphertext from server
	while(chars_rd < pt_len){ //get encrypted message from server in loop
		chars_rd += recv(socketFD, buffer, sizeof(buffer) - 1, 0); 
		if(chars_rd < 0){
			error("CLIENT: ERROR cannot read from server \n");
		}
		strcat(ciphertext, buffer);
	}
	strcat(ciphertext, "\n"); //add newline char at end of ciphertext for printing

	printf("%s", ciphertext);

	close(socketFD);

	return 0;
}
