#include "GetHandler.h"
#include <fstream>
#include <sys/socket.h>  // Pentru funcția send
#include <cstring>

void GetHandler::handleGetStaticFile(const char* filename, const char* contentType, int client_socket) {
    FILE* file = NULL;

    std::string fullPath = "resources/" + std::string(filename);

    if (strstr(contentType, "image") != NULL) {
        file = fopen(fullPath.c_str(), "rb");  // deschidem in modul binar
    } else {
        file = fopen(fullPath.c_str(), "r");   // deschidem in modul text
    }

    if (file == NULL) {
        // rerturn error 404 page not found
        char response[] = "HTTP/1.1 404 NOT FOUND\r\nContent-Type: text/html\r\n\r\n"
                          "<html><head><title>404 Not Found</title></head>"
                          "<body><h1>Pagina nu a fost găsită!</h1></body></html>";
        send(client_socket, response, strlen(response), 0);
        return;
    }

    // trimitere header de succes
    char response_header[BUFFER_SIZE];
    snprintf(response_header, sizeof(response_header), "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", contentType);
    send(client_socket, response_header, strlen(response_header), 0);

    // citim si trimitem continutul fișierului
    char file_buffer[BUFFER_SIZE];
    size_t bytes_read_from_file;
    while ((bytes_read_from_file = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0) {
        send(client_socket, file_buffer, bytes_read_from_file, 0);
    }

    fclose(file);
}


void GetHandler::parseURLParam(const std::string& path) {
    m_parametrii.clear();

    size_t questionMarkPos = path.find("?");

    // npos inseamna ca nu am gasit o valoare valida
    if (questionMarkPos == std::string::npos) return ;

    std::string query = path.substr(questionMarkPos + 1); // extragem partea de dupa "?"

    size_t pos = 0;

    // query poate arata asa: /search?cautare=masina&culoare=rosu
    while ((pos = query.find("&")) != std::string::npos) {
        std::string token = query.substr(0, pos);   

        size_t equalSignPos = token.find("=");
        // daca nu gasim egal impartim in cheie si valoare
        if (equalSignPos != std::string::npos) {
            std::string key = token.substr(0, equalSignPos);
            std::string value = token.substr(equalSignPos + 1);
            m_parametrii[key] = value;
        }

        query.erase(0, pos + 1);
    }


    size_t equalSignPos = query.find("=");
    if (equalSignPos != std::string::npos)
    {
        std::string key = query.substr(0, equalSignPos);
        std::string value = query.substr(equalSignPos + 1);
        m_parametrii[key] = value;
    }
}


void GetHandler::handleGetSearch(int client_socket) {
   
    auto searchParam = m_parametrii.find("cautare");
    if (searchParam != m_parametrii.end()) // daca am gasit parametrul cautare
    {
        std::string filename = searchParam->second + ".html";

        
        FILE* file = fopen(("resources/" + filename).c_str(), "r");
        if (file == NULL) {
            const char* response = "HTTP/1.1 404 NOT FOUND\r\nContent-Type: text/html\r\n\r\n"
                                   "<html><head><title>404 Not Found</title></head>"
                                   "<body><h1>Pagina nu a fost găsită!</h1></body></html>";
            send(client_socket, response, strlen(response), 0);
            return;
        }

        const char* contentType = getContentType(filename.c_str());
        char response_header[BUFFER_SIZE];
        snprintf(response_header, sizeof(response_header), "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", contentType);
        send(client_socket, response_header, strlen(response_header), 0);

        char file_buffer[BUFFER_SIZE];
        size_t bytes_read_from_file;
        while ((bytes_read_from_file = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0) {
            send(client_socket, file_buffer, bytes_read_from_file, 0);
        }
        fclose(file);
    }
    else 
    {
        const char* response = "HTTP/1.1 400 BAD REQUEST\r\nContent-Type: text/html\r\n\r\n"
                               "<html><head><title>400 Bad Request</title></head>"
                               "<body><h1>Parametrul de căutare nu a fost specificat!</h1></body></html>";
        send(client_socket, response, strlen(response), 0);
    }
}


void GetHandler::handleGetPath(const std::string& path, int client_socket) {

    parseURLParam(path);

    std::string basePath = path;
    size_t questionMarkPos = path.find("?");
    if (questionMarkPos != std::string::npos && questionMarkPos == path.size() - 1)
    {
        basePath = path.substr(0, questionMarkPos);
    }

    std::cout<< "Path: " << basePath << "\n";

    if (m_parametrii.find("cautare") != m_parametrii.end())
    {
        handleGetSearch(client_socket);
    }
    else 
    {
        std::string filename;
        if (basePath == "/")
        {
            filename = "index.html";
        }
        else
        {
            filename = basePath.substr(1);
        }
        std::cout<< "Filename: " << filename << "\n";
        handleGetStaticFile(filename.c_str(), getContentType(filename.c_str()), client_socket);
    }
}


const char* GetHandler::getContentType(const char* filename) {
    if (strstr(filename, ".html") != NULL) return "text/html";
    if (strstr(filename, ".css") != NULL) return "text/css";
    if (strstr(filename, ".js") != NULL) return "application/javascript";
    if (strstr(filename, ".jpg") != NULL || strstr(filename, ".jpeg") != NULL) return "image/jpeg";
    if (strstr(filename, ".png") != NULL) return "image/png";
    return "text/plain";  // default, daca nu stim tipul
}

