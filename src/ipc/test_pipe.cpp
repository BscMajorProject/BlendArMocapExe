#include "pipe.h"


int main(){
    BlendArMocap::Pipe pipe = BlendArMocap::Pipe();
    std::string msg = "another msg";
    for (int i = 0; i < 100; i++) {
        pipe.Send(msg);
    }
}