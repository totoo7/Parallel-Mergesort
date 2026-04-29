#include "client/client.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

void Client::run() {
    int client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == -1) {
        perror("socket failed");
        return;
    } 

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);

    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    if (connect(client, (sockaddr*)&server, sizeof(server)) == -1) {
        perror("connect failed");
        return;
    }

    const char* msg = "Hello from client";
    send(client, msg, strlen(msg), 0);

    char buffer[1024] = {0};
    read(client, buffer, sizeof(buffer));

    std::cout << "Server replied: " << buffer << std::endl;

    close(client);
}