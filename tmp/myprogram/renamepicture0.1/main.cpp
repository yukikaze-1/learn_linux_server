#include <iostream>
#include "wallpaper.h"
#include <string>

//#define MAXSIZE 4096

using std::string;


int main()
{
    string path = "hello";
    //取得输入的路径名
    Get_Input_Path(path);

    //将该文件夹内所有的图片按序命名为1、2、3...
    Rename_File(path);

    std::cout<<"Done\n";

    return 0;
}