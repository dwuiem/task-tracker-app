#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_CLIENT_SESSION_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_CLIENT_SESSION_H_

#include <server/connection.h>
#include <server/user.h>

#include <boost/asio.hpp>
#include <boost/regex.hpp>

#include "command_handler.h"
#include "session_authorization.h"

class Session : public std::enable_shared_from_this<Session>, public TCP::Connection, public SessionAuthorization {
public:
    using session_ptr = std::shared_ptr<Session>;
    using tcp = boost::asio::ip::tcp;
    static session_ptr create(tcp::socket&& socket) {
        return session_ptr(new Session(std::move(socket)));
    }

    void start();

    std::shared_ptr<User> get_user() const;
private:

    static inline const char DELIM = ' ';

    explicit Session(tcp::socket&& socket);

    void display_commands();

    static inline const std::string ON_JOIN = "You have successfully joined";
    static inline const std::string ASK_TO_AUTH = "Please, enter your username";
    static inline const std::string ON_AUTH_MESSAGE = "You are welcome";
    static inline const std::string ASK_TO_COMMAND = "Write a command";
};

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_CLIENT_SESSION_H_
