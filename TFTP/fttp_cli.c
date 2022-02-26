#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <strings.h>
#include <stdint.h>

enum opcode 
{
    RRQ = 1,WRQ
};

int request_packet(enum opcode op,unsigned char *buffer,char *filename);
int ack_packet(unsigned char *buffer,unsigned char par1,unsigned char par2);

/*
    ./tftp_cli  -g/-p ip port filename
*/
int main(int argc,char **argv)
{
    if (argc!=5)
    {
        printf("Usage: ./tftp_cli -g/-p ip port filename\n");
        printf("\t'-g' - скачать файл с сервера, -p - загрузить файл на сервер\n");
        printf("\tip - ipv4 адрес сервера\n");
        printf("\tport - порт сервера (обычно 69)\n");
        printf("\tfilename - имя файла, который нужно скачать/загрузить\n");

        return -1;
    }


    unsigned char buffer[256],file_buf[1024],ack_buf[32];
    struct sockaddr_in serv_addr;
    int sockfd,port = atoi(argv[3]),getsock;

    sockfd = socket(AF_INET,SOCK_DGRAM,0);

    bzero(&serv_addr,sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);


    enum opcode op;

    if (strcmp("-p",argv[1])==0)
    {
        op = WRQ;        
    }
    else if (strcmp("-g",argv[1])==0)
    {
        op = RRQ;
    }
    else
    {
        close(sockfd);
        printf("First param must be '-g' or '-p'\n");
        return  -2;
    }
    
    
    int size = request_packet(op,buffer,argv[4]);   
    int recv_size;

    if (size<0)
    {
        close(sockfd);
        printf("Packet creating error\n");
        return -1;
    }

    sendto(sockfd,buffer,size,0,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

    int ack_size = 0;
    unsigned short packet_num;
    struct timeval timeout;

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(struct timeval *)&timeout,sizeof(struct  timeval));

    if (op==RRQ)
    {
        FILE *f = fopen(argv[4],"wb");
        int cnt = 0;
        
        while(1)
        {
            size = recvfrom(sockfd,file_buf,sizeof(file_buf),0,(struct sockaddr*)&serv_addr,&recv_size);
            packet_num = file_buf[3];

            if (size<=0)
            {
                continue;    
            }
            
            

            fwrite(file_buf+4,size-4,1,f);
            
            //printf("Packet #%u receive, size = %d\n",packet_num,size-4);

            ack_size = ack_packet(ack_buf,file_buf[2],file_buf[3]);
            sendto(sockfd,ack_buf,ack_size,0,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

            if (size-4<512)
            {
                printf("Last packet receive!\n");
                break;
            }
        }
        fclose(f);
    }
    else 
    {
        
    }
    
    
    close(sockfd);

    return 0;
}


/*
    Create RRQ/WRQ packet
*/

int request_packet(enum opcode op,unsigned char *buffer,char *filename)
{
    int n = sprintf(buffer,"%c%c%s%c%s%c",0,op,filename,0,"octet",0);

    if (n<=0)
    {
        return -1;
    }

    return n;
}

int ack_packet(unsigned char *buffer,unsigned char par1,unsigned char par2)
{
    int n = sprintf(buffer,"%c%c%c%c",0,4,par1,par2);

    if (n<=0)
    {
        return -1;
    }

    return n;
}