#include "server/user_session.h"

UserSession::UserSession(tcp::socket&& socket) {
    connection_ = TCP::Connection::create(std::move(socket));
    ip_address_ = connection_->get_client_address();
}

void UserSession::start() {
    connection_->on_connect = [this] () {
        EventHandler::on_connect(ip_address_);
        send(ON_JOIN_MESSAGE);
        change_state(AUTHORIZATION);
    };

    connection_->on_disconnect = [this] () {
        EventHandler::on_disconnect(ip_address_);
    };

    connection_->start();

    connection_->on_read = [this] (std::string message) {
        switch (state_) {
            case AUTHORIZATION:
                authorize_user(message);
                break;
            case MAIN:

                break;
            case ENTER_NEW_TASK_NAME:

                break;
        }
    };
}

void UserSession::send(std::string text) {
    std::stringstream message;
    message << text;
    connection_->send(message.str());
}
void UserSession::authorize_user(std::string name) {
    try {
        std::regex username_pattern("\\w{3,15}");
        if (!std::regex_match(name, username_pattern)) {
            throw IncorrectUsername("User name is incorrect. Please retry");
        }
        user_ = std::make_shared<User>(name);
        send(ON_AUTH_MESSAGE + ", " + name);
        change_state(MAIN);
    } catch (const IncorrectUsername& e) {
        send(e.what());
    }
}

void UserSession::change_state(State new_state) {
    state_ = new_state;
    send(navigation.at(state_));
}

