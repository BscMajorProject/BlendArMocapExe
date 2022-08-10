#include "client.h"
#include "string.h"


int main(){
    BlendArMocap::Client client = BlendArMocap::Client();
    std::ostringstream os;
    for (int i = 0; i < 100; i++){
        os << i << " ";
    }

    auto msg = os.str();
    for (int i = 0; i < 10; i++){ 
        client.Send(msg);
    }

    return 0;
}