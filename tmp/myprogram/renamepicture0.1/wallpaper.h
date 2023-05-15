

#ifndef WALL_PAPER_H_
#define WALL_PAPER_H_
    #include <string>
    #include <stdio.h>
    #include <sstream>

    //get the path
    void Get_Input_Path(std::string& path);

    //rename the picture in order of 1、2、3...
    void Rename_File(const std::string& path);
    
    //the package of rename
    int Rename(const char*oldname,int newname,const std::string& path,const std::string& filename);
   
   //our to_string func
   template<typename T>
   std::string myto_string(const T& n)
   {
        std::ostringstream stm;
        stm<<n;
        return stm.str();
   }
    //get the image info
   std::string get_image_info(const std::string& path,const std::string& filename);

#endif
