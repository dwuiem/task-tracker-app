//
// Created by denis on 10.07.2024.
//

#include <iostream>
#include "client/Application.h"

Application::Application(TCP::Client& client) : _client(client) {}

void Application::run() {

    UserInterface::start();
    UserInterface::tryConnectAlert();

    while(true) {
        try {
            _client.startRunning();
            break;
        } catch (const TCP::FailedConnect& e) {
            std::cerr << e.what() << std::endl;
            stop();
            _client.reset();
        }
    }

    try {
        readMessages();
    } catch (const TCP::LostConnection& e) {
        stop();
        return;
    }

    UserInterface::disconnectAlert();
    stop();
}

void Application::readMessages() {
    std::string message = UserInterface::readLine();
    if (message == QUIT) {
        return;
    }
    _client.send(message);
    readMessages();
}

void Application::stop() {
    _client.stop();
    _client.finishThread();
}
