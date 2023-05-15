

#ifndef WALL_PAPER_H_
#define WALL_PAPER_H_
    #include <string>
    #include <stdio.h>
    #include <sstream>
    //the format "000000xxx" is limited
    #define LEN_LIMIT 9

    //get the path
    void Get_Input_Path(std::string& path);

    //rename the picture in order of 1、2、3...
    void Rename_File(const std::string& path);
    
    //the package of rename
    int Rename(const char*oldname,const std::string& path, const std::string& filename);
   
   //fomat the string as "000000xxx"
   std::string format(std::string& name);
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
