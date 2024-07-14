#include "server/client_session.h"

ClientSession::ClientSession(tcp::socket&& socket) {
    connection_ = TCP::Connection::create(std::move(socket));
    ip_address_ = connection_->get_client_address();
}

void ClientSession::start() {
    connection_->on_connect = [this] () {
        EventHandler::on_connect(ip_address_);
        send("You have successfully joined");
    };

    connection_->on_disconnect = [this] () {
        EventHandler::on_disconnect(ip_address_);
    };

    connection_->start();

    send(ASK_TO_AUTH);
    change_action(&ClientSession::authorize_user);
}

void ClientSession::authorize_user(std::string name) {
    try {
        std::regex username_pattern("^[a-zA-Z][a-zA-Z0-9_.]{2,15}$");
        if (!std::regex_match(name, username_pattern)) {
            throw IncorrectUsername("User name is incorrect. Please retry");
        }
        user_ = std::make_shared<User>(name);
        send(ON_AUTH_MESSAGE + ", " + name);
        display_menu();
    } catch (const IncorrectUsername& e) {
        send(e.what());
    }
}

void ClientSession::display_menu() {
    send(MENU);
    change_action(&ClientSession::choose_menu_item);
}

void ClientSession::send(std::string text) {
    std::stringstream message;
    message << text;
    connection_->send(message.str());
}

void ClientSession::change_action(void (ClientSession::*callback)(std::string)) {
    connection_->set_on_read(std::bind(callback, this, std::placeholders::_1));
}
void ClientSession::choose_menu_item(std::string item) {
    try {
        int number_of_item = std::stoi(item);
        switch (number_of_item) {
            case 1:
                send("You have chosen item " + item);
                break;
            case 2:
                send("You have chosen item " + item);
                break;
            case 3:
                send("You have chosen item " + item);
                break;
            default:
                throw std::invalid_argument("from switch");
        }
        display_menu();
    } catch (const std::invalid_argument& e) {
        send("Item doesn't exist");
    }
}

