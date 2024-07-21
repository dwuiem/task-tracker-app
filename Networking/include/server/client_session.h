#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_CLIENT_SESSION_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_CLIENT_SESSION_H_

#include <server/connection.h>
#include <server/event_handler.h>
#include <server/user.h>

#include <boost/asio.hpp>
#include <regex>
#include <ctime>

#include "task_distributor.h"

using tcp = boost::asio::ip::tcp;

class ClientSession {
public:
    using session_ptr = std::shared_ptr<ClientSession>;
    static session_ptr create(tcp::socket&& socket, std::unordered_map<std::string, std::shared_ptr<User>>& user_map) {
        return session_ptr(new ClientSession(std::move(socket), user_map));
    }

    void start();
    void send(const std::string& text);
private:
    static inline const std::string ON_JOIN = "You have successfully joined";
    static inline const std::string ASK_TO_AUTH = "Please, enter your username";
    static inline const std::string ON_AUTH_MESSAGE = "You are welcome";
    static inline const std::string ON_AUTH_FAIL = "User name is incorrect. Please retry";
    static inline const std::string ASK_TO_COMMAND = "Write a command";

    static inline const char DELIM = ' ';

    explicit ClientSession(tcp::socket&& socket, std::unordered_map<std::string, std::shared_ptr<User>>& user_map);

    void change_action(void (ClientSession::*callback)(const std::string&));
    void authorize_user(const std::string& name);
    void display_commands();
    void parse_command(const std::string& line);
    void create_task(const std::vector<std::string>& args);
    void display_tasks(const std::vector<std::string>& args);

    std::shared_ptr<User> get_user(const std::string&);
    bool user_exists(const std::string& name) const;

    std::unordered_map<std::string, std::shared_ptr<User>>& user_map_;

    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> commands = {
        {"create", [this](auto && PH1) { create_task(std::forward<decltype(PH1)>(PH1)); } },
        {"display", [this](auto && PH1) { display_tasks(std::forward<decltype(PH1)>(PH1)); } }
    };

    TaskDistributor& task_distributor_;

    TCP::Connection connection_;
    std::shared_ptr<User> user_;
};

class InvalidCommandException : public std::exception {
public:
    explicit InvalidCommandException(std::string message) : message_(std::move(message)) {}
    const char *what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};

class InvalidUsernameException : public std::exception {
public:
    explicit InvalidUsernameException(std::string message) : message_(std::move(message)) {}
    const char *what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_CLIENT_SESSION_H_
