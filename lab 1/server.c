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
    // ��������� �������� ������ ��� TCP/IP
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr), conn_len; 
    char ipv4[INET_ADDRSTRLEN];

	
	// AF_INET - ���� ����������
	// SOCK_STREAM - ��������� ���������� � ���� �������������� ���������������� ������ ������ 

	// ������ ������ ��������� ����
    if ((sock_fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        printf("Socket error\n");
        return -1;
    }

	// ������� �����
    bzero(&addr,sizeof(addr));

    addr.sin_family = AF_INET;
    // ��� ����-������
    // htonl - �������������� 32-x ������� �����
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // ����� �����
    addr.sin_port = 0;

	// ����������� ������ ���
    if (bind(sock_fd,(struct sockaddr *)&addr,sizeof(addr)) < 0)
    {
        printf("Bind error\n");
        return -1;
    }
    
	// ����� ������� ����� �����
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

	// ntohs - ������ ��� �������������� ������ ����� �� ������� ���������� ���� � ���� � ��������� ������� ���������� ���� �� �����
    printf("ip: %s, port: %d\n",ipv4,ntohs(addr.sin_port));

	// ����� ���-�� ��������� ���������� ���������(����� ��������� � ������� � ������ ������)
	// ���-�� �������� = 5
    if (listen(sock_fd,5) < 0)
    {
        printf("Listen error\n");
        return -1;
    }

    signal(SIGCHLD,sig_chld);

	
    for(;;)
    {
    	// ��������� ������ �� ���������� �� ����������� ������
        if ((conn_fd = accept(sock_fd,NULL,NULL)) >= 0)
        {
            if ((ch_pid = fork()) == 0)
            {
                close(sock_fd);

                char buf;

                for (int i=0;i<10;i++)
                {
                	// ��������� ��������� �� ��������� ������
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
