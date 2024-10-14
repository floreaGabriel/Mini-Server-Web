#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>




int main(int argc, char* argv[])
{

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

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR || SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // configurarea adresei serverului

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // asculta pe toate interfetele de retea
    address.sin_port = htons(8080);

    // atasam socketul la port

    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)))
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


    int new_socket;

    if((new_socket = accept(server_fd, &address, (socklen_t*)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("Serverul a acceptat conexiunea pe portul 8080...\n");

    char buffer[1024] = {0};
    char *msg_to_send = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world";

    read(new_socket, buffer, 1024);
    printf("Cerere de la client:\n%s\n", buffer);

    send(new_socket, msg_to_send, strlen(msg_to_send), 0);
    printf("Raspuns trimis clientului\n");

    close(new_socket);

    return 0;
}