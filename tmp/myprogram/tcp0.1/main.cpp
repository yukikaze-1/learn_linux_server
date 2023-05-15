#include "tcpcli0.1.h"

int main(int argc ,char ** argv)
{
     struct sockaddr_in servaddr;
     int sock_fd,file_fd;
    //get the target host by name or addr
    Get_Host_Info(servaddr);
    //connect the target host
    sock_fd =Tcp_Connect(servaddr);

    while (1)
    {
        std::string filename;
        //A.get the path and filename (image) and open it
        file_fd = Get_Usr_Iput(filename);
        //B.send the image to the target host
        Send_To_Serv(sock_fd,file_fd,filename);
        //C.receive the image that is renamed  (or dealed)
        Recv_From_Serv(sock_fd);
    }

    //close
    Close();

    return 0;
}