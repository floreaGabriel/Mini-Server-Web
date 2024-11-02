#ifndef PostHandler_H
#define PostHandler_H

#include <string>

class PostHandler {
public:
    PostHandler();
    ~PostHandler();

    void handlePostRequest(const std::string& path, int client_socket, const char* buffer);

private:
    std::string extractPayLoad(const char* buffer);
    bool usernameExists(const std::string& filename, const std::string& username);
    std::string getPasswordForUsername(const std::string& filename, const std::string& username);
    void handleRegister(int client_socket, const std::string& payload);
    void handleSignIn(int client_socket, const std::string& payload);
    void parseCredentials(const std::string& payload, std::string& username, std::string& password);
};

#endif // PostHandler_H
