#include "unp.h"

int main(int argc,char** argv)
{
    if(argc!=3){
        printf("usage: daytimetcpcli1 <hostname> <service>\n");
        exit(0);
    }
        
    //variables definition
    int connfd;
    socklen_t len;
    char buf[MAXLINE];
    struct sockaddr_in servaddr;

    struct in_addr inetaddr;
    struct in_addr *inetaddrp[2];
    struct in_addr **pptr;

    //gethostbyname
    struct hostent *hp;
    if((hp = gethostbyname(argv[1])) == NULL){
        if(inet_aton(argv[1],&inetaddr) == 0){
            printf("hostname error for %s:%s\n",argv[1],hstrerror(h_errno));
            exit(0);
        }else{
            inetaddrp[0] = &inetaddr;
            inetaddrp[1] = NULL;
        }

    }else{
        pptr = (struct in_addr ** )hp->h_addr_list;
    }
        

    //getservbyname
    struct servent *sp;
    if((sp = getservbyname(argv[2],"tcp"))==NULL){
        printf("get servbyname error for %s\n",argv[2]);
        exit(0);
    }
    
    //socket
    while(*pptr!=NULL){
        connfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

        //fill the ip address and port 
        bzero(&servaddr,sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = sp->s_port;
        memcpy(&servaddr.sin_addr,*pptr,sizeof(struct in_addr));

        //connect
        if( (connect(connfd,(SA*)&servaddr,sizeof(servaddr))) == 0 ){
            break;
        }else{
            printf("connect error\n");
            close(connfd);
        }
        ++pptr;
    }//end while

    if(*pptr == NULL){
        printf("unable to connect\n");
        exit(0);
    }
    
    //do something
    ssize_t n;
    while((n = read(connfd,buf,MAXLINE))){  
        buf[n] = 0;
        fputs(buf,stdout);
    }

    return 0;
}