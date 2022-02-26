#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>

#define BUFFER_SIZE 1024

int main() {
	int serverSocket, clientSocket, length;
	struct sockaddr_in serverAddr;

	if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Ошибка при выполнении socket");
		exit(1);
	}
	
	bzero((char *)&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = 0;

	if(bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) {
		perror("Ошибка bind.");
		exit(1);
	}

	length = sizeof(serverAddr);
	if(getsockname(serverSocket, (struct sockaddr *)&serverAddr, (socklen_t*)&length)) {
		perror("Вызов getsockname неудачен.");
		exit(1);
	}
	
	printf("Номер порта сервера - %d\n", ntohs(serverAddr.sin_port));
	listen(serverSocket, 5);

	for(;;) {
		char message[BUFFER_SIZE];
		int msgLength;
		if((clientSocket = accept(serverSocket, 0, 0)) < 0) {
			perror("Ошибка при выполнении accept");
			exit(1);
		}
		
		bzero(message, BUFFER_SIZE);
		while((msgLength = recv(clientSocket, message, BUFFER_SIZE, 0))) 
			printf("Получено сообщение: %s\n", message);
		
		close(clientSocket); 
	}
}
