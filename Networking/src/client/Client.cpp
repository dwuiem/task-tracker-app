//
// Created by denis on 07.07.2024.
//

#include <iostream>
#include "client/Client.h"

TCP::Client::Client(const std::string& address, int port) : _socket(_ioContext) {
    io::ip::tcp::resolver resolver{_ioContext};
    _endpoints = resolver.resolve(address, std::to_string(port));
}

void TCP::Client::run(std::promise<int>& promise) {
    io::async_connect(
        _socket,
        _endpoints,
        [this, &promise](boost::system::error_code ec, io::ip::tcp::endpoint ep) {
            if (ec) {
                promise.set_value(false);
                return;
            }
            promise.set_value(true);
            asyncRead();
        });
    _ioContext.run();
}

void TCP::Client::send(const std::string& message) {
    if (!_socket.is_open()) {
        throw LostConnection();
    }
    _outgoingMessages.push(message);
    asyncWrite();
}

void TCP::Client::stop() {
    boost::system::error_code ec;
    _ioContext.stop();
    _socket.close(ec);
    if (ec) {
        std::cerr << "Closed with an error\n";
    }
}
void TCP::Client::reset() {
    _ioContext.reset();
    _socket = io::ip::tcp::socket(_ioContext);
}

void TCP::Client::asyncWrite() {
    io::async_write(_socket,
        io::buffer(_outgoingMessages.front() + "\n"),
        [this](boost::system::error_code ec, size_t bytesTransferred){
            if (ec) {
                return;
            }
            _outgoingMessages.pop();
            if (!_outgoingMessages.empty()) {
                asyncWrite();
            }
    });
}
void TCP::Client::asyncRead() {
    io::async_read_until(_socket, _streamBuffer, "\n", [this](boost::system::error_code ec, size_t bytesTransferred) {
        if (ec) {
            throw LostConnection("Connection has lost. Press enter to restart");
            return;
        }
        onRead();
        _streamBuffer.consume(_streamBuffer.size());
        asyncRead();
    });
}
void TCP::Client::onRead() {
    std::stringstream message;
    message << std::istream(&_streamBuffer).rdbuf();
    std::cout << message.str();
}
void TCP::Client::startRunning() {
    std::promise<int> promiseConnect;
    std::future<int> futureConnect = promiseConnect.get_future();

    _runningThread = std::thread([this, &promiseConnect] {
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
void TCP::Client::finishThread() {
    if (_runningThread.joinable()) {
        _runningThread.join();
    }
}
