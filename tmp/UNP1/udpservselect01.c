#include "unp.h"

void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    pid = wait(&stat);
    printf("child % d terminated\n",pid);
    return ;
}

int main()
{
    //TCP variabe definitions
    int listenfd,connfd;
    struct sockaddr_in cliaddr;
    socklen_t len;
    pid_t childpid;

    //UDP variable definitions
    int sockfd;
    ssize_t n;
    char updbuf[MAXLINE];
    struct sockaddr_in cliaddr_udp;
    //struct sockaddr_in servaddr; use the same socket with TCP

    //both UDP and TCP variables definition
    struct sockaddr_in servaddr;
    fd_set rset;
    int maxfdp1;
    int nreay;

    //TCP initialize 
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family =AF_INET;
    servaddr.sin_port = htos(SERV_PORT);
    servaddr.sin_addr.s_addr = htol(INADDR_ANY);

    //socket
        listenfd = Socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    //set socket options
        const int on =1;
        Setsockopt(sockfd ,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
        
    //bind
        Bind(listenfd,(SA*)&servaddr,sizeof(servaddr));

    //lsiten
        Listen(listenfd,LISTENQ);

    //UDP initialize
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family =AF_INET;
    servaddr.sin_port = htos(SERV_PORT);
    servaddr.sin_addr.s_addr = htol(INADDR_ANY);
    //socket
        sockfd = Socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    //bind
        Bind(sockfd,(SA*)&servaddr,sizeof(servaddr));


    //other deal
    Signal(SIGCHLD,sig_chld);
    FD_ZERO(&rset);
    maxfdp1 = max(listenfd,sockfd)+1;

    //dosomething
    while(1){
        //pre-deal
        FD_SET(listenfd,&rset);
        FD_SET(sockfd,&rset);

        //select
        if((nreay = select(maxfdp1,&rset,NULL,NULL,NULL) )<0){
            if(errno == EINTR)  continue;
            else err_sys("select error");
        }

        //TCP
        if(FD_ISSET(listenfd,&rset)){
            len = sizeof(cliaddr);
            connfd = Accept(listenfd,(SA*)&cliaddr,&len);

            if( (childpid = Fork()) == 0){// if it is child program
                Close(listenfd);
                str_echo(connfd);
                exit(0);
            }
            Close(connfd);//parent program close the connfd,keep alive with listenfd
        }    
            
        //UDP
        if(FD_ISSET(sockfd,&rset)){
            len = sizeof(cliaddr_udp);
            n = Recvfrom(sockfd,updbuf,MAXLINE,0,(SA*)&cliaddr_udp,&len);

            Sendto(sockfd,updbuf,n,0,(SA*)&cliaddr_udp,len);
        }
    }

    return 0;
}