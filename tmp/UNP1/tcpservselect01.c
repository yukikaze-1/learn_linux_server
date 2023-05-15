#include "unp.h"

int main(int argc,char** argv)
{
    //definition of variable
    int listenfd,connfd,nready,sockfd;
    int clien;

    int i,maxi;
    int client[FD_SETSIZE];
    for(i=0;i<FD_SETSIZE;++i){
        client[i]=-1;
    }
    maxi =-1;

    int maxfd;
    fd_set rset,allset;
    FD_ZERO(&allset);

    char buf[MAXLINE];
     ssize_t n;

    struct sockaddr_in servaddr ,cliaddr;

    bzero(&cliaddr,sizeof(cliaddr));

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family =AF_INET;
    servaddr.sin_addr.s_addr= htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    //socket
    listenfd = Socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    //bind
    Bind(listenfd,(SA*)&servaddr,sizeof(servaddr));
    //listen
    Listen(listenfd,LISTENQ);

    maxfd =listenfd;
    FD_SET(listenfd,&allset);

    //accept
    while(1){
        rset = allset;
        nready = Select(maxfd+1,&rset,NULL,NULL,NULL);

        //new connection
        if(FD_ISSET(listenfd,&rset)){
            clien = sizeof(cliaddr);
            connfd = Accept(listenfd,(SA*)&cliaddr,&clien);

            //update the data client[]
            for(i =0;i<FD_SETSIZE;++i){
                if(client[i] < 0){
                    client[i] =connfd;
                    break;
                }
                if(i == FD_SETSIZE){
                    err_quit("too many clients");
                }
            }
            //update the data allset
            FD_SET(connfd,&allset);
            if(connfd>maxfd)
                maxfd =connfd;
            if(i>maxi)
                maxi =i;

            if(--nready <= 0)
                continue;

        }//end new connection

        //have data to deal for each client in client[]
        for(i =0;i<maxi;++i){
            if( (sockfd =client[i])<0)
                continue;
            if(FD_ISSET(sockfd,&allset)){
                if((n = Read(sockfd,buf,MAXLINE)) == 0){ //recieve the EOF
                //close the socket and update the data of client[] and allset
                    Close(sockfd);
                    client[i] =-1;
                    FD_CLR(sockfd,&allset);
                }else   //didnt recieve the EOF ,have data to write to the socket
                    Writen(sockfd,buf,n);
            }
        }
    }
}