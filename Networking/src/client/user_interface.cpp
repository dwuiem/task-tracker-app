#include <iostream>

#include "client/user_interface.h"
#include "client/client.h"

std::string UserInterface::read_command() {
    std::string command;
    std::getline(std::cin, command);
    return command;
}
std::string UserInterface::read_line() {
    std::string message;
    std::getline(std::cin, message);
    return message;
}

void UserInterface::start() {
    std::cout << "Enter \"" << JOIN << "\" to connect manager\n";
    confirm_join();
}

void UserInterface::confirm_join() {
    try {
        std::string command = read_command();
        if (command != JOIN) {
            throw IncorrectCommand("retry and type \"" + JOIN + "\"");
        }
    } catch (const IncorrectCommand& e) {
        std::cerr << "Incorrect: " << e.what() << std::endl;
        confirm_join();
    }
}

void UserInterface::connection_alert() {
    std::cout << "Connecting ...\n";
}
void UserInterface::disconnection_alert() {
    std::cout << "You have left from the server\n";
}
