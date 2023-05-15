#ifndef TCP_CLI_H_
#define TCP_CLI_H_
    #include <iostream>
    #include <netinet/in.h>             //for struct sockaddr_in and htos()
    #include <sys/socket.h>           //for struct sockaddr / socket() /connect()/bind()/listen()
                                                    //accept()/getsockname()/getpeername()/getsockopt() /
                                                    //setsockopt()
    #include <arpa/inet.h>              //for inet_pron() and inet_ntop()
    #include <unistd.h>                   //for fork()/ exec() /close(fd)
    #include <sys/wait.h>               //for wait() / waitpid()/
    #include <sys/select.h>            //for select()
    #include <time.h>                      //for struct timespec and struct timeval 
    #include <netdb.h>                    //for gethostbyname() /struct hostent /gethostbyaddr()
                                                    //getservbyport() / struct servent /getservbyname()
                                                    //getaddrinfo() / struct addrinfo /gai_strerror()
                                                    //freeaddrinfo() / getnameinfo()
                                                    //gethostbyname_r() / gethostbyaddr_r()
    #include <strings.h>                  //for bzero()
    #include <string>
    #include <stdlib.h>                    //for exit()
    #include <dirent.h>                    //for opendir() and readdir() and struct dirent and closedir()
    #include <sys/stat.h>                //for stat()  and struct stat and struct timeval and 
                                                    //struct timespec and macro S_ISDIR()
                                                    //and mkdir()
    #include <stdio.h>                     //for rename()
    #include <fcntl.h>                      //for open()
    #include <memory.h>                 //for memcpy()
                            

    #define SA struct sockaddr
    #define SERV_PORT 4396
    #define BUF_SIZE  32
    #define CLI_STORAGE_PATH "/home/test"
    #define RWXRWXRWX (S_IRWXU|S_IRWXO|S_IRWXO)
    #define FILENAME_SIZE 64
 
    //a struct that use to connect with serv
    struct message{
        bool flag; //1 means ok 0 means NO
        off_t len;
        char filename[FILENAME_SIZE];

        public:
        message():flag('O'),len(0),filename{'\0'}{};
    };


    //get the target host by name or addr
    void Get_Host_Info(struct sockaddr_in& servaddr);

    //connect the target host
    int Tcp_Connect(struct sockaddr_in& servaddr);

    //A.get the path and filename (image) and open it
    int Get_Usr_Iput(std::string& filename);
    
    //B.send the image to the target host
    void Send_To_Serv(int sockfd,int file_fd,std::string& filename);
    //send the file length to the server and wait for the respon
    int send_length(int sockfd,struct message& m);

    //C.receive the image that is renamed  (or dealed)
    void Recv_From_Serv(int sock_fd);
    //recv the length from the server
    int recv_lengtn(int sock_fd,message& m);

    //mkdir for store the file receive from server
    int store_file(off_t * buf,const message& m);

    //close
    void Close();



#endif