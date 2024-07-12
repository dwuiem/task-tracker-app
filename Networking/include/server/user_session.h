#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_USER_SESSION_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_USER_SESSION_H_

#include <server/connection.h>
#include <server/event_handler.h>
#include <server/user.h>

#include <boost/asio.hpp>
#include <regex>

using tcp = boost::asio::ip::tcp;

using connection_ptr = std::shared_ptr<TCP::Connection>;

class UserSession {
public:
    using session_ptr = std::shared_ptr<UserSession>;

    static session_ptr create(tcp::socket&& socket) {
        return session_ptr(new UserSession(std::move(socket)));
    }

    void start();

private:
    enum State {
        AUTHORIZATION,
        MAIN,
        ENTER_TASK_NAME
    };

    static inline const std::string ON_JOIN_MESSAGE = "You have successfully connected to system";
    static inline const std::string ON_AUTH_MESSAGE = "You are welcome";

    static inline const std::unordered_map<State, std::string> navigation = {
        {AUTHORIZATION,"Enter username to authorize in system"},

        {MAIN, "[MENU]: Select one of the following items (enter number):\n"
               " - (1) Create a new task"},

        {ENTER_TASK_NAME, "[CREATING]: Choose name for new task"}
    };

    UserSession(tcp::socket&& socket);

    void authorize_user(std::string name);

    void send(std::string text);

    void change_state(State new_state);

    State state_;

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

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_USER_SESSION_H_
