#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_CONNECTION_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_CONNECTION_H_

#include <boost/asio.hpp>

#include <server/event_handler.h>

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

        std::string get_client_address() const noexcept;

        std::function<void()> on_connect;
        std::function<void()> on_disconnect;
        std::function<void(std::string)> on_read;

    private:

        Connection(io::ip::tcp::socket&& socket);

        void async_write();
        void async_read();
        void on_write();
        void close();

        std::string client_address_;
        std::queue<std::string> outgoing_text_;

        io::ip::tcp::socket socket_;
        io::streambuf stream_buffer_ {MAX_BUFFER_SIZE + 1};
};
}

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_CONNECTION_H_
