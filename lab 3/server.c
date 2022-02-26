#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>




int main()
{
    int sock_fd,conn_fd,nfds;
    fd_set rfds,afds;
    char buf;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr), conn_len; 
    char ipv4[INET_ADDRSTRLEN];

    if ((sock_fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        printf("Socket error\n");
        return -1;
    }

    bzero(&addr,sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = 0;

    if (bind(sock_fd,(struct sockaddr *)&addr,sizeof(addr)) < 0)
    {
        printf("Bind error\n");
        return -1;
    }

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

    printf("ip: %s, port: %d\n",ipv4,ntohs(addr.sin_port));

    if (listen(sock_fd,5) < 0)
    {
        printf("Listen error\n");
        return -1;
    }

    nfds = getdtablesize();


    FD_ZERO(&afds);
    FD_SET(sock_fd,&afds);


    for(;;)
    {
        memcpy(&rfds,&afds,sizeof(rfds));

        if (select(nfds,&rfds,(fd_set *)NULL,(fd_set *)NULL,(struct timeval *)NULL) < 0)
        {
            printf("Select error\n");
            return -1;
        }

        if (FD_ISSET(sock_fd,&rfds))
        {
            conn_fd = accept(sock_fd,(struct sockaddr *)NULL,NULL);

            if (conn_fd < 0)
            {
                printf("Error accepting connection\n");
                return -1;
            }


            FD_SET(conn_fd,&afds);
        }

        for (int fd=0;fd<nfds;fd++)
        {
            if (fd != sock_fd && FD_ISSET(fd,&rfds))
            {
                int read_bytes = recv(fd,&buf,sizeof(buf),0); 
                
                if (read_bytes == 0)
                {
                    printf("CLOSE\n");
                    close(fd);
                    FD_CLR(fd,&afds);
                }
                else
                {
                    printf("%c\n",buf);
                }
                
            }
        }

    }

        

        
    

    close(sock_fd);

    return 0;
}
