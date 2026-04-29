#include "server/server.hpp"

int main(int, char**) {
    Server server(8080);
    server.run();
    return 0;
}