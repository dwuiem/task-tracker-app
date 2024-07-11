//
// Created by denis on 11.07.2024.
//

#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_USERSESSION_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_USERSESSION_H_

#pragma once

#include <server/Connection.h>
#include <server/EventHandler.h>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

using connection_ptr = std::shared_ptr<TCP::Connection>;

class UserSession : public std::enable_shared_from_this<UserSession> {
public:
    using session_ptr = std::shared_ptr<UserSession>;
    static session_ptr create(connection_ptr connection) {
        return session_ptr(new UserSession(std::move(connection)));
    }
    void start();
    ~UserSession() {
        std::cerr << "Session destroyed" << std::endl;
    }
private:
    UserSession(connection_ptr connection);
    void sendNotify(std::string text);
    connection_ptr _connection;
    std::string _ipAddress;
};

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_USERSESSION_H_
