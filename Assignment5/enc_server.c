#include "funcs.h" //include all necessary libs as well as helper functions

int main(int argc, char *argv[]){
	//The below code makes liberal use of Beej's example stream socket
	//https://beej.us/guide/bgnet/html/#a-simple-stream-server
	int connectSock, listenSock, chars_rd = 0; /* listen on listenSock, new conn on connectSock */
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t size_of_cli_info;
	pid_t pid; //pid for child process

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
		exit(1);
	}

	//if sock is successfully created, bind to port and begin to listen
	if(bind(listenSock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
		close(listenSock);
		error("SERVER: ERROR on binding \n");
		exit(1);
	}
	listen(listenSock, MAX_CONNS); //allow up to 5 concurrent conns


	//where the magic happens, actually accept connections
	while(1){ //let's spin!
		//accept connection request and create a socket
		size_of_cli_info = sizeof(cli_addr); //get size for creating socket
	
		//accept conn and error check
		if((connectSock = accept(listenSock, (struct sockaddr*)&cli_addr, &size_of_cli_info)) < 0){
			close(listenSock); //shut off listener
			error("SERVER: ERROR on accept \n");
			continue; //from Beej's socket tutorial - simple stream server
		} //eo if

		//Below code follows Beej's Stream Server example, uses a child process to handle encoding data
		pid = fork(); //fork that process!
		if(pid == -1){ //if fork was unsuccessful
			close(listenSock); //stop listening for incoming conns
			error("SERVER: ERROR could not fork process \n");
			continue;
		} //eo if
		else{ //if we actually spawned a child properly
			//create strings for recieved data from client
			char recvbuf[MAX_BUFFER]; //create buffer for rx'd data
			char encryptmsg[MAX_BUFFER]; //create string to store encrypted message
			char plaintxt[MAX_BUFFER]; //store plaintext
			char keytxt[MAX_BUFFER]; //store key

			//init buffer to 0 with memset
			memset(recvbuf, '\0', sizeof(recvbuf));

			//read the client's handshake first to ensure we conn to right server
			while(chars_rd == 0){
				chars_rd = recv(connectSock, recvbuf, sizeof(recvbuf) - 1, 0); //rx handshake msg
				if(chars_rd < 0) { //make sure we actually read from connected client properly
					error("SERVER: ERROR cannot read from client \n");
					close(connectSock);
					exit(1);
				} //eo if
			} //eo while

			//verify handshake message against defined message
			if(strcmp(recvbuf, ENC_CONFIRM_MSG) != 0){ //if the wrong client connects by accident
				printf("SERVER: Client handshake message does not match \n");
				chars_rd = send(connectSock, BAD_SERV, 7, 0); //send rejection message to client
				if(chars_rd < 0) {
					error("SERVER: ERROR cannot write to client \n");
					close(connectSock);
					exit(2);
				} //eo inner if
				exit(2);
			} //eo outer if
			else { //right client connected
			      printf("SERVER: Connected to correct client \n");
			      chars_rd = send(connectSock, CORRECT_SERV, 7, 0); //send accept message to client
			      if(chars_rd < 0) {
					error("SERVER: ERROR cannot write to client \n");
					close(connectSock);
					exit(2);
			      } //eo if
			
			     memset(recvbuf, 0, MAX_BUFFER); //clear buffer
			     chars_rd = 0; //reset chars read count

			     while(chars_rd == 0) { //read file size
				    chars_rd = recv(connectSock, recvbuf, sizeof(recvbuf) - 1, 0); //rx file len from client
				    if(chars_rd < 0){
					 error("SERVER: ERROR cannot read from client \n");
				    } //eo if
			     } //eo while
			     int flen = atoi(recvbuf); //get file length as an int

			     chars_rd = 0; //reset chars cnt
			     memset(recvbuf, 0, MAX_BUFFER); //clr buffer

			} //eo else

		} //eo while

} //eo main
