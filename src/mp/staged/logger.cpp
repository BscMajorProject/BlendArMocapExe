#include <iostream>

enum Serverity { DEBUG, INFO, WARN, ERR };

#define log(_serverity, msg) LogConfig(_serverity, msg)

void LogConfig(Serverity serverity, std::string msg){
    switch (serverity){
        case DEBUG:
        std::cout << msg << std::endl;
        break;
        case INFO:
        std::cout << msg << std::endl;
        break;
        case WARN:
        std::cerr << "\033[1;31m" << msg << "\033[0m\n";
        break;
        case ERR:
        std::cerr << "\033[1;33m" << msg << "\033[0m\n";
        break;
        default:
        std::cout << msg << std::endl;
        break;
    }
}


int main(){
    log(DEBUG, "some debug");
    log(WARN, "some warn");
    log(ERR, "some error");
    log(INFO, "some info");

}