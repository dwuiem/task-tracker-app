#include <server/connection.h>

#include <utility>

TCP::Connection::Connection(boost::asio::ip::tcp::socket&& socket, std::function<void(const User& user, const std::string& message)> notifier) :
socket_(std::move(socket)),
MessageSender(std::move(notifier)) {
    try {
        std::stringstream address;
        address << socket_.remote_endpoint();
        client_address_ = address.str();
    } catch (const boost::system::error_code& ec) {
        std::cerr << ec.what() << std::endl;
    }
}


void TCP::Connection::connect() {
    if (on_connect) {
        on_connect();
    }
    async_read();
}

void TCP::Connection::async_write() {
    io::async_write(socket_,
        io::buffer(outgoing_text_.front() + "\n"),
        [this](boost::system::error_code ec, size_t byteTransferred) {
            if (ec) {
                close();
                return;
            }
            on_write();
        });
}
void TCP::Connection::on_write() {
    outgoing_text_.pop();
    if (!outgoing_text_.empty()) {
        async_write();
    }
}

void TCP::Connection::async_read() {
    io::async_read_until(
        socket_,
        stream_buffer_,
        "\n",
        [this] (boost::system::error_code ec, size_t bytesTransferred) {
            if (ec) {
                close();
                return;
            }

            std::stringstream line;
            line << std::istream(&stream_buffer_).rdbuf();

            std::string message;
            std::getline(line, message);

            if (on_read) {
                on_read(message);
            }

            EventHandler::on_command(client_address_, message);

            stream_buffer_.consume(stream_buffer_.size());
            async_read();
        });
}

void TCP::Connection::send(const std::string& message, MessageType message_type) {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    std::stringstream text;
    switch (message_type) {
        case MessageType::INFO:
            text << "[" << std::setw(2) << std::setfill('0') << localTime->tm_hour << ":"
                << std::setw(2) << std::setfill('0') << localTime->tm_min << ":"
                << std::setw(2) << std::setfill('0') << localTime->tm_sec << " | INFO]";
            break;
        case MessageType::NOTIFY:
            text << YELLOW << "[" << std::setw(2) << std::setfill('0') << localTime->tm_hour << ":"
                << std::setw(2) << std::setfill('0') << localTime->tm_min << ":"
                << std::setw(2) << std::setfill('0') << localTime->tm_sec << " | NOTIFY]" << RESET;
            break;
        case MessageType::EXCEPTION:
            text << RED << "[ERROR]" << RESET;
            break;
    }
    text << " ";
    text << message;
    bool queueIdle = outgoing_text_.empty();
    outgoing_text_.push(text.str());
    if (queueIdle) {
        async_write();
    }
}

void TCP::Connection::close() {
    if (on_disconnect) {
        on_disconnect();
    }
    socket_.close();
}
std::string TCP::Connection::get_client_address() const noexcept {
    return client_address_;
}

