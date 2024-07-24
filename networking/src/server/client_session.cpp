#include "server/client_session.h"

ClientSession::ClientSession(tcp::socket&& socket) :
    TCP::Connection(std::move(socket)),
    task_distributor_(TaskDistributor::get_instance()) {}

void ClientSession::start() {
    on_connect = [this]() {
        EventHandler::on_connect(get_client_address());
        send_message(ON_JOIN);
    };

    on_disconnect = [this]() {
        EventHandler::on_disconnect(get_client_address());
    };

    connect();

    send_message(ASK_TO_AUTH);
    on_read = [this](const std::string& username) {
        try {
            authorize_user(username);
            display_commands();
        } catch (const InvalidUsernameException& e) {
            send_message(e.what());
        }
    };
}

void ClientSession::display_commands() {
    send_message(ASK_TO_COMMAND);
    on_read = [this](const std::string& command_line) {
        parse_command(command_line);
    };
}

void ClientSession::parse_command(const std::string& line) {
    std::string command;
    std::vector<std::string> args;
    try {
        // Check empty line and closed quotes
        if (line.empty()) throw InvalidCommandException("Command line is empty");
        size_t quote_count = std::count(line.begin(), line.end(), '"');
        if (quote_count % 2 != 0) throw InvalidCommandException("Quotes were not closed");

        std::vector<std::string> tokens;
        boost::sregex_token_iterator it(line.begin(), line.end(), command_line_pattern, {1, 2});
        boost::sregex_token_iterator end;
        while (it != end) {
            std::string token = *it;
            if (!token.empty()) tokens.push_back(token);
            it++;
        }
        command = tokens.front();
        tokens.erase(tokens.begin());
        args = tokens;
        execute_command(command, args);
    } catch (const InvalidCommandException& e) {
        send_message(e.what());
    }
    display_commands();
}

void ClientSession::execute_command(const std::string& command, const std::vector<std::string>& args) {
    try {
        auto command_handler = commands.at(command);
        command_handler(args);
    } catch (const std::out_of_range& e) {
        send_message("Command doesn't exist");
    } catch (const std::exception& e) {
        send_message(e.what());
    }
}

void ClientSession::create_task(const std::vector<std::string>& args) {
    if (args.size() < 3) throw InvalidCommandException("Too few arguments");
    std::vector<std::shared_ptr<User>> collaborators = {user};
    if (args.size() > 3) {
        for (size_t i = 3; i < args.size(); i++) {
            if (!user_exists(args[i])) throw InvalidCommandException("User \"" + args[i] + "\" doesn't exist");
            auto user = user_map.at(args[i]);
            collaborators.push_back(user);
        }
    }
    task_distributor_.add_task(std::make_shared<Task>(args[0], args[1], args[2]), collaborators);
    for (const auto& collaborator : collaborators) {
        if (collaborator != user) {
            send_to_user(collaborator, "User \"" + user->get_name() + "\" added a task for you");
        }
    }
    send_message("Task has created");
}

void ClientSession::display_tasks(const std::vector<std::string>& args) {
    send_message("Task list\n");
    for (const auto& task : user->get_all_tasks()) {
        std::string names;
        for (const auto& collaborator : task_distributor_.get_collaborators(task)) {
            names += collaborator->get_name() + DELIM;
        }
        send_message(task->get_info() + "\nCollaborators: " + names);
    }
}
std::shared_ptr<User> ClientSession::get_user() const {
    return user;
}

