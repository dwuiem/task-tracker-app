#include "server/session.h"

Session::Session(tcp::socket&& socket) :
    TCP::Connection(std::move(socket)) {}

void Session::start() {
    on_connect = [this]() {
        EventHandler::on_connect(get_client_address());
        send(ON_JOIN);
    };

    on_disconnect = [this]() {
        EventHandler::on_disconnect(get_client_address());
    };

    connect();

    send(ASK_TO_AUTH);
    on_read = [this](const std::string& username) {
        try {
            authorize_user(username);
            display_commands();
        } catch (const InvalidUsernameException& e) {
            send(e.what());
        }
    };
}

void Session::display_commands() {
    send(ASK_TO_COMMAND);
    on_read = [this](const std::string& command_line) {
        auto self = shared_from_this();
        CommandHandler command_handler(user, self);
        try {
            command_handler.execute(command_line);
        } catch (const InvalidCommandException& e) {
            send(e.what());
        }
    };
}

std::shared_ptr<User> Session::get_user() const {
    return user;
}

