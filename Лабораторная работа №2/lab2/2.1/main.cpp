#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <strings.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

void *T(void *arg) {
	int clientSocket = *((int *)arg);
	pthread_detach(pthread_self());
	char message[BUFFER_SIZE];
	int msgLength;

	bzero(message, BUFFER_SIZE);
	while((msgLength = recv(clientSocket, message, BUFFER_SIZE, 0)))
		printf("Получено сообщение: %s\n", message);

	close(clientSocket);
	return NULL;
}

int main() {
	pthread_t tid;
	int serverSocket, length;
	struct sockaddr_in serverAddr;
	int clientSocket; 
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
	//	clientSocket = (int *)malloc(sizeof(int));
		if((clientSocket = accept(serverSocket, 0, 0)) < 0) {
			perror("Ошибка при выполнении accept");
			exit(1);
		}
		pthread_create(&tid, 0, T, (void *)&clientSocket);		
	}
	return 0;
}
