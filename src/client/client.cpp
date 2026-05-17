#include "client/client.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <sstream>
#include <vector>
#include <sys/time.h>

void print_help() {
    std::cout << "===== Parallel Merge Sort Client =====\n";
    std::cout << "Input 'exit' to disconnect\n";
    std::cout << "Input 'help' to get this menu\n";
    std::cout << "\n";
    std::cout << "Choose mode:\n";
    std::cout << "0 -> normal sort\n";
    std::cout << "1 -> benchmark mode\n";
    std::cout << "\n";
    std::cout << "Input format:\n";
    std::cout << "0 <array_size> <thread_count> <numbers...>\n";
    std::cout << "Example:\n";
    std::cout << "0 5 4 5 4 3 2 1\n";
    std::cout << "\n";
    std::cout << "Benchmark format:\n";
    std::cout << "1\n";
    std::cout << "======================================\n\n";
}

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
        perror("Connect failed, retrying...");
    }
    if (!connected) {
        std::cerr << "ERROR: cannot connect to server\n";
        close(client);
        return;
    }
    print_help();
    while (true) {
        std::ostringstream oss;
        std::string first_input;
        std::cin >> first_input;
        if (first_input == "exit") {
            send(client, first_input.c_str(), first_input.size(), 0);
            break;
        }
        if (first_input == "help") {
            print_help();
            continue;
        }
        int benchmark_flag = std::stoi(first_input);

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

    }
    close(client);
}