#include "unp.h"

void dg_echo(int sockfd,SA* pcliaddr,socklen_t clien)
{
    int n;
    socklen_t len;

     char buf[MAXLINE];

     while(1){
        len =clien;
        n = Recvfrom(sockfd,buf,MAXLINE,0,pcliaddr,&len);
        Sendto(sockfd,buf,n,0,pcliaddr,len);
     }

}

int main(int argc,char** argv)
{
    //variable definitions
    int listenfd;

    struct sockaddr_in servaddr,cliaddr;
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htos(SERV_PORT);
    servaddr.sin_addr.s_addr = htol(INADDR_ANY);

    //socket
    listenfd = Socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);

    //bind
    Bind(listenfd,(SA*)&servaddr,sizeof(servaddr));

    //do  something
    dg_echo(listenfd,(SA*)&cliaddr,sizeof(cliaddr));

    return 0;
}