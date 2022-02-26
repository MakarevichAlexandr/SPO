#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
	if(argc != 4) {
		printf("usage: %s <ip-address> <port> <number>\n", argv[0]);
		exit(1);
	}

	int socketID;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in serverAddr;
	struct hostent *hst;

	if((socketID = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Error: could not create socket\n");
		exit(1);
	}
	
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi(argv[2]));	
	if(inet_pton(AF_INET, argv[1], &serverAddr.sin_addr) <= 0) {
		perror("inet_pton error occured");
		exit(1);
	}	

	if(connect(socketID, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
		perror("Error: connect");
		exit(1);
	}
		
	strcpy(buffer, argv[3]);	
	for(int i = 0; i < 100; i++) {
		if(send(socketID, buffer, strlen(argv[3]), 0) == -1) {
			perror("Ошибка при выполнении send\n");
			exit(1);
		}	
		sleep(atoi(argv[3]));	
	}	
	close(socketID);	
	return 0;
}
