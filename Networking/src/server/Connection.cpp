//
// Created by denis on 07.07.2024.
//

#include <iostream>

#include "server/Connection.h"

TCP::Connection::Connection(boost::asio::ip::tcp::socket&& socket) : _socket(std::move(socket)){
    try {
        std::stringstream address;
        address << _socket.remote_endpoint();
        _clientAddress = address.str();
    } catch (const boost::system::error_code& ec) {
        std::cerr << ec.what() << std::endl;
    }
}

void TCP::Connection::start() {
    // On connection handler
    std::cout << "User " << _clientAddress << " has joined" << std::endl;
    send("You have successfully joined to server. Enter \\q to leave");
    // Reading messages
    asyncRead();
}

void TCP::Connection::asyncWrite() {
    io::async_write(_socket,
        io::buffer(_outgointText.front() + "\n"),
        [this](boost::system::error_code ec, size_t byteTransferred) {
            if (ec) {
                _socket.close();
                std::cout << "User " << _clientAddress << " has left" << std::endl;
                return;
            }
            _outgointText.pop();
            if (!_outgointText.empty()) {
                asyncWrite();
            }
        });
}

void TCP::Connection::asyncRead() {
    io::async_read_until(
        _socket,
        _streamBuffer,
        "\n",
        [self = shared_from_this(), this] (boost::system::error_code ec, size_t bytesTransferred) {
            if (ec) {
                _socket.close();
                std::cout << "User " << _clientAddress << " has left" << std::endl;
                return;
            }
            self->onRead();
            asyncRead();
        });
}

void TCP::Connection::onRead() {
    std::stringstream message;
    message << std::istream(&_streamBuffer).rdbuf();
    _streamBuffer.consume(_streamBuffer.size());
    std::cout << "Message from " << _clientAddress << " : " << message.str();
}

void TCP::Connection::send(const std::string& message) {
    _outgointText.push(message);
    asyncWrite();
}

