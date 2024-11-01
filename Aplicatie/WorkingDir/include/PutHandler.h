#ifndef PUT_HANDLER_H
#define PUT_HANDLER_H

#include <string>

class PutHandler {
public:
    void handlePutPath(const std::string& path, int client_socket, const char* buffer);
    void handleUpdate(const std::string& username, const std::string& newPassword, const std::string& filePath);
};

#endif // PUT_HANDLER_H