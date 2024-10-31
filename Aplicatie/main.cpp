#include "threadpool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define NUM_THREADS 10  

//milea tesh branch

#define PORT 8080
#define BUFFER_SIZE 2048
#define URL_PARAMS_MAX_SIZE 10

typedef struct {
    char key[64];
    char value[64];
} URLParameters ;

void handlePutPath(char* path) {

}

void handlePostPath(char* path) {

}

void handleStaticFile(const char* filename, const char* contentType, int client_socket) {
    
    FILE* file = NULL;

    if (strstr(contentType, "image") != NULL) {
        file = fopen(filename, "rb");  // deschidem in modul binar
    } else {
        file = fopen(filename, "r");   // deschidem in modul text
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

const char* getContentType(char* filename) {
    if (strstr(filename, ".html") != NULL) return "text/html";
    if (strstr(filename, ".css") != NULL) return "text/css";
    if (strstr(filename, ".js") != NULL) return "application/javascript";
    if (strstr(filename, ".jpg") != NULL || strstr(filename, ".jpeg") != NULL) return "image/jpeg";
    if (strstr(filename, ".png") != NULL) return "image/png";
    return "text/plain";  // default, daca nu stim tipul
}


int parseURLParam(char* path, URLParameters params[], int maxSize) {

    char* query = strstr(path, "?");
    if (!query) {
        return 0; // Fara parametri
    }

    query++;  // Sarim peste semnul "?"

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

void handleSearch(char* path, int client_socket) {

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

void handleGetPath(char* path, int client_socket) {
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
        handleSearch(path, client_socket);
    } else {
        // Fișiere statice
        const char* contentType = getContentType(filename);
        handleStaticFile(filename, contentType, client_socket);  
    }
}




void handleMethods(const char* method, char* path, int client_socket)
{
    if (strcmp(method, "GET") == 0)
    {
        handleGetPath(path, client_socket);
    } else if (strcmp(method, "POST") == 0) {
        handlePostPath(path);
    } else if (strcmp(method, "PUT") == 0) {
        handlePutPath(path);
    }

}

void handleConnection(int client_socket)
{
    printf("Thread %ld procesează conexiunea pe socketul %d\n", std::this_thread::get_id(), client_socket);

    char buffer[BUFFER_SIZE];
    int bytesRead;

    bytesRead = read(client_socket, buffer, BUFFER_SIZE);
    if(bytesRead < 0)
    {
        perror("Eroare la citirea requestului primit de la client!\n");
        exit(client_socket);
        return;
    }

    buffer[bytesRead] = '\0';
    printf("\n\n%s\n\n", buffer);

    char method[16], path[256]; 
    sscanf(buffer, "%s %s", method, path);

    handleMethods(method, path, client_socket);

    close(client_socket);

}

int main(int argc, char* argv[])
{
    threadpool pool(NUM_THREADS);

    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);


    // crearea unui socket
    // AF_INET pentru Address family - internet adrese ipv4 xxx.xxx.xxx.xxx
    // SOCK_STREAM inseamna socketul va folosi TCP
    // SOCK_DGRAM inseamna socketul va folosi UDP

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // atasam socketul la un port

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }


    // configurarea adresei serverului

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // asculta pe toate interfetele de retea
    address.sin_port = htons(8080);

    // atasam socketul la port

    if(::bind(server_fd, (struct sockaddr*)&address, sizeof(address)))
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // punem serverul pe ascultare

    if(listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Serverul asculta pe portul 8080...\n");


    int client_socket;


    
    while (1)
    {
        int client_socket;
        if ((client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printf("Conexiune acceptata pe socketul %d\n", client_socket);

        // Adaugăm o funcție lambda în coada threadpool-ului care să proceseze conexiunea
        pool.enqueue([client_socket] {
            handleConnection(client_socket);
        });
    }


    


    return 0;
}