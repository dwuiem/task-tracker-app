#include <iostream>
#include "client/application.h"

Application::Application(TCP::Client& client) : client_(client) {}

void Application::run() {

    UserInterface::start();
    UserInterface::connection_alert();

    while(true) {
        try {
            client_.start_running();
            break;
        } catch (const TCP::FailedConnect& e) {
            std::cerr << e.what() << std::endl;
            stop();
            client_.reset();
        }
    }

    try {
        reading_messages();
    } catch (const TCP::LostConnection& e) {
        stop();
        return;
    }

    UserInterface::disconnection_alert();
    stop();
}

void Application::reading_messages() {
    std::string message = UserInterface::read_line();
    if (message == EXIT) return;
    client_.send(message);
    reading_messages();
}

void Application::stop() {
    client_.stop();
    client_.finish_thread();
}
