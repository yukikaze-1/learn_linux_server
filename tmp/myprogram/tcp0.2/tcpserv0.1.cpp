#include "tcpserv0.1.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;


//get the target host by name or addr
void Get_Host_Info(struct sockaddr_in& servaddr)
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
    //read the message and file in scoket
    //retrun the new file fd and file name
    file_fd = read_store_file(connfd,filename);
    
    //do something on the file
    dosomething(file_fd,filename);

    //write the file and new message to the socket
    write_back_file(connfd,file_fd,filename);
}

//read the message and file in scoket
//retrun the new file fd
int read_store_file(int connfd,string & filename)
{
    message m;
    //read the message
    read_message(connfd,m);
   filename = m.filename;
    //read and store the file
    string path = SERV_STORAGE_PATH;
    chdir(path.c_str());
    mkdir("tmp2",RWXRWXRWX);
    path=path+"/tmp2";
    chdir(path.c_str());

    char* buf = (char*)malloc(sizeof(off_t)*m.len+1);
    int file_fd;
    if(( file_fd =open(m.filename,O_RDWR|O_CREAT,RWXRWXRWX)) <0){
        cout<<"creat file:"<<m.filename<<" error~";
        return -1;
    }  
    FILE* fp = fdopen(file_fd,"wb");
    ssize_t count ;
    if((count = fwrite(buf,1,(size_t)m.len,fp) )<m.len){
        cout<<"write less into file:"<<m.filename<<endl;
        cout<<"count:"<<count<<endl;
        return -1;
    }
    fclose(fp);
    free(buf);
    return file_fd;
}

//read the message and return it
void read_message(int connfd,struct message& m)
{
    char tmp[100];
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
void write_back_file(int connfd,int file_fd,const string& filename)
{
    //get the file stat
    struct stat info;
    if(fstat(file_fd,&info)<0){
        cout<<"fstat file:"<<filename<<" error\n";
        exit(8);
    }
    //send the message first
    message m;
    m.flag=1;
    m.len = info.st_size;
    const char * p = filename.c_str();
    memcpy(m.filename,p,filename.size());
    send_message(connfd,m);

    //send the file next
    FILE* fp = fdopen(file_fd,"r");
    //malloc the sapce to store the picture
    off_t* sendline = (off_t*)malloc(info.st_size);

    // store the picture in the buffer of sendline 
    fread(sendline,info.st_size,1,fp);
    if(ferror(fp)){
        cout<<"fread error\n";
        exit(7);
    }

    if (write(connfd,sendline,info.st_size)  == -1){
        cout<<"write to the socket error\n";
    }
    cout<<"send the file:"<<filename<<"success~\n";
    //close the file stream and fd and free the buffer
    free(sendline);
    fclose(fp);
    close(file_fd);
}

//send the message
void send_message(int connfd,struct message& m)
{
    char tmp[100];
    memset(tmp,0,sizeof(tmp)); //clear the buffer
    memcpy(tmp,&m,sizeof(m));//copy the message to the buffer

   if( write(connfd,&tmp,sizeof(tmp))  <0){
    cout<<"send message error\n";
    exit(-1);
   }
   cout<< " send file message: flag "<<m.flag<<"len:";
   cout<<m.len<<"filename:"<<m.filename<<" success~";

}


//set the signal SIGCHLD
void Set_Signal()
{
    struct sigaction act;
    act.sa_restorer = sig_chld;
    if(sigaction(SIGCHLD,&act,NULL)<0){
        cout<<"sigaction SIGCHLD error~\n";
        exit(10);
    }
}

void sig_chld()
{
    pid_t pid;
    int stat;

    while(pid = waitpid(-1,&stat,WNOHANG)){
        printf("child %d terminated\n",pid);
    }
    return ;
}
//close
void Close()
{

}