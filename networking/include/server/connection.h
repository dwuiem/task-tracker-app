#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_CONNECTION_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_CONNECTION_H_

#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"

#include <boost/asio.hpp>

#include <queue>

#include "event_handler.h"
#include "message_sender.h"

namespace TCP {

constexpr uint16_t MAX_BUFFER_SIZE = 65535;

namespace io = boost::asio;

class Connection : public MessageSender {
public:
    explicit Connection(io::ip::tcp::socket&& socket, std::function<void(const User& user, const std::string& message)> notifier);

    void connect();
    void send(const std::string& message, MessageType message_type = MessageType::INFO) override;

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
