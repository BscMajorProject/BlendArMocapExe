#ifndef CGT_CLIENT_
#define CGT_CLIENT_


#include <sys/socket.h>
#include <sys/select.h>
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
        enum OpsFlag { READ=0, WRITE=1 };
        struct timeval timeout;
        absl::Status Connect();
        absl::Status SendChunks(std::string str);
        absl::Status RecvResp();
        void ProcessMessage(std::string msg);
        void ProcessMessage(char* msg);
        bool IsSelectable(int descriptor, OpsFlag flag);
    
    public:
        bool connected;
        void Send(char* msg);
        void Send(std::string msg);
    
    private:
        int port = 31597;
        int sock, client_fd, valread;
        char buffer[1] = { 0 };
        struct sockaddr_in serv_addr;
    };
}



#endif