#include <server/command_handler.h>

CommandHandler::CommandHandler(const std::shared_ptr<User>& user, const std::shared_ptr<MessageSender>& notifier) :
    user_(user),
    notifier_(notifier) {}

boost::posix_time::ptime CommandHandler::parse_datetime(const std::string &input) {
    std::istringstream iss(input);
    std::string first;
    std::string second;
    iss >> first >> second;
    if (!second.empty()) {
        boost::gregorian::date date = parse_date(first);
        boost::posix_time::time_duration time = parse_time(second);
        return {date, time};
    }
    if (boost::regex_match(first, full_date_pattern) || boost::regex_match(first, month_date_pattern)) {
        const boost::posix_time::hours max_hours(23);
        const boost::posix_time::minutes max_minutes(59);
        return {parse_date(first), max_hours + max_minutes};
    }
    return {boost::gregorian::day_clock::local_day(), parse_time(first)};
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
    std::optional<std::string> description = std::nullopt;
    if (args[1] != "-") {
        description = args[1];
    }
    std::optional<boost::posix_time::ptime> deadline = std::nullopt;
    if (args[2] != "-") {
        deadline = parse_datetime(args[2]);
    }
    std::unordered_set<std::shared_ptr<User>> collaborators;
    for (size_t i = 3; i < args.size(); i++) {
        try {
            auto collaborator = UserStorage::get_user(args[i]);
            if (collaborator->get_id() == user_->get_id()) throw InvalidCommandException("You can't add yourself as a collaborator");
            collaborators.insert(collaborator);
        } catch (const UserNotFoundException&) {
            throw InvalidCommandException("User not found");
        }
    }
    std::shared_ptr<Task> task = std::make_shared<Task>(title, description, deadline, user_);
    for (const auto& collaborator : collaborators) {
        task->add_collaborator(collaborator);
        collaborator->add_task(task);
        notifier_->send_to_user(collaborator, "User \"" + user_->get_name() + "\" added a task for you");
    }
    user_->add_task(task);
    std::ostringstream out;
    out << GREEN << "You have created task" << RESET;
    notifier_->send(out.str(), MessageType::INFO);
}

void CommandHandler::select_task(const std::vector<std::string> &args) {
    if (args.empty()) throw InvalidCommandException("Specify task ID");

}

void CommandHandler::list(const std::vector<std::string> &args) const {
    if (user_->get_all_tasks().empty()) {
        notifier_->send("There are no any tasks in your list", MessageType::INFO);
        return;
    }
    std::ostringstream out;
    out << GREEN << "Task list\n" << RESET;
    out << "------------------\n";
    for (const auto& task : user_->get_all_tasks()) {
        out << "### Task ID: " + std::to_string(task->get_id()) + ". Created " << YELLOW << time_to_string(task->get_creation_time()) << RESET << "\n";
        out << "### Title: " + task->get_title() + "\n";
        if (task->get_description().has_value()) out << "### Description: " + task->get_description().value() + "\n";
        if (task->get_deadline().has_value()) out << "### Deadline: " + time_to_string(task->get_deadline().value()) + "\n";
        if (task->is_completed()) out << "(!) Status:" << GREEN << "COMPLETED\n" << RESET;
        out << "(*) Creator: " + (task->get_creator() == user_ ? "[YOU]" : task->get_creator()->get_name()) + "\n";
        if (!task->get_collaborators().empty()) {
            out << "(*) Collaborators: ";
            for (const auto& collaborator : task->get_collaborators()) {
                if (collaborator == user_) {
                    out << "[YOU] ";
                } else {
                    out << collaborator->get_name() + " ";
                }
            }
            out << "\n";
        }
        out << "------------------";
        out << "\n";
    }
    notifier_->send(out.str(), MessageType::INFO);
}

boost::gregorian::date CommandHandler::parse_date(const std::string &input) {
    boost::gregorian::date result;
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

boost::posix_time::time_duration CommandHandler::parse_time(const std::string &input) {
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
        return {h + m};
    }
    throw InvalidCommandException("Time format incorrect");
}

std::string CommandHandler::time_to_string(boost::posix_time::ptime pt) {
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << pt.date().day() << '.';
    oss << std::setw(2) << std::setfill('0') << pt.date().month().as_number() << '.';
    oss << std::setw(4) << std::setfill('0') << pt.date().year() << ' ';
    oss << std::setw(2) << std::setfill('0') << pt.time_of_day().hours() << ':';
    oss << std::setw(2) << std::setfill('0') << pt.time_of_day().minutes();
    return oss.str();
}


