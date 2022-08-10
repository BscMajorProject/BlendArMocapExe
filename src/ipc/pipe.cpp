#include "pipe.h"

#define FIFO_OUT "/tmp/blendarmocap_OUT"
#define FIFO_IN "/tmp/blendarmocap_IN"


namespace BlendArMocap 
{
    Pipe::Pipe() 
    {
        // make sure fifo pipes exist
        mkfifo(FIFO_OUT, 0666);
        mkfifo(FIFO_IN, 0666);
        Handshake();
    }

    absl::Status Pipe::Send(char *msg) {
        if (this->connected) {
            absl::Status write_status = WriteMsg(msg);
            if (!write_status.ok()) { return write_status; }
            absl::Status recv_status = RecvResp();
            if (!recv_status.ok()) { return recv_status; }
        }
        return absl::OkStatus();
    }

    absl::Status Pipe::Send(std::string msg){
        if (this->connected) {
            const char *cstr = msg.c_str();
            absl::Status write_status = WriteMsg(cstr);
            if (!write_status.ok()) { return write_status; }
            absl::Status recv_status = RecvResp();
            if (!recv_status.ok()) { return recv_status; }
        }
        return absl::OkStatus();
    }

    // https://man7.org/linux/man-pages/man2/select.2.html    
    bool Pipe::IsSelectable(int file_descriptor, PipeFlag flag){
        if (file_descriptor < 0) { return false; }

        // resetting timeout
        this->timeout.tv_sec = 3;
        this->timeout.tv_usec = 0;
        
        fd_set set; // create selection set
        FD_ZERO(&set); // clearing the set
        FD_SET(file_descriptor, &set); // add file descriptor to set

        // check if selectable
        int selection;
        switch (flag){
            case READ:
            selection = select(file_descriptor + 1, &set, NULL, NULL, &timeout);
            break;
            case WRITE:
            selection = select(file_descriptor + 1, NULL, &set, NULL, &timeout);
            break;
            default:
            selection = select(file_descriptor + 1, &set, NULL, NULL, &timeout);
            break;
        }

        if (selection == -1) {
            this->connected = false;
            return false;
        }
        else if (selection) {
            return true;
        }
        else {
            this->connected = false;
            LOG(ERROR) << "Pipe timeout";
            close(file_descriptor);
            return false;
        }
    }

    absl::Status Pipe::Handshake() {
        // make sure pipe on the other hand has been started and is empty (may use larger buffer?)
        int in_pipe_desc = open(FIFO_IN, O_RDWR);
        if (IsSelectable(in_pipe_desc, READ)) {
            int num = read(in_pipe_desc, this->buffer, sizeof(this->buffer));
            if (num < 0) { return absl::AbortedError("Reading respone failed."); }
            this->connected = true;
        }
        this->buffer[0] = '\0';
        return absl::OkStatus();
    }

    absl::Status Pipe::WriteMsg(char *message){
        int out_pipe = open(FIFO_OUT, O_RDWR); // RDWR for selection (WROLNLY)
        if (!IsSelectable(out_pipe, WRITE)) { return absl::AbortedError("Input pipe cannot be opened."); }
        if (out_pipe < 0) { return absl::AbortedError("Output pipe cannot be opened."); }
        int num = write(out_pipe, message, strlen(message));
        if (num < 0) { return absl::AbortedError("Writing failed."); }
        close(out_pipe);
        return absl::OkStatus();
    }

    absl::Status Pipe::WriteMsg(const char *message){
        int out_pipe = open(FIFO_OUT, O_RDWR); // RDWR for selection (WRONLY)
        if (!IsSelectable(out_pipe, WRITE)) { return absl::AbortedError("Input pipe cannot be opened."); }
        if (out_pipe < 0) { return absl::AbortedError("Output pipe cannot be opened."); }
        int num = write(out_pipe, message, strlen(message));
        if (num < 0) { return absl::AbortedError("Writing failed."); }
        close(out_pipe);
        return absl::OkStatus();
    }

    absl::Status Pipe::RecvResp(){
        int in_pipe = open(FIFO_IN, O_RDWR); // RDWR for selection (RDONLY)
        if (!IsSelectable(in_pipe, READ)) { return absl::AbortedError("Input pipe cannot be opened."); }
        // Read pipe value for synching
        int num = read(in_pipe, this->buffer, sizeof(this->buffer));
        if (num < 0) { return absl::AbortedError("Reading respone failed."); }
        // Clear buffer.
        this->buffer[0] = '\0';
        close(in_pipe);
        return absl::OkStatus();
    }
}