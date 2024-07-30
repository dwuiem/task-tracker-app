#include <complex>
#include <server/command_handler.h>

#include "server/user_storage.h"

CommandHandler::CommandHandler(const std::shared_ptr<User>& user, const std::shared_ptr<MessageSender>& notifier) :
    user_(user),
    notifier_(notifier) {}

std::pair<std::string, std::vector<std::string>> CommandHandler::parse_command(const std::string &input) {
    if (input.empty()) throw InvalidCommandException("Command line is empty");
    size_t quote_count = std::count(input.begin(), input.end(), '"');
    if (quote_count % 2 != 0) throw InvalidCommandException("Quotes were not closed");

    std::string command;
    std::vector<std::string> args;

    std::vector<std::string> tokens;
    boost::sregex_iterator it(input.begin(), input.end(), command_pattern);
    const boost::sregex_iterator end;
    for (; it != end; ++it) {
        std::string match = it->str();
        if (match.front() == '"' && match.back() == '"') {
            match = match.substr(1, match.size() - 2);
        }
        tokens.push_back(match);
    }

    command = tokens.front();
    tokens.erase(tokens.begin());
    args = tokens;
    return {command, args};
}

void CommandHandler::execute(const std::string &command_line) {
    const auto& [command, args] = parse_command(command_line);
    try {
        commands.at(command)(args);
    } catch (const std::out_of_range&) {
        throw InvalidCommandException("Command doesn't exist");
    }
}

void CommandHandler::create_task(const std::vector<std::string> &args) {
    if (args.size() < 3) throw InvalidCommandException("Too few arguments");
    const std::string& title = args[0];
    const std::string& description = args[1];
    const std::string& date = args[2];
    std::vector<std::shared_ptr<User>> collaborators;
    for (size_t i = 3; i < args.size(); i++) {
        try {
            auto collaborator = UserStorage::get_user(args[i]);
            if (collaborator->get_id() == user_->get_id()) throw InvalidCommandException("You can't add yourself as a collaborator");
            collaborators.push_back(collaborator);
        } catch (const UserNotFoundException&) {
            throw InvalidCommandException("User not found");
        }
    }
    std::shared_ptr<Task> task = std::make_shared<Task>(title, description, date, user_);
    for (const auto& collaborator : collaborators) {
        task->add_collaborator(collaborator);
        collaborator->add_task(task);
        notifier_->send_to_user(collaborator, "User \"" + user_->get_name() + "\" added a task for you");
    }
    user_->add_task(task);
    notifier_->send("You have created task");
}

void CommandHandler::display(const std::vector<std::string> &args) const {
    if (user_->get_all_tasks().empty()) {
        notifier_->send("There are no any tasks in your list");
        return;
    }
    std::string task_list = "Task list\n";
    for (const auto& task : user_->get_all_tasks()) {
        task_list += task->get_info() + "\n\n";
    }
    notifier_->send(task_list);
}

