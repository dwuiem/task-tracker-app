#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_TASK_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_TASK_H_

#include <string>
#include <unordered_set>
#include <optional>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "user.h"

class User;

class Task {
    using time_t = boost::posix_time::ptime;
public:
    Task(std::string title, std::optional<std::string> description, std::optional<time_t> deadline, std::shared_ptr<User> creator);
    void set_title(const std::string& title);
    void set_description(const std::string& description);

    void add_collaborator(const std::shared_ptr<User>& collaborator);

    int get_id() const;
    std::string get_title() const;
    std::optional<std::string> get_description() const;
    std::optional<time_t> get_deadline() const;

    std::shared_ptr<User> get_creator() const;
    std::unordered_set<std::shared_ptr<User>> get_collaborators() const;
private:
    static inline int counter = 0;
    int id_;
    std::string title_;
    std::optional<std::string> description_;
    std::optional<time_t> deadline_;

    std::shared_ptr<User> creator_;
    std::unordered_set<std::shared_ptr<User>> collaborators_;

};

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_TASK_H_
