#include <iostream>
//for lstat and struct stat
#include <sys/stat.h>
//for opendir + readdir + closedir and struct dirent
#include <dirent.h>
#include <stdlib.h>
#include <cstring>
#include <errno.h>
#include <utility>

using namespace  std;

string fullpath_name;
long long count_file =0;
int count_dir =0;

void showfile(const string  path)
{
    count_dir++;
    cout<<"*"<<count_dir<<"*"<<"Now in path:"<<path<<endl;
    DIR * current_dir = opendir(path.c_str());
    struct dirent* dir_line ;
    string name  = path;
  
    while((dir_line = readdir(current_dir))!=NULL){

        struct stat info_in;
        name.clear();
        name = path;
        if(*--name.end() != '/')
            name+='/';
        name +=dir_line->d_name; 
       
        if(*--name.end() == '.')
            continue;

        if( stat(name.c_str(),&info_in) != 0){
            cout<<strerror(errno)<<endl;
        }
        if(S_ISDIR(info_in.st_mode) == 0){//NOT A DIR
        count_file++;
        cout<<"*"<<count_file<<"*"<<name<<endl;
        continue;
        }
        //is a directory
        showfile(name);
    }

    closedir(current_dir);
}


int main(int argc,char ** argv)
{
    if(argc != 2){
        cout<<"use: <fullpath> to start the program"<<endl;
        exit(0);
    }

    //input the full path:
    fullpath_name = argv[1];
    
    //recursal the path
    showfile(fullpath_name);

    return 0;
}
