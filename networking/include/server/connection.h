#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_CONNECTION_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_CONNECTION_H_

#include <boost/asio.hpp>

#include <queue>

#include "event_handler.h"
#include "message_sender.h"

namespace TCP {

const uint16_t MAX_BUFFER_SIZE = 65535;

namespace io = boost::asio;

class Connection : public MessageSender {
public:
    explicit Connection(io::ip::tcp::socket&& socket);

    void connect();
    void send(const std::string& message, MessageType message_type = MessageType::INFO) override;
    void send_to_user(std::shared_ptr<User> user, const std::string &message) override;

    std::string get_client_address() const noexcept;

    std::function<void(const std::shared_ptr<User>&, const std::string& message)> post_to_user;
protected:
    std::function<void()> on_connect;
    std::function<void()> on_disconnect;
    std::function<void(const std::string&)> on_read;

private:
    void async_write();
    void async_read();
    void on_write();
    void close();

    std::string client_address_;
    std::queue<std::string> outgoing_text_;

    io::ip::tcp::socket socket_;
    io::streambuf stream_buffer_{MAX_BUFFER_SIZE + 1};
};
}

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_CONNECTION_H_
