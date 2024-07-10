//
// Created by denis on 10.07.2024.
//

#include <iostream>

#include "client/UserInterface.h"
#include "client/Client.h"

std::string UserInterface::readCommand() {
    std::string command;
    std::getline(std::cin, command);
    return command;
}
std::string UserInterface::readLine() {
    std::string message;
    std::getline(std::cin, message);
    return message;
}

void UserInterface::start() {
    std::cout << "Enter \"" << JOIN << "\" to connect manager\n";
    confirmJoin();
}

void UserInterface::confirmJoin() {
    try {
        std::string command = readCommand();
        if (command != JOIN) {
            throw IncorrectCommand("retry and type \"" + JOIN + "\"");
        }
    } catch (const IncorrectCommand& e) {
        std::cerr << "Incorrect: " << e.what() << std::endl;
        confirmJoin();
    }
}

void UserInterface::tryConnectAlert() {
    std::cout << "Connecting ...\n";
}
void UserInterface::disconnectAlert() {
    std::cout << "You have left from the server\n";
}
