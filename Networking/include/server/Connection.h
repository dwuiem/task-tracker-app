//
// Created by denis on 07.07.2024.
//

#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_CONNECTION_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_CONNECTION_H_

#include <boost/asio.hpp>
#include <memory>
#include <queue>

namespace TCP {

    const uint16_t MAX_BUFFER_SIZE = 65535;

    namespace io = boost::asio;

    class Connection : public std::enable_shared_from_this<Connection> {

        using connection_ptr = std::shared_ptr<Connection>;

    public:
        static connection_ptr create(io::ip::tcp::socket&& socket) {
            return connection_ptr(new Connection(std::move(socket)));
        }

        void start();

        void send(const std::string& message);

    private:

        Connection(io::ip::tcp::socket&& socket);

        void asyncWrite();
        void asyncRead();

        void onRead();

        std::string _clientAddress;
        std::queue<std::string> _outgointText;

        io::ip::tcp::socket _socket;
        io::streambuf _streamBuffer {MAX_BUFFER_SIZE + 1};
    };
}

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_CONNECTION_H_
