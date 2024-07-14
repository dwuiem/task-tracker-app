#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_CLIENT_SESSION_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_CLIENT_SESSION_H_

#include <server/connection.h>
#include <server/event_handler.h>
#include <server/user.h>

#include <boost/asio.hpp>
#include <regex>

using tcp = boost::asio::ip::tcp;

using connection_ptr = std::shared_ptr<TCP::Connection>;

class ClientSession {
public:
    static inline const std::string ASK_TO_AUTH = "Please, enter your username";
    static inline const std::string ON_AUTH_MESSAGE = "You are welcome";

    static inline const std::string MENU = "[MENU]: Select one of the following items (enter number):\n"
                                           " - (1) Create a new task\n"
                                           " - (2) Find a task\n"
                                           " - (3) Display all current tasks";

    using session_ptr = std::shared_ptr<ClientSession>;

    static session_ptr create(tcp::socket&& socket) {
        return session_ptr(new ClientSession(std::move(socket)));
    }

    void start();

    void send(std::string text);

private:

    ClientSession(tcp::socket&& socket);

    void authorize_user(std::string name);
    void display_menu();
    void choose_menu_item(std::string item);

    void change_action(void (ClientSession::*callback)(std::string));

    connection_ptr connection_;
    std::string ip_address_;

    std::shared_ptr<User> user_;
};

class IncorrectUsername : public std::exception  {
public:
    explicit IncorrectUsername(std::string message) : message_(std::move(message)) {}
    const char* what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_CLIENT_SESSION_H_
