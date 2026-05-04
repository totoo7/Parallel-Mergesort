
#include "server/server.hpp"
#include "algorithms/merge_sort.hpp"
#include <iostream>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <algorithm>

Server::Server(int port) : port(port), thread_pool(std::thread::hardware_concurrency()) {}

void Server::handle_client(int client_socket) {
    char buffer[1024] = {0};
    ssize_t bytes = read(client_socket, buffer, sizeof(buffer));
    if (bytes == -1) {
        perror("ERROR: read failed.");
        close(client_socket);
        return;
    }

    
    std::cout << "Received: " << buffer << std::endl;
    std::istringstream iss(buffer);
    std::string cmd;
    int n;
    iss >> cmd >> n;
    if (cmd != "MERGE_SORT" || n <= 0) {
        std::string err = "ERROR: invalid request.";
        send(client_socket, err.c_str(), err.size(), 0);
        close(client_socket);
        return;
    }

    std::vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        if (!(iss>>arr[i])) {
            std::string err = "ERROR: invalid data.";
            send(client_socket, err.c_str(), err.size(), 0);
            close(client_socket);
            return;
        }
    }

    merge_sort(arr);

    std::ostringstream oss;
    for (int x : arr) {
        oss << x << " ";
    }

    std::string response = oss.str();

    if (send(client_socket, response.c_str(), response.size(), 0) == -1) {
        perror("ERROR: send failed.");
    } 
    close(client_socket);
    std::cout << "Client disconnected.\n";
}

void Server::run() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("ERROR: socket failed.");
        return;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));


    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    hint.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr*)&hint, sizeof(hint)) == -1) {
        perror("ERROR: bind failed.");
        return;
    }
    
    if (listen(server_fd, 1) == -1) {
        perror("ERROR: listen failed.");
        return;
    }

    std::cout << "Server listening on port: " << port << "...\n";

    while (true) {
        sockaddr_in client;
        socklen_t client_size = sizeof(client);
    
        int client_socket = accept(server_fd, (sockaddr*)&client, &client_size);
        if (client_socket == -1) {
            perror("ERROR: client failed.");
            continue;
        }

        std::cout << "Client connected.\n";
        thread_pool.enqueue([this, client_socket]() {
            handle_client(client_socket);
        });
    }

    close(server_fd);
}