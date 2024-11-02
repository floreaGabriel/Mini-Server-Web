#ifndef Server_H
#define Server_H

#include "threadpool.h"
#include "GetHandler.h"
#include "PutHandler.h"
#include "PostHandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>


class Server {
public:
    Server(int port, int number_of_threads);
    ~Server();

    void start();

private:
    void handleConnection(int client_socket);
    void handleMethods(const char* method, char* path, int client_socket, const char* buffer);


    int server_fd;
    int port;
    struct sockaddr_in address;
    int opt;
    threadpool pool;
    int addrlen;



};

#endif // Server_H