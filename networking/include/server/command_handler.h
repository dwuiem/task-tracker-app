#ifndef COMMAND_EXECUTOR_H
#define COMMAND_EXECUTOR_H

#include "message_sender.h"
#include "connection.h"
#include "database.h"

#include <boost/regex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <optional>

enum class Command {
    CREATE,
    LIST,
    SELECT,
    EDIT,
    COMPLETE,
    REMOVE
};

enum class EditingAttribute {
    TITLE,
    DESCRIPTION,
    DEADLINE_TIME,
};

class CommandHandler {
public:
    explicit CommandHandler(User user, const std::shared_ptr<MessageSender>& notifier);

    static boost::posix_time::ptime parse_datetime(const std::string& input);

    static std::pair<Command, std::vector<std::string>> parse_command(const std::string& input);

    User get_user() const;
    std::shared_ptr<MessageSender> get_notifier() const;

    void execute(const std::string& command_line);
private:
    void create_task(const std::vector<std::string>& args);
    void select_task(const std::vector<std::string>& args);
    void list(const std::vector<std::string>& args) const;
    void edit(const std::vector<std::string>& args);
    void complete();

    std::string get_task_info(const Task &task) const;

    static boost::gregorian::date parse_date(const std::string &input);
    static boost::posix_time::time_duration parse_time(const std::string& input);

    static std::string time_to_string(boost::posix_time::ptime time);

    inline static const std::unordered_map<std::string, Command> commands = {
        {"create", Command::CREATE},
        {"list", Command::LIST},
        {"select", Command::SELECT},
        {"edit", Command::EDIT},
        {"remove", Command::REMOVE},
    };

    inline static const std::unordered_map<std::string, EditingAttribute> editing_attributes = {
        {"title", EditingAttribute::TITLE},
        {"description", EditingAttribute::DESCRIPTION},
        {"deadline", EditingAttribute::DEADLINE_TIME},
    };

    inline static const boost::regex command_pattern{R"((?:[^\s"]+|"[^"]*")+)"};
    inline static const boost::regex full_date_pattern{R"((\d{2})[./](\d{2})[./](\d{2,4}))"};
    inline static const boost::regex month_date_pattern{R"((\d{2})[./](\d{2}))"};
    inline static const boost::regex time_pattern{R"((\d{1,2}):(\d{2}))"};

    User user_;
    std::optional<Task> selected_task_;
    std::shared_ptr<MessageSender> notifier_;
};

class InvalidCommandException : public std::exception {
public:
    explicit InvalidCommandException(std::string message) : message_(std::move(message)) {}
    const char *what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};


#endif //COMMAND_EXECUTOR_H
