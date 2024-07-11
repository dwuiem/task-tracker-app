//
// Created by denis on 07.07.2024.
//

#include <server/Connection.h>

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
    EventHandler::onConnect(_clientAddress);
//    send("You have successfully joined to server. Enter \\q to leave");

    asyncRead();
}

void TCP::Connection::asyncWrite() {
    io::async_write(_socket,
        io::buffer(_outgoingText.front() + "\n"),
        [self = shared_from_this()](boost::system::error_code ec, size_t byteTransferred) {
            if (ec) {
                self->close();
                return;
            }
            self->_outgoingText.pop();
            if (!self->_outgoingText.empty()) {
                self->asyncWrite();
            }
        });
}

void TCP::Connection::asyncRead() {
    io::async_read_until(
        _socket,
        _streamBuffer,
        "\n",
        [self = shared_from_this()] (boost::system::error_code ec, size_t bytesTransferred) {
            if (ec) {
                self->close();
                return;
            }
            std::stringstream message;
            message << std::istream(&self->_streamBuffer).rdbuf();
            self->_streamBuffer.consume(self->_streamBuffer.size());
            if (self->onReadHandler) {
                self->onReadHandler(message.str());
            }
            EventHandler::onClientMessage(self->_clientAddress, message.str());
            self->asyncRead();
        });
}

void TCP::Connection::send(const std::string& message) {
    _outgoingText.push(message);
    asyncWrite();
}

void TCP::Connection::close() {
    EventHandler::onDisconnect(_clientAddress);
    _socket.close();
}
std::string TCP::Connection::getClientAddress() const noexcept {
    return _clientAddress;
}

