#include "funcs.h" //include all necessary libs as well as helper functions

int main(int argc, const char *argv[]){
	//The below code makes liberal use of Beej's example stream socket
	//https://beej.us/guide/bgnet/html/#a-simple-stream-server
	int connectSock, listenSock, status, chars_rd = 0; /* listen on listenSock, new conn on connectSock */
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t size_of_cli_info;
	pid_t pid; //pid for child process

	// Check usage & args
        if (argc < 2) { 
           fprintf(stderr,"USAGE: %s port\n", argv[0]); 
           exit(1);
        } 

	//setup server address struct, argv[1] is port num
	setupAddressStruct(&serv_addr, atoi(argv[1]), "INADDR_ANY"); //any cli can connect

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

	//where the magic happens, actually accept connections
	while(1){ //let's spin!
		listen(listenSock, MAX_CONNS); //allow up to 5 concurrent conns
		//accept connection request and create a socket
		size_of_cli_info = sizeof(cli_addr); //get size for creating socket
	
		//accept conn and error check
		connectSock = accept(listenSock, (struct sockaddr*)&cli_addr, &size_of_cli_info);
		if(connectSock < 0){
			close(listenSock); //shut off listener
			error("SERVER: ERROR on accept \n");
		} //eo if

		//Below code follows Beej's Stream Server example, uses a child process to handle encoding data
		pid = fork(); //fork that process!
		switch(pid){
			case -1: { //if fork was unsuccessful
				close(listenSock); //stop listening for incoming conns
				error("SERVER: ERROR could not fork process \n");
				continue;
			} //eo case -1
			case 0: { //if we actually spawned a child properly
				//create strings for recieved data from client
				char recvbuf[MAX_BUFFER]; //create buffer for rx'd data
				char decryptmsg[MAX_BUFFER] = {0, }; //create string to store decrypted message
				char enctxt[MAX_BUFFER]; //store plaintext
				char keytxt[MAX_BUFFER]; //store key

				//init buffer to 0 with memset
				memset(recvbuf, '\0', sizeof(recvbuf));

				//read the client's handshake first to ensure we conn to right server
				while(chars_rd == 0){
					chars_rd = recv(connectSock, recvbuf, sizeof(recvbuf) - 1, 0); //rx handshake msg
				} //eo while

				if(chars_rd < 0) { //make sure we actually read from connected client properly
						error("SERVER: ERROR cannot read from client \n");
				}//eo if

				//verify handshake message against defined message
				if(strcmp(recvbuf, DEC_CONFIRM_MSG) != 0 || strcmp(recvbuf, ENC_CONFIRM_MSG) == 0){ //if the wrong client connects by accident
					fprintf(stderr, "SERVER: Client handshake message does not match \n");
					chars_rd = send(connectSock, BAD_SERV, 7, 0); //send rejection message to client
					exit(2);
				} //eo outer if
				else { //right client connected
			      		//printf("SERVER: Connected to correct client \n");
			      		chars_rd = send(connectSock, CORRECT_SERV, 7, 0); //send accept message to client
			      		if(chars_rd < 0) {
						close(connectSock);
						error("SERVER: ERROR cannot write to client \n");
			      		} //eo if
			
			     		memset(recvbuf, '\0' , sizeof(recvbuf)); //clear buffer
			     		chars_rd = 0; //reset chars read count

			     		//read filesize
					chars_rd = recv(connectSock, recvbuf, sizeof(recvbuf) - 1, 0);
					if(chars_rd < 0){error("SERVER: ERROR cannot read from client \n");}
			     		long flen = atoi(recvbuf); //get file length as an int

			     		chars_rd = 0; //reset chars cnt
			     		memset(recvbuf, '\0', sizeof(recvbuf)); //clr buffer

					if(flen < 0 || flen == 0){
						error("SERVER: ERROR did not receive valid file length from client \n");
					}
	
					//Once file length has been sent, signal to client to start sending file data
					chars_rd = send(connectSock, "start", 5, 0);
					if(chars_rd < 0){
						error("SERVER: ERROR could not send message to client\n");
					}

					//clear!
					chars_rd = 0;
					memset(recvbuf, '\0', sizeof(recvbuf));

			     		//start by recieving plaintext
					if(readall(connectSock, recvbuf, MAX_BUFFER) == -1){
						error("SERVER: ERROR cannot read from client\n");
					}

					strcat(enctxt, recvbuf);
					strcat(enctxt, "\n");
			     		memset(recvbuf, '\0', sizeof(recvbuf)); //clear buffer

					//RX key
			     		if(readall(connectSock, recvbuf, MAX_BUFFER) == -1){
						error("SERVER: ERROR cannot read from client\n");
					}
					strcat(keytxt, recvbuf);
					strcat(keytxt, "\n");	     			

			    		memset(recvbuf, '\0', sizeof(recvbuf)); //clear buffer
					//printf("Plaintext rx'd: %s\n", enctxt);
					//printf("Keytext rx'd: %s\n", keytxt);

			    		//call decryption func
					decrypt(decryptmsg, enctxt, keytxt);
					long decryptlen = strlen(decryptmsg); //get length of fully decrypted string
			
					if(sendall(connectSock, decryptmsg, &decryptlen) == -1){
						error("SERVER: ERROR Cannot write to client \n");
					}
		 	    		exit(0);
				} //eo inner else
			} //eo case 0
			default: {
				pid_t actual = waitpid(pid, &status, WNOHANG);
			}    
		} //eo switch
		close(connectSock); //close connection to client
	} //eo while
	
	close(listenSock); //close listener socket
	return 0;
} //eo main


