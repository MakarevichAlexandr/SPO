#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <signal.h>

void sig_chld(int signo)
{
    pid_t pid;
    int stat;


    while ((pid = waitpid(-1,&stat,WNOHANG)) > 0);

    return;
}

int main()
{
    int sock_fd,conn_fd,ch_pid;
    // Структура хранения адреса для TCP/IP
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr), conn_len; 
    char ipv4[INET_ADDRSTRLEN];

	
	// AF_INET - стек протоколов
	// SOCK_STREAM - Сообщения передаются в виде упорядоченного двунаправленного потока байтов 

	// Создаём гнездо заданного типа
    if ((sock_fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        printf("Socket error\n");
        return -1;
    }

	// Очищаем буфер
    bzero(&addr,sizeof(addr));

    addr.sin_family = AF_INET;
    // Имя хост-машины
    // htonl - Преобразование 32-x битного числа
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // Номер порта
    addr.sin_port = 0;

	// Присваиваем гнезду имя
    if (bind(sock_fd,(struct sockaddr *)&addr,sizeof(addr)) < 0)
    {
        printf("Bind error\n");
        return -1;
    }
    
	// Чтобы увидеть номер порта
    if (getsockname(sock_fd,(struct sockaddr *)&addr,&len) < 0)
    {
        printf("getsockname error\n");
        return -1;
    }

    if (inet_ntop(AF_INET,&addr.sin_addr,ipv4,sizeof(ipv4)) == NULL)
    {
        printf("Can't convert IPv4 from binary to text form\n");
        return -1;
    }

	// ntohs - служит для преобразования номера порта из порядка следования байт в сети в локальный порядок следования байт на хосте
    printf("ip: %s, port: %d\n",ipv4,ntohs(addr.sin_port));

	// Задаём кол-во ожидающих клиентских сообщений(можно поставить в очередь к одному гнезду)
	// Кол-во запросов = 5
    if (listen(sock_fd,5) < 0)
    {
        printf("Listen error\n");
        return -1;
    }

    signal(SIGCHLD,sig_chld);

	
    for(;;)
    {
    	// Принимает запрос на соединение от клиентского гнезда
        if ((conn_fd = accept(sock_fd,NULL,NULL)) >= 0)
        {
            if ((ch_pid = fork()) == 0)
            {
                close(sock_fd);

                char buf;

                for (int i=0;i<10;i++)
                {
                	// Принимает сообщение из удалённого гнезда
                    recv(conn_fd,&buf,sizeof(buf),0);

                    printf("%c\n",buf);
                }

                exit(0);
            }
            
            close(conn_fd);
        }

        

        
    }

    close(sock_fd);

    return 0;
}
