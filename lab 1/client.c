#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>


int main(int argc,char **argv)
{
    int sock_fd;
    
    // Структура хранения адреса для TCP/IP
    struct sockaddr_in serv_addr;

    if (argc!=4)
    {
        printf("Usage: client <ip> <port> <integer>\n");
        return -1;
    }

	// Создаём гнездо заданного типа
    if ((sock_fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        printf("Socket error\n");
        return -1;
    }

    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    
    if (inet_pton(AF_INET,argv[1],&serv_addr.sin_addr) <= 0)
    {
        printf("inet_pton error\n");
        return -1;
    }
    
    // Преобразование 16 битного числа
    serv_addr.sin_port = htons(atoi(argv[2]));
	// Посылает запрос на соединение в серверное гнездо
    if (connect(sock_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
    {
        printf("Connection error\n");
        return -1;
    }

    char buf = argv[3][0];

    for (int i=0;i<10;i++)
    {	
    	// Передаёт сообщение в удалённое гнездо
        send(sock_fd,&buf,sizeof(buf),0);

        sleep(buf - '0');
    }

    return 0;
}
