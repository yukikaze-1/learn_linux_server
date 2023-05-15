#include <iostream>
#include "wallpaper.h"
#include <string>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define HEAD 4

using std::string;
using std::cout;
using std::cin;
using std::endl;
//取得输入的路径名
void Get_Input_Path(string& path)
{
    std::cout<<"please enter the picture files path:\n";
    std::cin>>path;
}

string get_image_info(const string& path,const string& filename)
{
    string result;
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
    }
    
    // we only use the first byte to choice ,if some hava the same start,we will note it
    switch (head[0])
    {
    case 255:
        result = ".jpg";
        break;
    case 137:
        result = ".png";
        break;
    case 71:
        result = ".gif";
        break;
        //tiff and bmp have the same start of 0x4D,but tiff is 0x4D 0x4D or 0x49 0x49
        //the bmp is 0x4D 0x42
    case 73:
    case 77:
    if(head[1] == 66){
        result = ".bmp";
    }else{
        result= ".tiff";
    }
        break;
    
    default:
    result = "NONE";
        break;
    }

    if(fclose(fp) ==EOF){
        cout<<"close filestream error\n";
    }
    return result;
}

int Rename(const char*oldname,int newname,const string & path,const string& filename)
{
    string name = get_image_info(path,filename);
    if(name == "NONE"){
        return 0;
    }
        string nname = myto_string(newname) + name;
        return rename(oldname,nname.c_str());
}

//将该文件夹内所有的图片按序命名为1、2、3...
void Rename_File(const string& path)
{
    int count = 0;
    struct dirent *fp;
    DIR *p = opendir(path.c_str());

    chdir(path.c_str());
    while((fp = readdir(p)) != NULL){
        if(fp->d_name[0] == '.' ){
            continue;
        }
        if(Rename(fp->d_name,count,path,fp->d_name) == -1){
            std::cout<<"rename error:"<<fp->d_name<<std::endl;
            break;
        } 
        ++count;
    }
    
    
}
