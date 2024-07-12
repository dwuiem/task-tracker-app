#include "server/server.h"

#include <iostream>

TCP::Server::Server(TCP::IPV ipv, int port) :
    ip_version_(ipv),
    port_(port),
    acceptor_(io_context_, io::ip::tcp::endpoint(ip_version_ == TCP::V4 ? io::ip::tcp::v4() : io::ip::tcp::v6(), port_)) {}

int TCP::Server::run() {
    try {
        accept();
        io_context_.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
void TCP::Server::accept() {
    socket_.emplace(io_context_);

    // Asynchronously accepting client

    acceptor_.async_accept(*socket_, [this] (const boost::system::error_code ec) {
        if (!ec) {
//            auto connection = Connection::create(std::move(*socket_));
            session_ptr session = UserSession::create(std::move(*socket_));
            sessions_.emplace(session);
            session->start();
        }
        accept();
    });
}
