//
// Created by denis on 07.07.2024.
//

#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_CONNECTION_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_CONNECTION_H_

#pragma once

#include <boost/asio.hpp>

#include <server/EventHandler.h>

#include <memory>
#include <queue>

namespace TCP {

    const uint16_t MAX_BUFFER_SIZE = 65535;

    namespace io = boost::asio;

class Connection : public std::enable_shared_from_this<Connection> {
    public:
        using connection_ptr = std::shared_ptr<Connection>;
        static connection_ptr create(io::ip::tcp::socket&& socket) {
            return connection_ptr(new Connection(std::move(socket)));
        }
        void start();

        void send(const std::string& message);

        std::string getClientAddress() const noexcept;

        std::function<void(std::string)> onReadHandler;
        ~Connection() {
            std::cerr << "Connection destroyed" << std::endl;
        }
    private:
        Connection(io::ip::tcp::socket&& socket);
        void asyncWrite();
        void asyncRead();

        void onRead();

        void close();

        std::string _clientAddress;
        std::queue<std::string> _outgoingText;

        io::ip::tcp::socket _socket;
        io::streambuf _streamBuffer {MAX_BUFFER_SIZE + 1};
    };
}

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_CONNECTION_H_
