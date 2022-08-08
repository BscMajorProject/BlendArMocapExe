#ifndef CGT_CLIENT_
#define CGT_CLIENT_


#include <sys/socket.h>
#include <arpa/inet.h>
#include "absl/status/status.h"
#include <unistd.h>
#include "glog/logging.h"

namespace BlendArMocap 
{
    class Client {
    public:
        Client();
        Client(int port);
        Client(const Client &other);
        ~Client();

    private:
        void Init();
        absl::Status Connect();
    
    public:
        bool connected;
        void Send(char* msg);
        void Send(std::string msg);
    
    private:
        int port = 31597;
        int sock, client_fd, valread;
        char buffer[32767] = { 0 };
        struct sockaddr_in serv_addr;
    };
}



#endif