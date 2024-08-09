#include <iostream>

#include <boost/asio.hpp>

#include "server/server.h"

constexpr int SERVER_PORT = 1337;

int main() {
    Database& db = Database::get_instance();
    db.connect();
    db.load_tables();
    TCP::Server server(TCP::IPV::V4, SERVER_PORT);
    server.run();
    return 0;
}
