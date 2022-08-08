// https://www.geeksforgeeks.org/socket-programming-cc/

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
            LOG(WARNING) << "Connection failed: " << conn_status;
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

    void Client::Send(char* msg){
        if (this->connected) {
            send(this->sock, msg, strlen(msg), 0);
            this->valread = read(this->sock, this->buffer, 1024);
        }
    }

    void Client::Send(std::string msg){
        if (this->connected) {
            const char *cstr = msg.c_str();
            send(this->sock, cstr, strlen(cstr), 0);
            this->valread = read(this->sock, this->buffer, 1024);
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
            shutdown(this->client_fd, SHUT_RDWR);
            close(this->client_fd);
            shutdown(this->sock, SHUT_RDWR);
            close(this->sock);
        }
    }
}