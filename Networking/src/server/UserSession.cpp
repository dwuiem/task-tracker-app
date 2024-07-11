//
// Created by denis on 11.07.2024.
//

#include "server/UserSession.h"

UserSession::UserSession(connection_ptr connection) :
    _connection(std::move(connection)){
    _ipAddress = _connection->getClientAddress();
}

void UserSession::start() {
    _connection->start();

    sendNotify("You has joined to manager successfully");

    _connection->onReadHandler = [this](std::string message) {
        sendNotify("You sent message");
    };
}

void UserSession::sendNotify(std::string text) {
    std::stringstream message;
    message << "[SERVER] : " << text;
    _connection->send(message.str());
}
