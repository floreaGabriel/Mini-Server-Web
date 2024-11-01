#ifndef GET_HANDLER_H
#define GET_HANDLER_H


#include <string>
#include <iostream>
#include <unordered_map>

#define URL_PARAMS_MAX_SIZE 10
#define BUFFER_SIZE 2048


class GetHandler {
public:

    void handleGetPath(const std::string& path, int client_socket);
    void handleGetSearch(int client_socket);
    
    void handleGetStaticFile(const char* filename, const char* contentType, int client_socket);
    const char* getContentType(const char* filename);
private:

    void parseURLParam(const std::string& path);

    std::unordered_map<std::string, std::string> m_parametrii;
};

#endif // GET_HANDLER_H