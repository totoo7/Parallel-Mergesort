#include "client/client.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <sstream>
#include <vector>

void Client::run() {
    int client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == -1) {
        perror("ERROR: socket failed.");
        return;
    } 

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);

    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    if (connect(client, (sockaddr*)&server, sizeof(server)) == -1) {
        perror("ERROR: connect failed.");
        return;
    }

    std::string cmd = "MERGE_SORT";
    
    int n;
    std::cin >> n;

    std::vector<int> arr(n);
    for (int i = 0; i < n; i++) 
        std::cin >> arr[i];
    
    std::ostringstream oss;
    oss << cmd << " " << n << " ";
    for (int x : arr) oss << x << " ";

    std::string msg = oss.str();
    send(client, msg.c_str(), msg.size(), 0);

    char buffer[1024] = {0};
    read(client, buffer, sizeof(buffer));

    std::cout << "Server replied: " << buffer << std::endl;

    close(client);
}