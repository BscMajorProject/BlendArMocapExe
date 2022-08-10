#ifndef CGT_IPC_PIPE_
#define CGT_IPC_PIPE_

#include "absl/status/status.h"
#include "glog/logging.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
// #include <unistd.h>
#include <string.h>
#include <sys/select.h>



namespace BlendArMocap
{
    class Pipe {
    public:
        Pipe();
        Pipe(const Pipe &other) = default;
        absl::Status Send(char *msg);
        absl::Status Send(std::string msg);
        bool connected;

    private:
        enum PipeFlag { READ=0, WRITE=1 };
        struct timeval timeout;
        char buffer[32];
        absl::Status Handshake();
        absl::Status RecvResp();
        absl::Status WriteMsg(char *msg);
        absl::Status WriteMsg(const char *msg);
        bool IsSelectable(int descriptor, PipeFlag flag);
    };
}

#endif