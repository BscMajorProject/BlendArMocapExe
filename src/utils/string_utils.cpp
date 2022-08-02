#include "string_utils.h"


#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

namespace BlendArMocap{
    std::string getstring(std::string path){
        std::ifstream t(path);
        std::stringstream buffer;
        buffer << t.rdbuf();
        return buffer.str();
    }

    std::string GetFileContents(std::string path){
        std::cout <<path << std::endl;

        std::ifstream t(path);
        std::string str;     
        
        t.seekg(0, std::ios::end);   
        std::cout << "found end " << t.end << std::endl;
        str.reserve(t.tellg());
        t.seekg(0, std::ios::beg);
        std::cout << t.beg << " // " << t.end << std::endl;

        str.assign((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
        std::cout << "fin" << std::endl;
        std::cout << str << std::endl;
        return str;
    }

}
