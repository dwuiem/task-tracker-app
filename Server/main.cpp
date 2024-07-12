#include <iostream>

#include <boost/asio.hpp>

#include "server/server.h"

int main() {
    TCP::Server server(TCP::IPV::V4, 1337);
    server.run();
    return 0;
}
