
#include "server/server.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

Server::Server(int port) : port(port), thread_pool(std::thread::hardware_concurrency()) {}

void Server::handle_client(int client_socket) {
    std::cout << "start handling client " << std::this_thread::get_id() << std::endl; //! test
    char buffer[1024] = {0};
    ssize_t bytes = read(client_socket, buffer, sizeof(buffer));
    if (bytes == -1) {
        perror("read failed");
        close(client_socket);
        return;
    }

    
    std::cout << "received: " << buffer << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3)); //! test
    const char* reply = "hello from server";
    if (send(client_socket, reply, strlen(reply), 0) == -1) {
        perror("send failed");
    } 
    std::cout << "finish client " << std::this_thread::get_id() << std::endl; //! test
    close(client_socket);
    std::cout << "client disconnected\n";
}

void Server::run() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        return;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));


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

    while (true) {
        sockaddr_in client;
        socklen_t client_size = sizeof(client);
    
        int client_socket = accept(server_fd, (sockaddr*)&client, &client_size);
        if (client_socket == -1) {
            perror("client failed");
            continue;
        }

        std::cout << "client connected\n";
        thread_pool.enqueue([this, client_socket]() {
            handle_client(client_socket);
        });
    }

    close(server_fd);
}