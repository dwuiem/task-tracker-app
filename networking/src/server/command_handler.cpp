#include <complex>
#include <server/command_handler.h>

#include <utility>

CommandHandler::CommandHandler(User user, const std::shared_ptr<MessageSender>& notifier) :
    user_(std::move(user)),
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

std::pair<Command, std::vector<std::string>> CommandHandler::parse_command(const std::string &input) {
    if (input.empty()) throw InvalidCommandException("Command line is empty");
    size_t quote_count = std::count(input.begin(), input.end(), '"');
    if (quote_count % 2 != 0) throw InvalidCommandException("Quotes were not closed");

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

    std::string command = tokens.front();
    tokens.erase(tokens.begin());
    std::vector<std::string> args = tokens;
    return {commands.at(command), args};
}

User CommandHandler::get_user() const {
    return user_;
}

std::optional<Task> CommandHandler::get_selected_task() const {
    return selected_task_;
}

std::shared_ptr<MessageSender> CommandHandler::get_notifier() const {
    return notifier_;
}

void CommandHandler::execute(const std::string &command_line) {
    try {
        switch (const auto& [command, args] = parse_command(command_line); command) {
            case Command::CREATE:
                create_task(args);
                selected_task_ = std::nullopt;
                break;
            case Command::LIST:
                list(args);
                selected_task_ = std::nullopt;
                break;
            case Command::SELECT:
                select_task(args);
                break;
            case Command::EDIT:
                edit(args);
                break;
            case Command::COMPLETE:
                complete();
                break;
            case Command::REMOVE:
                remove();
                selected_task_ = std::nullopt;
                break;
            default:
                break;
        }
    } catch (const std::out_of_range& e) {
        throw InvalidCommandException("Command doesn't exist");
    }
}

void CommandHandler::create_task(const std::vector<std::string> &args) {
    Database& database = Database::get_instance();
    if (args.size() < 3) throw InvalidCommandException("Too few arguments");

    const std::string& title = args[0];

    const std::optional<std::string> description = args[1] == "-" ?
        std::nullopt : static_cast<std::optional<std::string>>(args[1]);
    const std::optional<boost::posix_time::ptime> deadline = args[2] == "-" ?
        std::nullopt : static_cast<std::optional<boost::posix_time::ptime>>(parse_datetime(args[2]));

    std::set<int> collaborators_id;

    for (size_t i = 3; i < args.size(); i++) {
        if (!database.user_exists(args[i])) throw InvalidCommandException("Collaborator " + args[i] + " doesn't exist");
        if (args[i] == user_.get_name()) throw InvalidCommandException("You can't add yourself as a collaborator");
        User collaborator = database.get_user_by_name(args[i]);
        collaborators_id.insert(collaborator.get_id());
    }

    for (int id : collaborators_id) {
        User collaborator = database.get_user_by_id(id);
        notifier_->notify_user(collaborator, "User [" + user_.get_name() + "] added a common task with you");
    }

    boost::posix_time::ptime creation_time = boost::posix_time::second_clock::local_time();
    auto task = Task(0, title, description, deadline, creation_time, user_.get_id());
    database.add_task(task, collaborators_id);

    std::ostringstream out;
    out << GREEN << "You have created task" << RESET;
    notifier_->send(out.str(), MessageType::INFO);
}

void CommandHandler::select_task(const std::vector<std::string> &args) {
    Database& database = Database::get_instance();
    if (args.empty()) throw InvalidCommandException("Specify task ID");
    try {
        Task task = database.get_task_by_id(std::stoi(args[0]));
        if (!database.is_user_collaborator(user_, task) && user_.get_id() != task.get_creator_id()) throw InvalidCommandException("Task is not for you");
        selected_task_ = task;
        notifier_->send("Selected task:"
                        "\n------------------\n" +
                        get_task_info(task) +
                        "\n------------------", MessageType::INFO);
        std::ostringstream guide;
        guide << YELLOW << "Available actions: edit, complete, remove" << RESET;
        notifier_->send(guide.str());
    } catch (const InvalidCommandException&) {
        throw;
    } catch (const std::exception& e) {
        throw InvalidCommandException("Such task doesn't exist");
    }
}

void CommandHandler::list(const std::vector<std::string> &args) const {
    Database& database = Database::get_instance();
    std::vector<Task> user_tasks = database.get_tasks_for_user(user_.get_id());
    if (user_tasks.empty()) {
        notifier_->send("There are no any tasks in your list", MessageType::INFO);
        return;
    }
    std::ostringstream out;
    out << GREEN << "Task list\n" << RESET;
    out << "------------------";
    for (const auto& task : user_tasks) {
        out << "\n";
        out << get_task_info(task);
        out << "\n";
        out << "------------------";
    }
    notifier_->send(out.str(), MessageType::INFO);
}

void CommandHandler::remove() {
    if (!selected_task_.has_value()) throw InvalidCommandException("You didn't select task");
    Database& database = Database::get_instance();
    Task task = selected_task_.value();
    if (user_.get_id() != task.get_creator_id()) throw InvalidCommandException("You can't remove it as collaborator");
    database.delete_task(task);
    std::ostringstream out;
    out << GREEN << "You have deleted task [ID: " << std::to_string(task.get_id()) << "]" << RESET;
    notifier_->send(out.str(), MessageType::INFO);
    const std::vector<User> collaborators = database.get_collaborators_for_task(task.get_id());
    for (const auto& collaborator : collaborators) {
        notifier_->notify_user(collaborator, "User [" + user_.get_name() + "] deleted a task [ID = " + std::to_string(task.get_id()) + "]");
    }
}

