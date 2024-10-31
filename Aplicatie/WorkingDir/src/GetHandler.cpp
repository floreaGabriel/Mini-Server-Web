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

int GetHandler::parseURLParam(char* path, URLParameters params[], int maxSize) {
    char* query = strstr(path, "?");
    if (!query) {
        return 0; // fara parametri
    }

    query++;  // sarim peste semnul "?"

    // forma url : /search?cautare=masina&culoare=rosu

    int paramCount = 0;
    char* token = strtok(query, "&");
    while (token && paramCount < maxSize) {
        char* equalSign = strchr(token, '=');
        if (equalSign) {
            // Separam cheia de valoare
            *equalSign = '\0';
            strcpy(params[paramCount].key, token);
            strcpy(params[paramCount].value, equalSign + 1);
            paramCount++;
        }
        token = strtok(NULL, "&");
    }
    return paramCount;
}

void GetHandler::handleGetSearch(char* path, int client_socket) {
    URLParameters params[URL_PARAMS_MAX_SIZE];
    int paramsCount = parseURLParam(path, params, URL_PARAMS_MAX_SIZE);

   
    if (paramsCount > 0) {
        char filename[64] = {0};
        for (int i = 0; i < paramsCount; i++) {
            // cautam parametrul specific "cautare"
            if (strcmp(params[i].key, "cautare") == 0) {
                snprintf(filename, sizeof(filename), "%s.html", params[i].value);
                break;
            }
        }
        if (strlen(filename) > 0) {
            // deschidem fisierul corespunzator termenului de cautare
            FILE* file = fopen(filename, "r");
            if (file == NULL) {
                char response[] = "HTTP/1.1 404 NOT FOUND\r\nContent-Type: text/html\r\n\r\n"
                                "<html><head><title>404 Not Found</title></head>"
                                "<body><h1>Pagina nu a fost găsită!</h1></body></html>";
                send(client_socket, response, strlen(response), 0);
                return;
            }

            // trimitere header de succes
            char response_header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
            send(client_socket, response_header, strlen(response_header), 0);

            // citim si trimitem continutul fisierului
            char file_buffer[BUFFER_SIZE];
            size_t bytes_read_from_file;
            while ((bytes_read_from_file = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0) {
                send(client_socket, file_buffer, bytes_read_from_file, 0);  
            }

            fclose(file);
        } else {
            // daca nu exista parametrul "cautare", returnam 400 BAD REQUEST
            char response[] = "HTTP/1.1 400 BAD REQUEST\r\nContent-Type: text/html\r\n\r\n"
                              "<html><head><title>400 Bad Request</title></head>"
                              "<body><h1>Parametrul de căutare nu a fost specificat!</h1></body></html>";
            send(client_socket, response, strlen(response), 0);
        }
    } else {
        // fara parametri, raspundem cu 400 BAD REQUEST
        char response[] = "HTTP/1.1 400 BAD REQUEST\r\nContent-Type: text/html\r\n\r\n"
                          "<html><head><title>400 Bad Request</title></head>"
                          "<body><h1>Parametrii nu au fost furnizați!</h1></body></html>";
        send(client_socket, response, strlen(response), 0);
    }
}


void GetHandler::handleGetPath(char* path, int client_socket) {
    char* questionMarkPos = strstr(path, "?");
    char filename[256];

        
    if (questionMarkPos) {
        // daca exista parametrii în URL, decupam partea inainte de "?"
        size_t pathLen = questionMarkPos - path;
        strncpy(filename, path, pathLen);

        filename[pathLen] = '\0';  
        char *p = strdup(filename);

        if (filename[0] == '/' && filename[1] != '\0') {
            strcpy(filename, p + 1);
        }
    } else {
        // daca nu exista parametrii, folosim intregul path
        if (strcmp(path, "/") == 0) {
            strcpy(filename, "index.html");  // implicit la index.html
        } else {
            strcpy(filename, path + 1);  // scoatem slash-ul din path
        }
    }


    if (strstr(filename, "search") != NULL) {
        handleGetSearch(path, client_socket);
    } else {
        // Fișiere statice
        const char* contentType = getContentType(filename);
        handleGetStaticFile(filename, contentType, client_socket);  
    }
}


const char* GetHandler::getContentType(char* filename) {
    if (strstr(filename, ".html") != NULL) return "text/html";
    if (strstr(filename, ".css") != NULL) return "text/css";
    if (strstr(filename, ".js") != NULL) return "application/javascript";
    if (strstr(filename, ".jpg") != NULL || strstr(filename, ".jpeg") != NULL) return "image/jpeg";
    if (strstr(filename, ".png") != NULL) return "image/png";
    return "text/plain";  // default, daca nu stim tipul
}

