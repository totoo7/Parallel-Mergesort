#include "client/client.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <sstream>
#include <vector>
#include <sys/time.h>

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

    int attempts = 3;
    bool connected = false;

    for (int i = 0; i < attempts; i++) {
        if (connect(client, (sockaddr*)&server, sizeof(server)) == 0) {
            connected = true;
            break;
        }
        perror("connect failed, retrying...");
    }

    std::ostringstream oss;

    int benchmark_flag;
    std::cin >> benchmark_flag;

    if (benchmark_flag == 1) {
        oss << 1;
    } else {
        std::string cmd = "MERGE_SORT";
        int n;
        int thread_count;

        std::cin >> n;
        std::cin >> thread_count;

        std::vector<int> arr(n);

        for (int i = 0; i < n; i++) {
            std::cin >> arr[i];
        }

        oss << 0 << " "
            << cmd << " "
            << n << " "
            << thread_count << " ";

        for (int x : arr) {
            oss << x << " ";
        }
    }

    std::string msg = oss.str();
    send(client, msg.c_str(), msg.size(), 0);

    char buffer[1024] = {0};
    read(client, buffer, sizeof(buffer));

    std::cout << "Server replied: " << buffer << std::endl;

    close(client);
}