// TODO: add notify for collaborators

void CommandHandler::edit(const std::vector<std::string> &args) {
    if (!selected_task_.has_value()) throw InvalidCommandException("You didn't select task");
    Database& database = Database::get_instance();
    Task task = selected_task_.value();
    if (user_.get_id() != task.get_creator_id()) throw InvalidCommandException("You are not allowed to edit this task");
    if (args.size() < 2) throw InvalidCommandException("Too few arguments");
    try {
        switch (EditingAttribute attribute = editing_attributes.at(args[0]); attribute) {
            case EditingAttribute::TITLE: {
                task.set_title(args[1]);
                break;   
            }
            case EditingAttribute::DESCRIPTION: {
                const std::optional<std::string> description = args[1] == "-" ?
                    std::nullopt : static_cast<std::optional<std::string>>(args[1]);
                task.set_description(description);
                break;
            }
            case EditingAttribute::DEADLINE_TIME: {
                const std::optional<boost::posix_time::ptime> time = args[1] == "-" ?
                    std::nullopt : static_cast<std::optional<boost::posix_time::ptime>>(parse_datetime(args[1]));
                task.set_deadline_time(time);
                break;
            }
        }
    } catch (const std::out_of_range&) {
        throw InvalidCommandException("You can edit only 'title', 'description' and 'deadline");
    }
    std::ostringstream out;
    out << GREEN << "You have updated task [ID: " << std::to_string(task.get_id()) << "]" << RESET;
    notifier_->send(out.str(), MessageType::INFO);
    const std::vector<User> collaborators = database.get_collaborators_for_task(task.get_id());
    for (const auto& collaborator : collaborators) {
        notifier_->notify_user(collaborator, "Creator [" + user_.get_name() + "] edited a task [ID = " + std::to_string(task.get_id()) + "]");
    }
    database.update_task(task);
    selected_task_ = task;
}

void CommandHandler::complete() {
    if (!selected_task_.has_value()) throw InvalidCommandException("You didn't select task");
    Database& database = Database::get_instance();
    Task task = selected_task_.value();
    const std::vector<User> collaborators = database.get_collaborators_for_task(task.get_id());
    if (user_.get_id() == task.get_creator_id()) {
        for (const User& collaborator : collaborators) {
            if (!database.get_task_completed(collaborator, task)) throw InvalidCommandException("Not all collaborators have completed task");
        }
        task.complete();
        database.update_task(task);
        for (const auto& collaborator : collaborators) {
            notifier_->notify_user(collaborator, "Creator [" + user_.get_name() + "] completed a task [ID = " + std::to_string(task.get_id()) + "]");
        }
    } else {
        database.complete_collaborator_task(user_, task);
        for (const auto& collaborator : collaborators) {
            if (collaborator.get_id() != user_.get_id()) {
                notifier_->notify_user(collaborator, "Collaborator [" + user_.get_name() + "] did a task [ID = " + std::to_string(task.get_id()) + "]");
            }
        }
        User creator = database.get_user_by_id(task.get_creator_id());
        notifier_->notify_user(creator, "Collaborator [" + user_.get_name() + "] did a task [ID = " + std::to_string(task.get_id()) + "]");
    }
    selected_task_ = task;
}

std::string CommandHandler::get_task_info(const Task &task) const {
    Database& database = Database::get_instance();
    const User creator = database.get_user_by_id(task.get_creator_id());
    const std::vector<User> collaborators = database.get_collaborators_for_task(task.get_id());
    std::ostringstream out;
    out << "### Task ID: " + std::to_string(task.get_id()) + ". Created " << YELLOW << time_to_string(task.get_creation_time()) << RESET << "\n";
    out << "### Title: " + task.get_title() + "\n";
    if (task.get_description().has_value()) out << "### Description: " + task.get_description().value() + "\n";
    if (task.get_deadline().has_value()) out << "### Deadline: " + time_to_string(task.get_deadline().value()) + "\n";
    out << "(!) Status: ";
    if (task.is_completed()) {
        out << GREEN << "COMPLETED\n" << RESET;
    } else {
        out << "UNCOMPLETED\n";
    }
    out << "(*) Creator: " << (creator.get_id() == user_.get_id() ? "[YOU]" : creator.get_name());
    if (!collaborators.empty()) {
        out << "\n";
        out << "(*) Collaborators: ";
        for (const auto& collaborator : collaborators) {
            bool task_completed = database.get_task_completed(collaborator, task);
            if (collaborator.get_id() == user_.get_id()) {
                out << "[YOU]" << " ";
            } else {
                out << collaborator.get_name() << " ";
            }
            if (task_completed) {
                out << "(" << GREEN << "+" << RESET << ")";
            } else {
                out << "(" << RED << "-" << RESET << ")";
            }
            out << " ";
        }
    }

    return out.str();
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
        result = {static_cast<uint16_t>(year), static_cast<uint16_t>(month), static_cast<uint16_t>(day)};
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


