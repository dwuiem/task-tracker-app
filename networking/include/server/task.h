#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_TASK_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_TASK_H_

#include <string>
#include <optional>

#include <boost/date_time/posix_time/posix_time.hpp>

class Task {
    using time_t = boost::posix_time::ptime;
public:
    Task(int id, std::string title,
        std::optional<std::string> description,
        std::optional<time_t> deadline,
        time_t creation_time,
        int creator_id,
        bool completed = false);

    void set_title(const std::string& title);
    void set_description(const std::optional<std::string>& description);
    void set_deadline_time(const std::optional<time_t>& time);
    void complete();

    std::string get_title() const;
    time_t get_creation_time() const;
    std::optional<std::string> get_description() const;
    std::optional<time_t> get_deadline() const;
    int get_creator_id() const;
    int get_id() const;
    bool is_completed() const;
private:
    int id_;
    std::string title_;
    time_t creation_time_;
    std::optional<std::string> description_;
    std::optional<time_t> deadline_;
    int creator_id_;
    bool completed_;
};

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_TASK_H_
