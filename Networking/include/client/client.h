#ifndef CLIENT_NETWORKING_INCLUDE_CLIENT_CLIENT_H_
#define CLIENT_NETWORKING_INCLUDE_CLIENT_CLIENT_H_

#include <boost/asio.hpp>
#include <queue>

namespace TCP {

    const uint16_t MAX_BUFFER_SIZE = 65535;

    namespace io = boost::asio;

    class Client {
    public:
        Client(const std::string& address, int port);

        void start_running();

        void run(std::promise<int>& promise);
        void stop();
        void reset();

        void finish_thread();

        void send(const std::string& message);

    private:
        void async_write();
        void async_read();

        void on_read_handler();

        std::queue<std::string> outgoing_messages_;

        io::io_context io_context_;

        io::ip::tcp::socket socket_;
        io::ip::tcp::resolver::results_type endpoints_;

        io::streambuf stream_buffer_;

        std::thread running_thread_;
    };

    class FailedConnect : public std::exception {
    public:
        explicit FailedConnect(std::string message) : _message(std::move(message)) {}
        const char* what() const noexcept override {
            return _message.c_str();
        }
    private:
        std::string _message;
    };

    class LostConnection : public std::exception {
    public:
        LostConnection() = default;
        explicit LostConnection(std::string message) : _message(std::move(message)) {}
        const char* what() const noexcept override {
            return _message.c_str();
        }
    private:
        std::string _message;
    };

}

#endif //CLIENT_NETWORKING_INCLUDE_CLIENT_CLIENT_H_
