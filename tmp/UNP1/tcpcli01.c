#include "unp.h"

void
str_cli(FILE *fp, int sockfd)
{
	char	sendline[MAXLINE], recvline[MAXLINE];

	while (Fgets(sendline, MAXLINE, fp) != NULL) {

		Writen(sockfd, sendline, strlen(sendline));

		if (Readline(sockfd, recvline, MAXLINE) == 0)
			err_quit("str_cli: server terminated prematurely");

		Fputs(recvline, stdout);
	}
}

int main(int argc, char ** argv)
{
    //sockfd
    int connect_fd;
    
    if(argc != 2)
        err_quit("useage: tcpcli <IPaddress>");

    //struct sockadddr
    struct sockaddr_in servaddr;

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family =AF_INET;
    Inet_pton(AF_INET,argv[1],&servaddr.sin_addr.s_addr);

    //socket
    connect_fd = Socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    //connect
    Connect(AF_INET,(SA*)&servaddr,sizeof(servaddr));

    //do something
    str_cli(stdin,connect_fd);
    //close;
    exit(0);
    return 0;
}