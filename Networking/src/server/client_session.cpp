#include "server/client_session.h"

ClientSession::ClientSession(tcp::socket&& socket) {
    connection_ = TCP::Connection::create(std::move(socket));
    ip_address_ = connection_->get_client_address();
}

void ClientSession::start() {
    connection_->on_connect = [this]() {
        EventHandler::on_connect(ip_address_);
        send(ON_JOIN);
    };

    connection_->on_disconnect = [this]() {
        EventHandler::on_disconnect(ip_address_);
    };

    connection_->start();

    send(ASK_TO_AUTH);
    change_action(&ClientSession::authorize_user);
}

void ClientSession::send(const std::string& text) const {
    std::stringstream message;
    message << text;
    connection_->send(message.str());
}

void ClientSession::change_action(void (ClientSession::*callback)(const std::string&)) {
    connection_->set_on_read(std::bind(callback, this, std::placeholders::_1));
}

void ClientSession::authorize_user(const std::string& name) {
    try {
        std::regex username_pattern("^[a-zA-Z][a-zA-Z0-9_.]{2,15}$");
        if (!std::regex_match(name, username_pattern)) {
            throw IncorrectUsername(ON_AUTH_FAIL);
        }
        user_ = std::make_shared<User>(name);
        send(ON_AUTH_MESSAGE + ", " + name);
        display_commands();
    } catch (const IncorrectUsername& e) {
        send(e.what());
    }
}

void ClientSession::display_commands() {
    send(ASK_TO_COMMAND);
    change_action(&ClientSession::parse_command);
}

void ClientSession::parse_command(const std::string& line) {
    try {
        std::string command;
        std::vector<std::string> args;
        std::vector<std::string> tokens;

        std::string current;
        bool in_quotes = false;

        for (char c : line) {
            switch (c) {
                case '"':
                    in_quotes = !in_quotes;
                    break;
                case ' ':
                    if (!in_quotes && !current.empty()) {
                        tokens.push_back(current);
                        current.clear();
                    } else {
                        current += c;
                    }
                    break;
                default:
                    current += c;
                    break;
            }
        }
        if (!current.empty()) {
            tokens.push_back(current);
        }
        command = tokens[0];
        tokens.erase(tokens.begin());
        args = tokens;
        commands[command](args);
    } catch (...) {
        send("Command is incorrect");
        display_commands();
    }
}

void ClientSession::create_task(const std::vector<std::string>& args) {
    user_->add_task(std::make_shared<Task>(args));
    send("Task has created");
}

void ClientSession::display_tasks(const std::vector<std::string>& args) const {
    for (const Task& task : user_->get_all_tasks()) {
        send(task.get_info());
    }
}

