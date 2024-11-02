#ifndef PUT_HANDLER_H
#define PUT_HANDLER_H

#include <string>

class PutHandler {
public:
    void handlePutRequest(const std::string& path, int client_socket, const char* buffer);
    bool handleUpdate(const std::string& oldUsername, const std::string& username, const std::string& newPassword, const std::string& filePath);
};

#endif // PUT_HANDLER_H