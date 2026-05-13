
#include "server/server.hpp"
#include "algorithms/merge_sort.hpp"
#include <iostream>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <algorithm>
#include <random>
#include <chrono>

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
    int benchmark_flag;
    iss >> benchmark_flag;
    if (benchmark_flag == 1) {

        std::vector<int> thread_counts = {1, 2, 4, 8};

        const int ARRAY_SIZE = 100000;
        const int RUNS_PER_TEST = 5;

        std::stringstream response;

        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<> dist(1, 1000000);

        std::vector<int> original(ARRAY_SIZE);

        for (int& x : original) {
            x = dist(gen);
        }

        double serial_time = 0.0;

        for (int run = 0; run < RUNS_PER_TEST; run++) {
            std::vector<int> arr = original;
            ThreadPool pool(1);
            auto start = std::chrono::high_resolution_clock::now();
            parallel_merge_sort(arr, pool);
            auto end = std::chrono::high_resolution_clock::now();
            serial_time += std::chrono::duration<double, std::milli>(end - start).count();
        }

        serial_time /= RUNS_PER_TEST;

        response << "Threads: 1"
                << " Average: "
                << serial_time
                << " ms"
                << " Speedup: 1.0\n";

        for (int thread_count : thread_counts) {

            if (thread_count == 1)
                continue;

            double total_time = 0.0;
            for (int run = 0; run < RUNS_PER_TEST; run++) {
                std::vector<int> arr = original;
                ThreadPool pool(thread_count);
                auto start = std::chrono::high_resolution_clock::now();
                parallel_merge_sort(arr, pool);
                auto end = std::chrono::high_resolution_clock::now();
                total_time += std::chrono::duration<double, std::milli>(end - start).count();
            }
            double average = total_time / RUNS_PER_TEST;
            double speedup = serial_time / average;
            response << "Threads: "
                    << thread_count
                    << " Average: "
                    << average
                    << " ms"
                    << " Speedup: "
                    << speedup
                    << "\n";
        }
        std::string result = response.str();
        send(client_socket, result.c_str(), result.size(), 0);
        close(client_socket);
        return;
    }

    std::string cmd;
    int n;
    int thread_count;
    iss >> cmd >> n >> thread_count;
    if (cmd != "MERGE_SORT" || n <= 0) {
        std::string err = "ERROR: invalid request.";
        send(client_socket, err.c_str(), err.size(), 0);
        close(client_socket);
        return;
    }

    if (n <= 0 || n > 1000000) {
        std::string err = "ERROR: Invalid size\n";
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

    auto start = std::chrono::high_resolution_clock::now();

    ThreadPool local_pool(thread_count);
    parallel_merge_sort(arr, local_pool);

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Time: "
            << std::chrono::duration<double, std::milli>(end - start).count()
            << " ms\n";

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