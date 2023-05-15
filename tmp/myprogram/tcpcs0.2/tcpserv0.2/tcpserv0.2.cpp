#include "tcpserv0.2.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;


//get the target host by name or addr
void Get_Socket_Info(struct sockaddr_in& servaddr)
{
    int port;
    while(1){
        cout<<"please input the port number: (2048<x<65535)";
        cin>>port;
        if(2048< port && port<  65535)
            break;
    }
    //update the  IP address information
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;  
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
}

 //server init
int  TCP_Server_Init(const sockaddr_in& servaddr)
{
    int listen_fd;
    //socket
    if( (listen_fd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) <0){
        cout<<"socket error\n";
        exit(1);
    }
    //bind
    if( bind(listen_fd,(SA*)&servaddr,sizeof(servaddr)) < 0){
        cout<<"bind error\n";
        exit(2);
    }

    //listen
    if(listen(listen_fd,BACKLOGNUM) < 0){
        cout<<"listen error:\n";
        exit(3);
    }
    return listen_fd;
}

 //show the connection
void Show_Connect(const sockaddr_in& cliaddr)
{
    char buf[128];
    cout<<"conncet from:"<<inet_ntop(AF_INET,&cliaddr.sin_addr.s_addr,buf,sizeof(buf));
    cout<<",port:"<<ntohs(cliaddr.sin_port)<<endl;
}

 //child progress do the bussiness
void Deal(int connfd)
{
    int file_fd;
    string filename;
    message m;
    struct stat info;

    //read the message 
    recv_message(connfd,m);

    //read the file  retrun  new file fd and file name and stat info
    file_fd = recv_file(connfd,m,info,filename);
    
    //do something on the file
    dosomething(file_fd,filename);

    //send the message
    send_message(connfd,info,filename);

    //write the file and new message to the socket
    send_file(connfd,file_fd,info);
}

//read  file in scoket retrun the new file fd and file stat info
int recv_file(int connfd,message& m,struct stat& info,string& filename)
{
    filename = m.filename;

    //read and store the file
    string path = SERV_STORAGE_PATH;
    chdir(path.c_str());
    mkdir("tmp2",RWXRWXRWX);
    path=path+"/tmp2";
    chdir(path.c_str());

    void* buf = (void*)malloc(sizeof(off_t)*m.len+1);

    int file_fd;
    if(( file_fd =open(m.filename,O_RDWR|O_CREAT,RWXRWXRWX)) <0){
        cout<<"creat file:"<<m.filename<<" error~";
        exit(-1);
    }  

    FILE* fp = fdopen(file_fd,"wb");
    ssize_t count ;
    if((count = fwrite(buf,1,(size_t)m.len,fp) )<m.len){
        cout<<"write less into file:"<<m.filename<<endl;
        cout<<"count:"<<count<<endl;
        free(buf);
        exit(-1);
    }
    fclose(fp);
    free(buf);
    return file_fd;
}

//read the message and return it
void recv_message(int connfd,struct message& m)
{
    char tmp[sizeof(message)+1];
    memset(tmp,0,sizeof(tmp));//clear the buffer

     if(read(connfd,tmp,sizeof(tmp)) <0){
        cout<<"read the message error~";
        exit(2);
    }
    memcpy(&m,tmp,sizeof(m));
    cout<<"message receive from custom:"<<m.filename<<"*"<<m.flag<<"*"<<m.len<<endl;
    
}
    
//do something on the file
void dosomething(int file_fd,const string& filename)
{
    
}

//write the file and new message to the socket
void send_file(int connfd,int file_fd,const struct stat& info)
{
    if(sendfile(connfd,file_fd,NULL,info.st_size)<0){
        cout<<"send_file error~\n";
        exit(7);
    }
}

//send the message
void send_message(int connfd,const struct stat &info,const string& filename)
{
    message m;
    m.flag = 1;
    m.len = info.st_size; 
    cout<<"m.len:"<<m.len<<endl;
    const char* p = filename.c_str();
    memcpy(m.filename,p,filename.size());

    char tmp[100];
    memset(tmp,0,sizeof(tmp)); //clear the buffer
    memcpy(tmp,&m,sizeof(m));//copy the message to the buffer

   if( write(connfd,&tmp,sizeof(tmp))  <0){
    cout<<"send message error\n";
    exit(2);
   }
   cout<< " send file message: flag "<<m.flag<<"len:"<<m.len<<"filename:"<<m.filename<<" success~";
   
}


//set the signal SIGCHLD
void Set_Signal()
{
    struct sigaction act;
    act.sa_handler = sig_chld;
    //act.__sigaction_handler.sa_handler= sig_chld;
    if(sigaction(SIGCHLD,&act,NULL)<0){
        cout<<"sigaction SIGCHLD error~\n";
        exit(10);
    }
}

void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    pid = wait(&stat);
    printf("child %d terminated\n",pid);
    
    return ;
}
//close
void Close()
{

}