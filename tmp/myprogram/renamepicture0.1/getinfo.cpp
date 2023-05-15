#include <iostream>
#include <sys/stat.h>
#include <string>
#include  <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#define HEAD 4

using namespace std;


int main()
{
    string path,filename;
    cout<<"enter the file path:\n";
    cin>>path;
    cout<<"input the file name:\n";
    cin>>filename;

    string fullname = path+"/"+filename;
    cout<<"filename:"<<fullname<<endl;

    int fd ;
    if((fd= open(fullname.c_str(),O_RDONLY)) ==-1){
        cout<<"open error\n";
        return 0;
    }
    FILE * fp = fdopen(fd,"r");
    //read the first 4Byte of file
    int head[HEAD];  
    for(int i=0;i<HEAD;++i){
        head[i] = fgetc(fp);
        printf("%#04x",head[i]);
    }
    cout<<endl<<"input done\n";
    

    if(fclose(fp) ==EOF){
        cout<<"close filestream error\n";
        return 0;
    }
    /*
    chdir(path.c_str());
    struct stat buff;
    struct stat* buf = &buff;
    if(stat(path.c_str(),buf) == -1){
        cout<<"stat error\n";
    }else{
    cout<<buf->st_mode<<endl;
    cout<<buf->st_ino<<endl;
    cout<<buf->st_dev<<endl;
    cout<<buf->st_rdev<<endl;
    cout<<buf->st_nlink<<endl;
    cout<<buf->st_uid<<endl;
    cout<<buf->st_gid<<endl;
    cout<<buf->st_size<<endl;
    cout<<buf->st_atim.tv_nsec<<endl;
    cout<<buf->st_atim.tv_sec<<endl;
    cout<<buf->st_mtim.tv_nsec<<endl;
    cout<<buf->st_mtim.tv_sec<<endl;
    cout<<buf->st_ctim.tv_nsec<<endl;
    cout<<buf->st_ctim.tv_sec<<endl;
    cout<<buf->st_blksize<<endl;
    cout<<buf->st_blocks<<endl;
    }
    */
    return 0;
}