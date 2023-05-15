#include "unp.h"

#include <sys/socket.h>//socke function definition

void str_echo(sockfd)
{
    ssize_t n;
    char buf[MAXLINE];
    while(1){
        while( ( n = read(sockfd,buf,MAXLINE )) > 0){
            Writen(sockfd,buf,n);
        }
        if(n<0 && errno == EINTR)   continue;
        else if (n<0 ){
            err_sys("str_echo:read erro!");
            break;
        }else break;
    }
}
//new 02
void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    while(pid = waitpid(-1,&stat,WNOHANG)){
        printf("child %d terminated\n",pid);
    }
    return ;
}//new 02 end

int main()
{
    //fd 
    int listen_fd,connect_fd;
    pid_t child_pid;

    //struct sockaddr  
    struct sockaddr_in cliaddr,servaddr;
    socklen_t clilen;

    //set the serve addr struct
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //the type of (sin_port) is (in_port_t) is (uint16_t)   is unsigned short
    servaddr.sin_port = htons(SERV_PORT);
    //servaddr.sin_port = (unsigned short)(SERV_PORT);


    // socket (family , type,protocol)
    listen_fd = Socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    //bind
    Bind(listen_fd,(void*)&servaddr,sizeof(servaddr));

    //listen
    Listen(listen_fd,LISTENQ);
    //new 02
    Signal(SIGCHLD,sig_chld);

    //accept
    while(1){
        clilen = sizeof(cliaddr);
        //new 02
        if(connect_fd = accept(listen_fd,(void*)&cliaddr,&clilen) < 0){
            if(errno == EINTR)
                continue;
            else
                err_sys("accept error");
        }//new 02 end

        if((child_pid = Fork()) == 0){
            Close(listen_fd);
            str_echo(connect_fd);
            exit(0);
        }
        Close(connect_fd);
    }
    return 0;
}