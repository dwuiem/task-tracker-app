#ifndef CLIENT_NETWORKING_INCLUDE_CLIENT_USER_INTERFACE_H_
#define CLIENT_NETWORKING_INCLUDE_CLIENT_USER_INTERFACE_H_

#include <utility>

const std::string JOIN = "join";
const std::string QUIT = "\\q";

class IncorrectCommand : public std::exception {
public:
    explicit IncorrectCommand(std::string message) : _message(std::move(message)) {}
    const char* what() const noexcept override {
        return _message.c_str();
    }

private:
    std::string _message;
};

class UserInterface {
public:

    static void start();
    static void confirm_join();
    static void connection_alert();
    static void disconnection_alert();

    static std::string read_command();
    static std::string read_line();
};

#endif //CLIENT_NETWORKING_INCLUDE_CLIENT_USER_INTERFACE_H_
