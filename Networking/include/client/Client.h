//
// Created by denis on 07.07.2024.
//

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

        void startRunning();

        void run(std::promise<int>& promise);
        void stop();
        void reset();

        void send(const std::string& message);

    private:
        void asyncWrite();
        void asyncRead();

        void onRead();

        std::queue<std::string> _outgoingMessages;

        io::io_context _ioContext;

        io::ip::tcp::socket _socket;
        io::ip::tcp::resolver::results_type _endpoints;

        io::streambuf _streamBuffer;

        std::thread _runningThread;
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

}

#endif //CLIENT_NETWORKING_INCLUDE_CLIENT_CLIENT_H_
