#include "tcpserv0.1.h"



int main(int argc ,char ** argv)
{
     struct sockaddr_in servaddr,cliaddr;
     int listen_fd,connect_fd;
     socklen_t cli_len;
     pid_t pid;
    //get the port 
    Get_Host_Info(servaddr);
    //server init {socket+bind+listen}
    listen_fd = TCP_Server_Init(servaddr);
    Set_Signal();
    while (1)
    {
        cli_len = sizeof(cliaddr);
        //accept or other:
        std::cout<<"waiting for custom connect\n";
        connect_fd = accept(listen_fd,(SA*)&cliaddr,&cli_len);
        Show_Connect(cliaddr);
        //fork child progress to deal,the parent continue for next loop
        if((pid=fork())>0){//parent
            close(connect_fd);
        }else{//child
            std::cout<<"child progress working now:\n";
            close(listen_fd);
            Deal(connect_fd);
            exit(0);
        }
    }

    //close
    Close();

    return 0;
}