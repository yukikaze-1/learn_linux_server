#include <iostream>
#include "wallpaper.h"
#include <string>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <sys/stat.h>
#include <stdlib.h>

#define HEAD 4
#define MAXSIZE 64

using std::string;
using std::cout;
using std::cin;
using std::endl;


unsigned int gif_count =0;
unsigned int jpg_jpeg_count =0;
unsigned int bmp_count = 0;
unsigned int tiff_count =0;
unsigned int png_count =0;

//chocie of which count we use
unsigned int* choice;

/*-1 means no type means error //not use
    0 means its a jpg or jpeg
    1 means its a gif 
    2 means its a bmp
    3 means its a tiff
    4 means its a png */
int flag = 0;


//取得输入的路径名
void Get_Input_Path(string& path)
{
    struct stat info;
    while(1){
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

}

//return the ".xxx"
string get_image_info(const string& path,const string& filename)
{
    string result;
    string fullname = path+"/"+filename;
    cout<<"filename:"<<fullname<<endl;

    int fd ;
    if((fd= open(fullname.c_str(),O_RDONLY)) ==-1){
        cout<<"open error\n";
    }
    FILE * fp ;
    if((fp= fdopen(fd,"r")) == NULL){
        cout<<"fdopen error\n";
    }
    //read the first 4Byte of file
    int head[HEAD];  
    for(int i=0;i<HEAD;++i){
        head[i] = fgetc(fp);
    }
    
    // we only use the first byte to choice ,if some have the same start,we will note it
    //and we set the flag by head[0] and head[1]
    switch (head[0])
    {
    case 255:
        result = ".jpg";
        ++jpg_jpeg_count;
        flag = 0;
        break;
    case 137:
        result = ".png";
        ++png_count;
        flag = 4;
        break;
    case 71:
        result = ".gif";
        ++gif_count;
        flag =1;
        break;
        //tiff and bmp have the same start of 0x4D,but tiff is 0x4D 0x4D or 0x49 0x49
        //the bmp is 0x4D 0x42
    case 73:
    case 77:
    if(head[1] == 66){
        result = ".bmp";
        ++bmp_count;
        flag = 2;
    }else{
        result= ".tiff";
        ++tiff_count;
        flag =3;
    }
        break;
    
    default:
    result = "NONE";
    flag =-1;
        break;
    }

    //decide use which count
    //and we set the choice by flag
        switch (flag)
    {
    case 0: //means jpg or jpeg
        choice =  &jpg_jpeg_count;
        break;
    case 1: //means gif
        choice =  &gif_count;
        break;
    case 2: //means bmp
        choice =  &bmp_count;
        break;
    case 3: //means tiff
        choice =  &tiff_count;
        break;    
    case 4: //means png
        choice =  &png_count;
        break;
    default:
        break;
    }

    if(fclose(fp) ==EOF){
        cout<<"close filestream error\n";
    }
    return result;
}

//fomat the string as "000000xxx"
string format(const string& name)
{
    string result;
    //count the digits of the number of count
    unsigned int n = *choice;
    unsigned int count =1;
    while((n/=10)>0){
        ++count;
    }
    //format the count out format
    switch (count)
    {
    case 1:
        result = "00000000"+name;
        break;
        case 2:
        result = "0000000"+name;
        break;
        case 3:
        result = "000000"+name;
        break;
        case 4:
        result = "00000"+name;
        break;
        case 5:
        result = "0000"+name;
        break;
        case 6:
        result = "000"+name;
        break;
        case 7:
        result = "00"+name;
        break;
        case 8:
        result = "0"+name;
        break;
    default:
        result = ""+name;
        break;
    }
    
    return result;
}

//rename the filename as "(GIF_)000000xxx.bbb"
int Rename(const char*oldname,const string & path,const string& filename)
{
    string back_name = get_image_info(path,filename);//get the ".xxx"

    //if the file is not a picture ,we skip it and go on
    if(back_name == "NONE"){
        return 0;
    }
    string newname = format(myto_string(*choice)) + back_name;//get the "000000xxx.xxx"
    
    if(back_name == ".gif"){
        newname = "GIF_"+ newname;
    }
        
        return rename(oldname,newname.c_str());
}

//将该文件夹内所有的图片按序命名为1、2、3...
void Rename_File(const string& path)
{
    
    struct dirent *fp;
    DIR *p = opendir(path.c_str());

    chdir(path.c_str());
    //sleep(30);
    while((fp = readdir(p)) != NULL){
        //skip the folder "." and ".."
        if(fp->d_name[0] == '.' ){
            continue;
        }
        if(Rename(fp->d_name,path,fp->d_name) == -1){
            std::cout<<"rename error:"<<fp->d_name<<std::endl;
            break;
        } 
    }

    cout<<"gif_count:"<<gif_count<<std::endl;
    cout<<"jpg_jpeg_c:"<<jpg_jpeg_count<<std::endl;
    cout<<"bmp_count:"<<bmp_count<<std::endl;
    cout<<"tiff_count:"<<tiff_count<<std::endl;
    cout<<"png_count:"<<png_count<<std::endl;
}
