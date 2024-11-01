#include "PutHandler.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <sys/socket.h>

bool PutHandler::handleUpdate(const std::string& oldUsername, const std::string& newUsername, const std::string& newPassword, const std::string& filePath){
   
    std::ifstream inputFile(filePath);

    std::vector<std::string> lines;
    std::string line;
    bool found = false;

    while(getline(inputFile, line)) {
        std::istringstream iss(line);

        std::string currName, currPassword;

        if (iss >> currName >> currPassword) {
            // daca gasim user in fisier
            if (currName == oldUsername) {
                line = newUsername + " " + newPassword;
                found = true;
            }
        }

        lines.push_back(line);
    }

    inputFile.close();

    
    std::ofstream outputFile(filePath);

    for (const auto&updatedLine : lines)
    {
        outputFile << updatedLine << "\n";
    }

    outputFile.close();

    if (!found)
    {
        std::cout << "Username not found in the file.\n";
        return false;
    }
    else 
    {
        std::cout << "Username found in the file.\n";
        return true;
    }


}

void PutHandler::handlePutRequest(const std::string& path, int client_socket, const char* buffer) {
    
    // de continuat functia de handle put request
    // requestul de put vine cu datele de forma username=nume&password=123
    
    if (path == "/update-user") {

        std::string body(buffer);

        std::unordered_map<std::string, std::string> params;
        // cautam inceputul de payload ...deobicei inceputul de payload este marcat printr o linie goala
        size_t payloadStart = body.find("\r\n\r\n");
        if (payloadStart != std::string::npos)
        {
            payloadStart += 4;
        }

        std::istringstream payload(body.substr(payloadStart));

        std::string key, value;

        while(std::getline(payload, key, '=') && std::getline(payload, value, '&')) {
            params[key] = value;
        }
        
        // daca gasim in parametrii cheile username si password
        if (params.find("oldUsername") != params.end() && params.find("password") != params.end() && 
            params.find("username") != params.end()) {

            std::string oldUsername = params["oldUsername"];
            std::string newUsername = params["username"];
            std::string newPassword = params["password"];

            if (handleUpdate(oldUsername, newUsername, newPassword, "resources/accounts.txt") == false)
            { 
                const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nUpdate stopped!Initial username is invalid!";
                send(client_socket, response, strlen(response), 0);
            }
            else
            {
                // dupa ce s au modificat datele trimitem un raspuns 
                const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nUpdate successful!";
                send(client_socket, response, strlen(response), 0);
            }
            
        } else {
            // trimitem un mesaj de eroare
            const char* response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nMissing username or password!";
            send(client_socket, response, strlen(response), 0);
        }
    }
    else {
        // trimitem un mesaj de eroare deoarece nu s a gasit acest request de PUT
        const char* response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nInvalid path!";
        send(client_socket, response, strlen(response), 0);
    }
    
}