#include <regex>
#include <server/command_handler.h>

CommandHandler::CommandHandler(const std::shared_ptr<User>& user, const std::shared_ptr<MessageSender>& notifier) :
    user_(user),
    notifier_(notifier) {}

boost::gregorian::date CommandHandler::parse_date(const std::string &input) {
    boost::gregorian::date result;
    const boost::regex full_date_pattern{R"((\d{2})[./](\d{2})[./](\d{2,4}))"};
    const boost::regex month_date_pattern{R"((\d{2})[./](\d{2}))"};
    int current_year = boost::gregorian::day_clock::local_day().year();
    int year = current_year;
    int day;
    int month;
    boost::smatch match;
    if (boost::regex_match(input, match, full_date_pattern)) {
        day = std::stoi(match[1]);
        month = std::stoi(match[2]);
        year = std::stoi(match[3]);
        if (year < 100) {
            year += current_year - current_year % 100;
        }
    } else if (boost::regex_match(input, match, month_date_pattern)) {
        day = std::stoi(match[1]);
        month = std::stoi(match[2]);
    } else {
        throw InvalidCommandException("Date format incorrect");
    }
    try {
        result = {year, month, day};
    } catch (...) {
        throw InvalidCommandException("Date doesn't exist");
    }
    return result;
}

std::pair<boost::posix_time::hours, boost::posix_time::minutes> CommandHandler::parse_time(const std::string &input) {
    const boost::regex time_pattern{R"((\d{1,2}):(\d{2}))"};
    boost::smatch match;
    if (boost::regex_match(input, match, time_pattern)) {
        int hours = std::stoi(match[1]);
        int minutes = std::stoi(match[2]);
        if (hours > 23 || minutes > 59) {
            throw InvalidCommandException("Hours must be less than 24 "
                                          "and minutes must be less than 60");
        }
        boost::posix_time::hours h(hours);
        boost::posix_time::minutes m(minutes);
        return {h, m};
    } else {
        throw InvalidCommandException("Time format incorrect");
    }
}

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
    notifier_->send("You have created task", MessageType::INFO);
}

void CommandHandler::display(const std::vector<std::string> &args) const {
    if (user_->get_all_tasks().empty()) {
        notifier_->send("There are no any tasks in your list", MessageType::INFO);
        return;
    }
    std::string task_list = "Task list\n";
    for (const auto& task : user_->get_all_tasks()) {
        task_list += "### Task ID: " + std::to_string(task->get_id()) + "\n"
                        "### Title: " + task->get_title() + "\n"
                        "### Description: " + task->get_description() + "\n"
                        "### Deadline: " + task->get_date() + "\n"
                        "(*) Creator: " + (task->get_creator() == user_ ? "[YOU]" : task->get_creator()->get_name()) + "\n";
        if (!task->get_collaborators().empty()) {
            task_list += "(*) Collaborators: ";
            for (const auto& collaborator : task->get_collaborators()) {
                if (collaborator == user_) {
                    task_list += "[YOU] ";
                } else {
                    task_list += collaborator->get_name() + " ";
                }
            }
            task_list += "\n";
        }
        task_list += "------------------";
        task_list += "\n";
    }
    notifier_->send(task_list, MessageType::INFO);
}

