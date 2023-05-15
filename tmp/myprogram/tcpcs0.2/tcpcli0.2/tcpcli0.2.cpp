#include "tcpcli0.2.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;

#define _TEST


//get the target host by name or IP address
void Get_Host_Info(struct sockaddr_in& servaddr)
{
    //choose use domain name or IP address
    char choice;
    while(1){
        cout<<"please choose input domain name or IP address:"\
                    "(1 for domain,2 for IP address)\n";
        cin>>choice;
        if(choice == '1' || choice == '2'){
            break;
        }    
    }

    //get the usr input
    string input_address;
    cout<<"now input your domain name or IP address:\n";
    cin>>input_address;

    //update the  IP address information
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;  
    servaddr.sin_port = htons(SERV_PORT);

    if(choice == '1'){ //choose domain name
        struct hostent *servinfo = gethostbyname(input_address.c_str());
        inet_pton(AF_INET,servinfo->h_addr_list[0],&servaddr.sin_addr.s_addr);
    }else{ //choose IP address
        inet_pton(AF_INET,input_address.c_str(),&servaddr.sin_addr.s_addr);
    }

}

//connect the target host
int Tcp_Connect(struct sockaddr_in& servaddr)
{
    //socket
    int sock_fd;
    if((sock_fd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == -1){
        cout<<"socket error\n";
        exit(1);
    }
  
    //connect
      if(connect(sock_fd,(SA*)&servaddr,sizeof(servaddr)) == -1){
        cout<<"connect error\n";
        exit(2);
      }
      
      cout<<"connect success!\n";
      return sock_fd;
}

//A.get the path and filename (image) and open it 
int Get_Usr_Iput(string& filename)
{
    string path,fullname;
    int file_fd;
    struct stat info;
    //get the path of file and verify it
    while(1){
        path.clear();
        cout<<"please enter the picture files path:\n";
        cin>>path;
        if(stat(path.c_str(),&info) ==-1){
            cout<<"stat error\n";
            continue;
        }
        if(!S_ISDIR(info.st_mode)){
            cout<<"Its not a directory! please try again!\n";
            continue;
        }else
            break;
    }

    //get the filename an d verify it
    while(1){
        filename.clear();
        cout<<"please enter the picture filesname:\n";
        cin>>filename;
        fullname = path+"/"+filename;
        if(stat(fullname.c_str(),&info) ==-1){
            cout<<"stat error\n";
            continue;
        }
        if(!S_ISREG(info.st_mode)){
            cout<<"Its not a regular file! please try again!\n";
            continue;
        }else
            break;
    }
    cout<<"in Get_Usr_Iput: file length:"<<info.st_size<<endl;

    if( (file_fd = open(fullname.c_str(),O_RDONLY)) == -1){
        cout<<"open file :"<<fullname<<" error\n";
        exit(3);
    }
    cout<<"open file:"<<fullname<<"success~\n";
    #ifdef _TEST
    cout<<"file_fd in Get_Usr_Iput:"<<file_fd<<endl;
    cout<<"in Get_Usr_Iput: file length*:"<<info.st_size<<endl;
    #endif
    return file_fd;
}

//B.send the image to the target host
void Send_To_Serv(int sock_fd,int file_fd,const string& filename)
 {
    //variable def
    struct stat info;
    if((fstat(file_fd,&info)) == -1){
        cout<<"fstat error\n";
        exit(4);
    }
    #ifdef _TEST
    cout<<"file length in Send_To_Serv:"<<info.st_size<<endl;
    #endif

    //send_message
    send_message(sock_fd,info,filename);

    //send_file
    send_file(sock_fd,file_fd,info);

 }

//send the file linfo to the server and wait for the respon
int send_message(int sock_fd,const struct stat &info,const string& filename)
{
    message m;
    m.flag = 1;
    m.len = info.st_size; 
    cout<<"m.len:"<<m.len<<endl;
    const char* p = filename.c_str();
    memcpy(m.filename,p,filename.size());

    char tmp[sizeof(message)];
    memset(tmp,0,sizeof(tmp)); //clear the buffer
    memcpy(tmp,&m,sizeof(m));//copy the message to the buffer

   if( write(sock_fd,&tmp,sizeof(tmp))  <0){
    cout<<"send message error\n";
    exit(2);
   }
   cout<< " send file message: flag "<<m.flag<<" len:"<<m.len<<"filename:"<<m.filename<<" success~\n";
   return 0;
}

//send the file
int send_file(int sock_fd,int file_fd,const struct stat &info)
{
    if(sendfile(sock_fd,file_fd,NULL,info.st_size) <0){
        cout<<"sendfile error\n";
        exit(2);
    }
    cout<<"send file success~\n";
    return 0;
}

void Recv_From_Serv(int sock_fd)
{
    //variable def
    message m;

    //epoll
    int epfd = epoll_create(5);
    epoll_event events[MAX_EVENT_NUMBER],ev;
    bzero(&ev,sizeof(ev));
    bzero(&events,sizeof(events));
    ev.events = EPOLLIN;
    ev.data.fd = sock_fd;
    if(epoll_ctl(epfd,EPOLL_CTL_ADD,sock_fd,&ev)<0){
            cout<<"epoll_ctl add error\n";
            exit(-1);
    }

       //while(true){    //由于使用非阻塞IO，需要不断读取，直到全部读取完毕
     int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);   //有nfds个fd发生事件
        for(int i=0;i<nfds;++i){
            char buf[sizeof(message)];
               // while(true){    //由于使用非阻塞IO，读取buffer，一次读取buf大小数据，直到全部读取完毕
                    bzero(&buf, sizeof(buf));
                    ssize_t bytes_read = read(events[i].data.fd, buf, sizeof(buf));
                    if(bytes_read > 0){
                        printf("message from client fd %d: %s\n", events[i].data.fd, buf);
                        
                    } else if(bytes_read == -1 && errno == EINTR){  //端正常中断、继续读取
                        printf("continue reading");
                        continue;
                    } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
                        printf("finish reading once, errno: %d\n", errno);
                        break;
                    } else if(bytes_read == 0){  //EOF，端断开连接
                        printf("EOF, client fd %d disconnected\n", events[i].data.fd);
                        close(events[i].data.fd);   //关闭socket会自动将文件描述符从epoll树上移除
                        break;
                    }
               //}
        }
