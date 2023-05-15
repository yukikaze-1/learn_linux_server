#include "unp.h"

#define OPEN_MAX 256

int main(int argc,char** argv)
{
    //variable definitions
    int listenfd,connfd,sockfd,nready;

    ssize_t n;
    char buf[MAXLINE];

    struct sockaddr_in cliaddr,servaddr;
    socklen_t clilen;
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family =AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

   //pollfd struct
   int i,maxi;
    struct pollfd client[OPEN_MAX];

    //socket
    listenfd = Socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    //bind
    Bind(listenfd,(SA*)&servaddr,sizeof(servaddr));

    //listen
    Listen(listenfd,LISTENQ);

    //initiaze the client[0] for listen socket
    client[0].fd =listenfd;
    client[0].events = POLLIN;
    for(i =0;i<OPEN_MAX;++i){
        client[i].fd =-1;
    }
    maxi = 0;

    //accept
    while(1){
        nready = Poll(&client,maxi+1,INFTIM);

        //new client connection
        if(client[0].events & POLLIN){
            clilen = sizeof(cliaddr);
            connfd = Accept(listenfd,(SA*)&cliaddr,&clilen);

            //update the struct pollfd
            for(i =1;i<OPEN_MAX;++i){
                if(client[i].fd < 0)
                    client[i].fd = connfd;
                   break;
            }
            if(i = OPEN_MAX)
                err_quit("too many clients");

             client[i].events = POLLIN;
             maxi = i>maxi?i:maxi;
             if(--nready <= 0)
                continue;
        }

        //have data to deal for each client in client[]
        for(i =1;i<maxi;++i){
            if((sockfd = client[i].fd) < 0)
                continue;
            if(client[i].revents & (POLLIN | POLLERR)){
                if((n = read(connfd,buf,MAXLINE) ) == 0){//recieve the EOF  from the client
                    Close(connfd);
                    client[i].fd = -1;
                }else if (n<0){//read fault
                    if(errno = ECONNRESET){
                        Close(connfd);
                        client[i].fd = -1;
                    }
                    else err_sys("read error");
                }else   
                    Writen(connfd,buf,n);

                if(--nready <=0)
                    break;
            }

        }

    }
    //close

    return 0;
}