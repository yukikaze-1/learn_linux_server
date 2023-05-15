#include <iostream>
#include "wallpaper.h"
#include <string>

/*1.add new function to resolve the recursive file
  *2.add new function to save the count of pictures ,so we can continue the count
  *when we start the program next time  
  *3.add new function that we can move the same catogory of pictures into a named
  *folder.eg: all the *.jpg are move to the folder: JPG,*.gif moves to :GIF 
  */


using std::string;
using std::cout;


int main()
{
    string path;
    
    Get_Input_Path(path);

    Rename_File(path);

    std::cout<<"Done\n";

    return 0;
}