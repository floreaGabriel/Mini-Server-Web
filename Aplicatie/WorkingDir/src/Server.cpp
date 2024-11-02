#include "Server.h"

Server::Server(int port, int number_of_threads) : port(port),opt(1),pool(number_of_threads)
{
    addrlen = sizeof(address);

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

}

Server::~Server() {
}

void Server::handleMethods(const char* method, char* path, int client_socket, const char* buffer)
{
    if (strcmp(method, "GET") == 0)
    {
        GetHandler* get = new GetHandler();
        get->handleGetPath(path, client_socket);
        free(get);
    } else if (strcmp(method, "POST") == 0) {
        PostHandler* post = new PostHandler();
        post->handlePostRequest(path,client_socket,buffer);
        free(post);
    } else if (strcmp(method, "PUT") == 0) {
        PutHandler* put = new PutHandler();
        put->handlePutRequest(path, client_socket, buffer);
        free(put);
    }
}

void Server::handleConnection(int client_socket)
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


    handleMethods(method, path, client_socket, buffer);

    close(client_socket);

}

void Server::start()
{
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
        pool.enqueue([this, client_socket] {
            handleConnection(client_socket);
        });
    }
}