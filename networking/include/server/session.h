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
    static session_ptr create(tcp::socket&& socket, const std::function<void(const User& user, const std::string& message)>& notifier) {
        return session_ptr(new Session(std::move(socket), notifier));
    }
    void start();

    User get_user() const;
private:
    explicit Session(tcp::socket&& socket, const std::function<void(const User& user, const std::string& message)>& notifier);
    void display_commands();
};

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_CLIENT_SESSION_H_
