#ifndef COMMAND_EXECUTOR_H
#define COMMAND_EXECUTOR_H

#include "user_storage.h"
#include "message_sender.h"
#include "connection.h"

#include <boost/regex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <optional>

class CommandHandler {
public:
    explicit CommandHandler(const std::shared_ptr<User>& user, const std::shared_ptr<MessageSender>& notifier);

    static boost::posix_time::ptime parse_datetime(const std::string& input);

    static std::pair<std::string, std::vector<std::string>> parse_command(const std::string& input);

    void execute(const std::string& command_line);
private:
    void create_task(const std::vector<std::string>& args);
    void select_task(const std::vector<std::string>& args);
    void list(const std::vector<std::string>& args) const;

    static boost::gregorian::date parse_date(const std::string &input);
    static boost::posix_time::time_duration parse_time(const std::string& input);

    static std::string time_to_string(boost::posix_time::ptime time);

    inline static const boost::regex command_pattern{R"((?:[^\s"]+|"[^"]*")+)"};
    inline static const boost::regex full_date_pattern{R"((\d{2})[./](\d{2})[./](\d{2,4}))"};
    inline static const boost::regex month_date_pattern{R"((\d{2})[./](\d{2}))"};
    inline static const boost::regex time_pattern{R"((\d{1,2}):(\d{2}))"};

    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> commands = {
        {"create", [this](const std::vector<std::string>& args) {
            create_task(args);
        }},
        {"list", [this](const std::vector<std::string>& args) {
            list(args);
        }},
        {"select", [this](const std::vector<std::string>& args) {
            select_task(args);
        }}
    };

    std::shared_ptr<User> user_;
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
