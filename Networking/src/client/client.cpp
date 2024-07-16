#include "client/client.h"

TCP::Client::Client(const std::string& address, int port) : socket_(io_context_) {
    io::ip::tcp::resolver resolver{io_context_};
    endpoints_ = resolver.resolve(address, std::to_string(port));
}

void TCP::Client::run(std::promise<int>& promise) {
    io::async_connect(
        socket_,
        endpoints_,
        [this, &promise](boost::system::error_code ec, io::ip::tcp::endpoint ep) {
            if (ec) {
                promise.set_value(false);
                return;
            }
            promise.set_value(true);
            async_read();
        });
    io_context_.run();
}

void TCP::Client::send(const std::string& message) {
    if (!socket_.is_open()) {
        throw LostConnection();
    }
    outgoing_messages_.push(message);
    async_write();
}

void TCP::Client::stop() {
    boost::system::error_code ec;
    io_context_.stop();
    socket_.close(ec);
    if (ec) {
        std::cerr << "Closed with an error\n";
    }
}
void TCP::Client::reset() {
    io_context_.reset();
    socket_ = io::ip::tcp::socket(io_context_);
}

void TCP::Client::async_write() {
    io::async_write(socket_,
        io::buffer(outgoing_messages_.front() + "\n"),
        [this](boost::system::error_code ec, size_t bytesTransferred){
            if (ec) {
                return;
            }
            outgoing_messages_.pop();
            if (!outgoing_messages_.empty()) {
                async_write();
            }
    });
}
void TCP::Client::async_read() {
    io::async_read_until(socket_, stream_buffer_, "\n", [this](boost::system::error_code ec, size_t bytesTransferred) {
        if (ec) {
            throw LostConnection("Connection has lost. Press enter to restart");
            return;
        }
        on_read_handler();
        stream_buffer_.consume(stream_buffer_.size());
        async_read();
    });
}
void TCP::Client::on_read_handler() {
    std::stringstream message;
    message << std::istream(&stream_buffer_).rdbuf();
    std::cout << message.str();
}
void TCP::Client::start_running() {
    std::promise<int> promiseConnect;
    std::future<int> futureConnect = promiseConnect.get_future();

    running_thread_ = std::thread([this, &promiseConnect] {
        try {
            this->run(promiseConnect);
        } catch (const LostConnection& e) {
            std::cerr << e.what() << std::endl;
            stop();
        }
    });

    bool connected = futureConnect.get();
    if (!connected) {
        throw FailedConnect("Failed to connect manager. Retrying ...");
    }
}
void TCP::Client::finish_thread() {
    if (running_thread_.joinable()) {
        running_thread_.join();
    }
}
