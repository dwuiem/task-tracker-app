#include "server/session.h"

Session::Session(tcp::socket&& socket, const std::function<void(const User& user, const std::string& message)>& notifier) :
    TCP::Connection(std::move(socket), notifier), SessionAuthorization() {}

void Session::start() {
    on_connect = [this]() {
        EventHandler::on_connect(get_client_address());
        std::ostringstream out;
        out << GREEN << "You have successfully joined" << RESET;
        send(out.str());
    };

    on_disconnect = [this]() {
        EventHandler::on_disconnect(get_client_address());
    };

    connect();

    send("Please enter your username");
    on_read = [this](const std::string& username) {
        try {
            authorize_user(username);

            std::ostringstream guide;
            guide << GREEN << "You are successfully authorized\n" << RESET;
            guide << YELLOW << "Available commands:\n";
            guide << "-> create \"title\" \"description\" \"date\" [usernames of collaborators ...]\n";
            guide << "-> list\n";
            guide << "-> select <id>\n";
            guide << "  |-> edit [title/description/deadline] \"new value\"\n";
            guide << "  |-> complete\n";
            guide << "-> remove <id>" << RESET;
            send(guide.str());

            const auto message_sender = static_cast<std::shared_ptr<MessageSender>>(shared_from_this());
            command_handler_ = CommandHandler(user.value(), message_sender);

            main();
        } catch (const InvalidUsernameException& e) {
            send(e.what(), MessageType::EXCEPTION);
        }
    };
}

std::optional<User> Session::get_user() const {
    return user.value();
}

void Session::main() {
    send("Enter a command");
    on_read = [this](const std::string& command_line) {
        try {
            command_handler_.value().execute(command_line);
        } catch (const InvalidCommandException& e) {
            send(e.what(), MessageType::EXCEPTION);
        }
        main();
    };
}
