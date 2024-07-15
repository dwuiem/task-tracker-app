#include <iostream>

#include <boost/asio.hpp>

#include "server/server.h"

int main() {
    const int default_port = 1337;
    TCP::Server server(TCP::IPV::V4, default_port);
    server.run();
    return 0;
}
