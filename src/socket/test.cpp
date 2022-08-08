#include "client.h"
#include "string.h"
#include <chrono>
#include <thread>

void some_test(){
    BlendArMocap::Client client = BlendArMocap::Client();
    std::string o_msg = "this is my other msg";
    char *msg = "this is my msg";
    for (int i = 0; i < 10; i++){ 
        std::string count = std::to_string(i);
        client.Send(o_msg);
        client.Send(msg);
        client.Send(count);
    }
    client.Send("0");
}

int main(){
    std::cout << "starting test" << std::endl;
    some_test();

    std::cout << "seme wait time" << std::endl;
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds
    sleep_for(nanoseconds(10));
    sleep_until(system_clock::now() + seconds(3));
    // some_test();
    std::cout << "stopped" << std::endl;
    return 0;
}