#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_TASK_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_TASK_H_

#include <string>
#include <unordered_set>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "user.h"

using boost::posix_time::ptime;

class User;

class Task {
public:
    Task(std::string title, std::string description, std::string date, std::shared_ptr<User> creator);
    void set_title(const std::string& title);
    void set_description(const std::string& description);
    void set_date(const std::string& date);

    void add_collaborator(const std::shared_ptr<User>& collaborator);

    std::string get_info() const;
private:
    static inline int counter = 0;
    int id_;
    std::string title_;
    std::string description_;
    std::string date_;

    std::shared_ptr<User> creator_;
    std::unordered_set<std::shared_ptr<User>> collaborators_;

};

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_TASK_H_
