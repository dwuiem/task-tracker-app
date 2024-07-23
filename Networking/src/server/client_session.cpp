#include "server/client_session.h"

ClientSession::ClientSession(tcp::socket&& socket, std::unordered_map<std::string, std::shared_ptr<User>>& user_map) :
    connection_(std::move(socket)),
    task_distributor_(TaskDistributor::get_instance()),
    user_map_(user_map) {}

void ClientSession::start() {
    connection_.on_connect = [this]() {
        EventHandler::on_connect(connection_.get_client_address());
        send(ON_JOIN);
    };

    connection_.on_disconnect = [this]() {
        EventHandler::on_disconnect(connection_.get_client_address());
    };

    connection_.start();

    send(ASK_TO_AUTH);
    change_action(&ClientSession::authorize_user);
}

void ClientSession::send(const std::string& text) {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    std::stringstream message;
    message << "[" << std::setw(2) << std::setfill('0') << localTime->tm_hour << ":"
            << std::setw(2) << std::setfill('0') << localTime->tm_min << ":"
            << std::setw(2) << std::setfill('0') << localTime->tm_sec << "] ";
    message << text;
    connection_.send(message.str());
}

void ClientSession::change_action(void (ClientSession::*callback)(const std::string&)) {
    connection_.set_on_read(std::bind(callback, this, std::placeholders::_1));
}

std::shared_ptr<User> ClientSession::get_authorized_user(const std::string& name) {
    if (user_exists(name)) {
        return user_map_.at(name);
    } else {
        auto new_user = std::make_shared<User>(name);
        user_map_[name] = new_user;
        return new_user;
    }
}

bool ClientSession::user_exists(const std::string& name) const {
    return user_map_.find(name) != user_map_.end();
}

void ClientSession::authorize_user(const std::string& username) {
    try {
        boost::regex username_pattern{"^[a-zA-Z][a-zA-Z0-9_.]{2,15}$"};
        if (!boost::regex_match(username, username_pattern)) throw InvalidUsernameException(ON_AUTH_FAIL);
        user_ = get_authorized_user(username);
        send(ON_AUTH_MESSAGE + ", " + username);
        display_commands();
    } catch (const InvalidUsernameException& e) {
        send(e.what());
    }
}

void ClientSession::display_commands() {
    send(ASK_TO_COMMAND);
    change_action(&ClientSession::parse_command);
}

void ClientSession::parse_command(const std::string& line) {
    std::string command;
    std::vector<std::string> args;
    try {
        if (line.empty()) throw InvalidCommandException("Line is empty");
        std::vector<std::string> tokens;
        std::string current;
        bool in_quotes = false;
        for (char c : line) {
            switch (c) {
                case '"':
                    in_quotes = !in_quotes;
                    break;
                case DELIM:
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
        if (!current.empty()) tokens.push_back(current);
        if (in_quotes) throw InvalidCommandException("Quotes were not closed");
        command = tokens[0];
        tokens.erase(tokens.begin());
        args = tokens;
        execute_command(command, args);
    } catch (const InvalidCommandException& e) {
        send(e.what());
    }
    display_commands();
}

void ClientSession::execute_command(const std::string& command, const std::vector<std::string>& args) {
    try {
        auto command_handler = commands.at(command);
        command_handler(args);
    } catch (const std::out_of_range& e) {
        send("Command doesn't exist");
    } catch (const std::exception& e) {
        send(e.what());
    }
}

void ClientSession::create_task(const std::vector<std::string>& args) {
    if (args.size() < 3) throw InvalidCommandException("Too few arguments");
    std::vector<std::shared_ptr<User>> collaborators = {user_};
    if (args.size() > 3) {
        for (size_t i = 3; i < args.size(); i++) {
            if (!user_exists(args[i])) throw InvalidCommandException("User \"" + args[i] + "\" doesn't exist");
            auto user = user_map_.at(args[i]);
            collaborators.push_back(user);
        }
    }
    task_distributor_.add_task(std::make_shared<Task>(args[0], args[1], args[2]), collaborators);
    for (const auto& collaborator : collaborators) {
        if (collaborator != user_) {
            send_to_user(collaborator, "User \"" + user_->get_name() + "\" added a task for you");
        }
    }
    send("Task has created");
}

void ClientSession::display_tasks(const std::vector<std::string>& args) {
    send("Task list\n");
    for (const auto& task : user_->get_all_tasks()) {
        std::string names;
        for (const auto& collaborator : task_distributor_.get_collaborators(task)) {
            names += collaborator->get_name() + DELIM;
        }
        send(task->get_info() + "\nCollaborators: " + names);
    }
}
std::shared_ptr<User> ClientSession::get_user() const {
    return user_;
}

