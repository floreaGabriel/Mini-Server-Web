#ifndef GET_HANDLER_H
#define GET_HANDLER_H


#include <string>

#define URL_PARAMS_MAX_SIZE 10
#define BUFFER_SIZE 2048

typedef struct {
    char key[64];
    char value[64];
} URLParameters ;

class GetHandler {
public:

    void handleGetPath(char* path, int client_socket);
    void handleGetSearch(char* path, int client_socket);
    int parseURLParam(char* path, URLParameters params[], int maxSize);
    void handleGetStaticFile(const char* filename, const char* contentType, int client_socket);
    const char* getContentType(char* filename);
};

#endif // GET_HANDLER_H