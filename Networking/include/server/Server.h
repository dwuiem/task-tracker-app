//
// Created by denis on 07.07.2024.
//

#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_SERVER_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_SERVER_H_

#pragma once

#include <boost/asio.hpp>

namespace TCP {

    enum IPV {
        V4,
        V6
    };

    namespace io = boost::asio;

    class Server {
    public:
        Server(IPV ipv, int port);

        int run();
    private:
        void accept();

        IPV _ipVersion;
        int _port;

        io::io_context _ioContext;
        io::ip::tcp::acceptor _acceptor;

        std::optional<io::ip::tcp::socket> _socket;
    };

}

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_SERVER_H_
