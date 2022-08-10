
#include "client.h"


namespace BlendArMocap 
{
    Client::Client(){
        Init();
    }

    Client::Client(int port){
        this->port = port;
        Init();
    }

    void Client::Init(){
        absl::Status conn_status = Connect();
        if (conn_status.ok()) { 
            LOG(INFO) << "Client connected";
            this->connected = true; 
        }
        else { 
            LOG(INFO) << "Connection failed: " << conn_status;
            this-> connected = false; 
        }
    }

    absl::Status Client::Connect(){
        // Create socket
        if ((this->sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            return absl::AbortedError("Socket creation error."); 
        }

        this->serv_addr.sin_family = AF_INET;
        this->serv_addr.sin_port = htons(this->port);
  
        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
            return absl::AbortedError("Invalid adress - address not supported.");
        }

        // Connect to server
        if ((this->client_fd = connect(
                this->sock, (struct sockaddr*)&this->serv_addr, sizeof(this->serv_addr))) < 0) {
            return absl::AbortedError("Connection Failed.");
        }

        return absl::OkStatus();
    }

    // https://man7.org/linux/man-pages/man2/select.2.html
    bool Client::IsSelectable(int file_descriptor, OpsFlag flag){
        if (file_descriptor < 0) { return false; }

        // resetting timeout
        this->timeout.tv_sec = 3;
        this->timeout.tv_usec = 0;
        
        fd_set rds; // create selection set
        FD_ZERO(&rds); // clearing the set
        FD_SET(file_descriptor, &rds); // add file descriptor to set

        fd_set err_set;
        FD_ZERO(&err_set);
	    FD_SET(sock, &err_set);
        FD_SET(file_descriptor, &err_set);

         // check if selectable
        int selection;
        switch (flag){
            case READ:
            selection = select(file_descriptor + 1, &rds, NULL, &err_set, &timeout);
            break;
            case WRITE: 
            // rdwr to check if server didn't read
            fd_set wrs;
            FD_ZERO(&wrs);
	        FD_SET(sock, &wrs);
            FD_SET(file_descriptor, &wrs);
            selection = select(file_descriptor + 1, NULL, &wrs, &err_set, &timeout);
            break;
            default:
            selection = select(file_descriptor + 1, &rds, NULL, &err_set, &timeout);
            break;
        }

        if (selection == -1) {
            this->connected = false;
            return false;
        }
        else if ((selection == 1) && (FD_ISSET(sock, &rds)) && (!FD_ISSET(sock, &err_set))) {
            /* FD_ISSET(0, &rfds) will be true. */
            return true;
        }
        else {
            this->connected = false;
            LOG(ERROR) << "Pipe timeout @" << selection;
            close(file_descriptor);
            return false;
        }
    }

    void Client::ProcessMessage(char* msg) 
    {
        int size = strlen(msg) -1; //nullterminator

        std::ostringstream os;
        os << size << "|" << msg;
        absl::Status status = SendChunks(os.str());
        if (!status.ok()) { LOG(ERROR) << status; }
    }

    void Client::ProcessMessage(std::string msg) 
    {
        int size = msg.length();
        
        std::ostringstream os;
        os << size << "|" << msg;
        absl::Status status = SendChunks(os.str());
        if (!status.ok()) { LOG(ERROR) << status; }
    }

    absl::Status Client::SendChunks(std::string str) {
        const char *cstr = str.c_str();
        int remaining = strlen(cstr); 
        
        while (true) {
            if (!IsSelectable(this->sock, WRITE)) { return absl::AbortedError("Socket can not be selected."); }
            int n = send(this->sock, cstr, strlen(cstr), 0);
            remaining -= n;
            if (remaining == 0) {
                break;
            }
            cstr += n;
        }
        return absl::OkStatus();
    }

    absl::Status Client::RecvResp()
    {
        if (!IsSelectable(this->sock, READ)) { return absl::AbortedError("Socket can not be selected."); }
        this->valread = read(this->sock, this->buffer, sizeof(buffer));
        this->buffer[0] = '\0';
        return absl::OkStatus();
    }

    void Client::Send(char* msg){
        if (this->connected) {
            ProcessMessage(msg);
            absl::Status status = RecvResp();
            if (!status.ok()) { LOG(ERROR) << status; }
        }
    }

    void Client::Send(std::string msg){
        if (this->connected) {
            ProcessMessage(msg);
            absl::Status status = RecvResp();
            if (!status.ok()) { LOG(ERROR) << status; }
        }
    }

    Client::Client(const Client &other){
        this->sock = other.sock;
        this->client_fd = other.client_fd;
        this->valread = other.valread;
        this->port = other.port;
        this->connected = other.connected;
        this->serv_addr = other.serv_addr;
    }

    Client::~Client(){
        if (this->connected) {
            LOG(INFO) << "Closing connection";
            // shutdown(this->client_fd, SHUT_WR);
            // close(this->client_fd);
            shutdown(this->sock, SHUT_WR);
            close(this->sock);
        }
    }
}