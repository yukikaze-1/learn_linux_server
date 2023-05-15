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
    #include <signal.h>                    //for sigaction() and struct sigaction
    #include <sys/sendfile.h>          //for sendfile()                        

    #define SA struct sockaddr
    #define	INADDR_ANY		((in_addr_t) 0x00000000)
    #define SERV_PORT 4396
    #define BUF_SIZE  32
    #define SERV_STORAGE_PATH "/home/test"
    //#define TMPDIR_PATH "/home"
    #define RWXRWXRWX (S_IRWXU|S_IRWXO|S_IRWXO)
    #define FILENAME_SIZE 64
    #define BACKLOGNUM 32 
 
    //a struct that use to connect with serv
    struct message{
        bool flag; //1 means ok 0 means NO
        off_t len;
        char filename[FILENAME_SIZE];

        public:
        message():flag(1),len(0),filename{'\0'}{};
    };

    //get the target host by name or addr
    void Get_Socket_Info(struct sockaddr_in& cliaddr);

    //server init
    int TCP_Server_Init(const sockaddr_in& servaddr);

    //recv the message retrun the new file fd
    int recv_file(int connfd,message& m,struct stat& info,std::string& filename);
    
    //read the message and return it
    void recv_message(int connfd,struct message& m);
    
    //send the message
    void send_message(int connfd,const struct stat &info,const std::string& filename);
    
    //write the file and new message to the socket
    void send_file(int connfd,int file_fd,const struct stat& info);

    //do something on the file
    void dosomething(int file_fd,const std::string& filename);

    //child progress do the bussiness
    void Deal(int connfd);

    //show the connection
    void Show_Connect(const sockaddr_in& cliaddr);

    //we should deal with the SIGCHLD signal
    void Set_Signal();
    void sig_chld(int signo);
    
    //close
    void Close();



#endif