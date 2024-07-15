#include <server/connection.h>

#include <utility>

TCP::Connection::Connection(boost::asio::ip::tcp::socket&& socket) : socket_(std::move(socket)){
    try {
        std::stringstream address;
        address << socket_.remote_endpoint();
        client_address_ = address.str();
    } catch (const boost::system::error_code& ec) {
        std::cerr << ec.what() << std::endl;
    }
}

void TCP::Connection::start() {
    if (on_connect) {
        on_connect();
    }
    async_read();
}

void TCP::Connection::async_write() {
    io::async_write(socket_,
        io::buffer(outgoing_text_.front() + "\n"),
        [self = shared_from_this()](boost::system::error_code ec, size_t byteTransferred) {
            if (ec) {
                self->close();
                return;
            }
            self->on_write();
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
        [self = shared_from_this()] (boost::system::error_code ec, size_t bytesTransferred) {
            if (ec) {
                self->close();
                return;
            }

            std::stringstream line;
            line << std::istream(&self->stream_buffer_).rdbuf();

            std::string message;
            std::getline(line, message);

            if (self->on_read) {
                self->on_read(message);
            }

            EventHandler::on_command(self->client_address_, message);

            self->stream_buffer_.consume(self->stream_buffer_.size());
            self->async_read();
        });
}

void TCP::Connection::send(const std::string& message) {
    bool queueIdle = outgoing_text_.empty();
    outgoing_text_.push(message);
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
void TCP::Connection::set_on_read(std::function<void(const std::string&)> callback) {
    on_read = std::move(callback);
}

