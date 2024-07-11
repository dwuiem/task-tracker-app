//
// Created by denis on 07.07.2024.
//

#include "server/Server.h"

#include <iostream>

TCP::Server::Server(TCP::IPV ipv, int port) :
    _ipVersion(ipv),
    _port(port),
    _acceptor(_ioContext, io::ip::tcp::endpoint(_ipVersion == TCP::V4 ? io::ip::tcp::v4() : io::ip::tcp::v6(), _port)) {}

int TCP::Server::run() {
    try {
        accept();
        _ioContext.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
void TCP::Server::accept() {
    _socket.emplace(_ioContext);

    // Asynchronously accepting client

    _acceptor.async_accept(*_socket, [this] (const boost::system::error_code ec) {
        if (!ec) {
            auto connection = Connection::create(std::move(*_socket));

            session_ptr session = UserSession::create(connection);
            _sessions.emplace(session);
            session->start();
        }
        accept();
    });
}
