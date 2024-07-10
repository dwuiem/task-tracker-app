//
// Created by denis on 10.07.2024.
//

#include <iostream>
#include "client/Application.h"

Application::Application(TCP::Client& client) : _client(client) {}

void Application::run() {
    UserInterface::askToJoin();
    UserInterface::tryConnect();

    while(true) {
        try {
            _client.startRunning();
            break;
        } catch (const TCP::FailedConnect& e) {
            _client.stop();
            _client.reset();
            std::cerr << e.what() << std::endl;
        }
    }
    readMessages();
    UserInterface::disconnectAlert();
}

void Application::readMessages() {
    std::cin.ignore();
    std::string message = UserInterface::readLine();
    if (message == QUIT) {
        _client.stop();
        return;
    }
    _client.send(message);
    readMessages();
}
