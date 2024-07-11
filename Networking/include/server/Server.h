//
// Created by denis on 07.07.2024.
//

#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_SERVER_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_SERVER_H_

#pragma once

#include "server/Connection.h"
#include "server/UserSession.h"

#include <boost/asio.hpp>
#include "unordered_set"

namespace TCP {

    enum IPV {
        V4,
        V6
    };

    namespace io = boost::asio;

    using session_ptr = std::shared_ptr<UserSession>;

    class Server {
    public:
        Server(IPV ipv, int port);

        int run();
    private:
        void accept();

        IPV _ipVersion;
        int _port;

        std::unordered_set<session_ptr> _sessions;

        io::io_context _ioContext;
        io::ip::tcp::acceptor _acceptor;

        std::optional<io::ip::tcp::socket> _socket;
    };

}

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_SERVER_H_
