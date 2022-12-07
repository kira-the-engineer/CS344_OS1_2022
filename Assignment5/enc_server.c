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
	}

	//if sock is successfully created, bind to port and begin to listen
	if(bind(listenSock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
		close(listenSock);
		error("SERVER: ERROR on binding \n");
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
		}

		//Below code follows Beej's Stream Server example, uses a child process to handle encoding data
		pid = fork(); //fork that process!
		if(pid == -1){ //if fork was unsuccessful
			close(listenSock); //stop listening for incoming conns
			error("SERVER: ERROR could not fork process");
			continue;
		}
		else{ //if we actually spawned a child properly
			//create strings for 
			char recvbuf[MAX_BUFFER]; //create buffer for rx'd data
			char encryptmsg[MAX_BUFFER]; //create string to store encrypted message
			char plaintxt[MAX_BUFFER]; //store plaintext
			char keytxt[MAX_BUFFER]; //store key
		}
	} 
}
