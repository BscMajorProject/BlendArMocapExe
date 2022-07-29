#include <iostream>


#define debug(msg) std::cout << msg << std::endl
#define log(msg) std::clog << msg << std::endl
#define err(msg) std::cerr << "\033[1;31m" << msg << "\033[0m\n"
#define warn(msg) std::cerr << "\033[1;33m" << msg << "\033[0m\n"

#define changed(src, other) ((src == other) ? false : true )
#define min(a, b) ((a < b) ? (a : b))
#define max(a, b) ((a > b) ? (a : b))
int main(){
    log("some message");
    err("some error");
    debug("some debug");
    warn("some warn");
}