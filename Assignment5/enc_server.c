#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


//error function from example server on replit
void error(const char *msg) {
	perror(msg);
	exit(1);
}


int main(int argc, char *argv[]){
	int connectSock, rdChars, listenSock;

}
