#include "PutHandler.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

void PutHandler::handleUpdate(const std::string& username, const std::string& newPassword, const std::string& filePath){
    std::ifstream inputFile(filePath);

    std::vector<std::string> lines;
    std::string line;
    bool found = false;

    while(getline(inputFile, line)) {
        std::istringstream iss(line);

        std::string currName, currPassword;

        if (iss >> currName >> currPassword) {
            // daca gasim user in fisier
            if (currName == username) {
                line = username + " " + newPassword;
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
    }
    else 
    {
        std::cout << "Username found in the file.\n";
    }


}

void PutHandler::handlePutPath(const std::string& path, int client_socket, const char* buffer) {
    
    // de continuat functia de handle put request
    
}