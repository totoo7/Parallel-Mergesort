#ifndef SERVER_HPP
#define SERVER_HPP

#include "server/thread_pool.hpp"

class Server {
    public:
        Server(int port);
        void handle_client(int client_socket);
        std::string run_benchmark();
        void run();
    private:
        ThreadPool thread_pool;
        int port;
        int server_fd;
};

#endif