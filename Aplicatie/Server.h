#ifndef Server_H
#define Server_H

#include "threadpool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>


#define BUFFER_SIZE 2048
#define URL_PARAMS_MAX_SIZE 10


typedef struct {
    char key[64];
    char value[64];
} URLParameters ;



class Server {
public:
    Server(int port, int number_of_threads);
    ~Server();

    void start();

private:
    void handleConnection(int client_socket);
    void handlePutPath(char* path);
    void handlePostPath(char* path);
    void handleStaticFile(const char* filename, const char* contentType, int client_socket);
    const char* getContentType(char* filename);

    int parseURLParam(char* path, URLParameters params[], int maxSize);
    void handleSearch(char* path, int client_socket);
    void handleGetPath(char* path, int client_socket);
    void handleMethods(const char* method, char* path, int client_socket);


    int server_fd;
    int port;
    struct sockaddr_in address;
    int opt;
    threadpool pool;
    int addrlen;



};

#endif // Server_H
