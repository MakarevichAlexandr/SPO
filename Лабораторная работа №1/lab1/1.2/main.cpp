#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <signal.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

void sig_chld(int signo) {
	pid_t pid;
	int stat;
	while( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);
	return;
}

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

	signal(SIGCHLD, sig_chld);

	for(;;) {
		char message[BUFFER_SIZE];
		int msgLength;
		if((clientSocket = accept(serverSocket, 0, 0)) < 0) {
			perror("Ошибка при выполнении accept");
			exit(1);
		}
			
		int f = fork();
		if(f < 0) {
			perror("Error: create fork");
		}else if(f == 0) {
			close(serverSocket);
		
			bzero(message, BUFFER_SIZE);
			while((msgLength = recv(clientSocket, message, BUFFER_SIZE, 0))) 
				printf("Получено сообщение: %s\n", message);
		
			close(clientSocket); 
			exit(0);
		}
		close(clientSocket);
	}
	return 0;
}
