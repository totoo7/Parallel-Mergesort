#ifndef SERVER_HPP
#define SERVER_HPP

class Server {
    public:
        Server(int port);
        void run();
    private:
        int port;
        int server_fd;
};

#endif