//}

    //recv message
    //recv_message(sock_fd,m);

    //recv file
    recv_file(sock_fd,m);

}

//recv the info from the server
int recv_message(int sock_fd,message& m)
{
    
    char tmp[sizeof(message)];
    memset(tmp,0,sizeof(tmp)); //clear the  buffer

    if((read(sock_fd,&tmp,sizeof(tmp))) <0){
        cout<<"read the respon error";
        exit(3);
    }
    memcpy(&m,tmp,sizeof(m));//copy the message to the struct message m

    if(m.flag!= 1){
        cout<< "receive the  wrong respon from the server\n";
        return -3;
    }
    return 0;
}

//recv the file
int recv_file(int sock_fd,const message& m)
{
    //change dir to target dir 
    string path = CLI_STORAGE_PATH;
    chdir(path.c_str());
    mkdir("tmp1",RWXRWXRWX);
    path+="/tmp1";
    chdir(path.c_str());

    //creat a file named m.filename
    int fd;
    if(( fd =open(m.filename,O_RDWR|O_CREAT,RWXRWXRWX)) <0){
        cout<<"creat file:"<<m.filename<<" error~";
        return -1;
    }
    //read the file from socket
    off_t* buf = (off_t*)malloc(m.len);
    if( read(sock_fd,buf,m.len) <0 ){
        cout<<"read file from server error\n";
        free(buf);
        exit(7);
    }
    //open the new file and write to it
    FILE* fp = fdopen(fd,"wb");
    if(fwrite(buf,m.len,1,fp)<m.len){
        cout<<"write less into file:"<<m.filename<<endl;
        free(buf);
        return -1;
    }
    fclose(fp);
    free(buf);
    return 0;
}

//close
void Close()
{

}