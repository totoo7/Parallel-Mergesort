
#include "server/server.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

Server::Server(int port) : port(port) {}

void Server::run() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        return;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    hint.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr*)&hint, sizeof(hint)) == -1) {
        perror("bind failed");
        return;
    }
    
    if (listen(server_fd, 1) == -1) {
        perror("listen failed");
        return;
    }

    std::cout << "server listening on port " << port << "...\n";

    sockaddr_in client;
    socklen_t client_size = sizeof(client);

    int client_socket = accept(server_fd, (sockaddr*)&client, &client_size);
    if (client_socket == -1) {
        perror("client failed");
        return;
    }

    char buffer[1024] = {0};
    read(client_socket, buffer, sizeof(buffer));

    std::cout << "received: " << buffer << std::endl;

    const char* reply = "hello from server";
    send(client_socket, reply, strlen(reply), 0);

    close(client_socket);
    close(server_fd);
}