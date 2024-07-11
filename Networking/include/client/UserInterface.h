
//
// Created by denis on 10.07.2024.
//

#ifndef CLIENT_NETWORKING_INCLUDE_CLIENT_USERINTERFACE_H_
#define CLIENT_NETWORKING_INCLUDE_CLIENT_USERINTERFACE_H_

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
    static void confirmJoin();
    static void tryConnectAlert();
    static void disconnectAlert();

    static std::string readCommand();
    static std::string readLine();
};

#endif //CLIENT_NETWORKING_INCLUDE_CLIENT_USERINTERFACE_H_
