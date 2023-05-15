#include "unp.h"

void dg_cli(FILE* fp,int sockfd,SA* pservaddr,socklen_t servlen)
{
    char sendline[MAXLINE],recvline[MAXLINE+1];

    ssize_t n;
    socklen_t len;
    struct sockaddr * preply_addr = Malloc(servlen);

    while(Fgets(sendline,MAXLINE,fp) != NULL){
    //sendto
        len = servlen;
        Sendto(sockfd,sendline,strlen(sendline),0,pservaddr,len);
    //recvfrom
        n = Recvfrom(sockfd,recvline,MAXLINE,0,preply_addr,&len);
        if(len!= servlen || memcmp(pservaddr,preply_addr,len)!=0){
            printf("reply from %s (ignored)\n",Sock_ntop(preply_addr,len));
            continue;
        }
        recvline[n] =0;
        Fputs(recvline,stdout);
    }

}

int main(int argc, char** argv)
{
    int sockfd;

    size_t n;
    char buf[MAXLINE];

    struct sockaddr_in servaddr;
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htos(SERV_PORT);
    Inet_pton(AF_INET,argv[1],&servaddr.sin_addr.s_addr);

    //socket
    sockfd = Socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);

    //do something
    dg_cli(stdin,sockfd,(SA*)&servaddr,sizeof(servaddr));

    return 0;
}