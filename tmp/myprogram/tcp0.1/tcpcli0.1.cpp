#include "tcpcli0.1.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;


//get the target host by name or addr
void Get_Host_Info(struct sockaddr_in& servaddr)
{
    //choose use domain name or IP address
    char choice;
    while(1){
        cout<<"please choose input domain name or IP address:"\
                    "(D for domain,I for IP address)\n";
        cin>>choice;
        if(choice == 'D' || choice == 'I'){
            break;
        }else{
            continue;
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

    if(choice == 'D'){
        struct hostent *servinfo = gethostbyname(input_address.c_str());
        inet_pton(AF_INET,servinfo->h_addr_list[0],&servaddr.sin_addr.s_addr);
    }else{ //choice == "I"
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
   // if((setsockopt(sock_fd,SOL_SOCKET,SO_SNDBUF,)))
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
    }
    cout<<"open file:"<<fullname<<"success~\n";
    cout<<"file_fd in Get_Usr_Iput:"<<file_fd<<endl;
    cout<<"in Get_Usr_Iput: file length*:"<<info.st_size<<endl;
    return file_fd;
}

//send the file length to the server and wait for the respon 
int send_length(int sock_fd,struct message& m)
{
    char tmp[100];
    memset(tmp,0,sizeof(tmp)); //clear the buffer
    memcpy(tmp,&m,sizeof(m));//copy the message to the buffer

   if( write(sock_fd,&tmp,sizeof(tmp))  <0){
    cout<<"send message error\n";
    return -1;
   }
   cout<< " send file message: flag "<<m.flag<<"len:"<<m.len<<"filename:"<<m.filename<<" success~";
    return 0;
}

//B.send the image to the target host
void Send_To_Serv(int sock_fd,int file_fd,string& filename)
 {
    cout<<"file_fd in Send_To_Serv:"<<file_fd<<endl;
    //get the file stat in order to get the file length
    struct stat info;
    if((fstat(file_fd,&info)) == -1){
        cout<<"fstat error\n";
        exit(4);
    }
    cout<<"file length in Send_To_Serv:"<<info.st_size<<endl;

    //open the file stream
    FILE * fp ;
    if((fp= fdopen(file_fd,"r") ) == NULL){
        cout<<"open file stream error\n";
        exit(3);
    }
    
    //send the file length to the server and wait for the respon
    message m;
    m.flag = 1;
    m.len = info.st_size; 
    cout<<"m.len:"<<m.len<<endl;
    const char* p = filename.c_str();
    //const char* p = (const char * )malloc(sizeof(char) * filename.size());
    memcpy(m.filename,p,filename.size());
    if(send_length(sock_fd,m) <0){
        exit(5);
    }
    //size_t long unsigned int
    //off_t long int
    //malloc the sapce to store the picture that ready to send
    off_t* sendline = (off_t*)malloc(info.st_size);

    // store the picture in the buffer of sendline 
    fread(sendline,(ssize_t)info.st_size,1,fp);
    if(ferror(fp)){
        cout<<"fread error\n";
        exit(7);
    }

    if (write(sock_fd,sendline,(ssize_t)info.st_size)  == -1){
        cout<<"write to the socket error\n";
    }
    cout<<"send the file:"<<filename<<"success~\n";
    //close the file stream and fd and free the buffer
    free(sendline);
    fclose(fp);
    close(file_fd);
 }

//recv the length from the server
int recv_length(int sock_fd,message& m)
{
    char tmp[100];
    memset(tmp,0,sizeof(tmp)); //clear the  buffer

    if((read(sock_fd,&tmp,sizeof(tmp))) <0){
        cout<<"read the respon error";
        return -2;
    }
    memcpy(&m,tmp,sizeof(m));//copy the message to the struct message m

    if(m.flag!= 1){
        cout<< "receive the  wrong respon from the server\n";
        return -3;
    }
    return 0;
}

//mkdir for store the file receive from server
int store_file(off_t * buf,const message& m)
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
    //open the new file and write to it
    FILE* fp = fdopen(fd,"wb");
    if(fwrite(buf,m.len,1,fp)<m.len){
        cout<<"write less into file:"<<m.filename<<endl;
        return -1;
    }
    fclose(fp);

    return 0;
}

//C.receive the image that is renamed  (or dealed)
void Recv_From_Serv(int sock_fd)
{
    message m;
    if((recv_length(sock_fd,m)) < 0){
        exit(6);
    }
    off_t* buf = (off_t*)malloc(m.len);

    if( read(sock_fd,buf,m.len) <0 ){
        cout<<"read file from server error\n";
        free(buf);
        exit(7);
    }
    if(store_file(buf,m) < 0){
        cout<<"store the file error\n";
        free(buf);
        exit(8);
    }

    free(buf);
}


//close
void Close()
{

}