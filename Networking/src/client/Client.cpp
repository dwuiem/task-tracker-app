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
    _runningThread.join();
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
                std::cerr << ec.what() << std::endl;
                stop();
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
            stop();
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
    std::promise<int> promise;
    std::future<int> future = promise.get_future();

    _runningThread = std::thread([this, &promise] {
        this->run(promise);
    });

    bool connected = future.get();
    if (!connected) {
        throw FailedConnect("Failed to connect manager. Retrying ...");
    }
}