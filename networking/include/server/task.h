#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_TASK_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_TASK_H_

#include <string>
#include <utility>
#include <vector>
#include <exception>
#include <stdexcept>
#include <optional>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/regex.hpp>

using boost::posix_time::ptime;

class Task {
public:
    Task(std::string title, std::string description, std::string date);
    void set_title(const std::string& title);
    void set_description(const std::string& description);
    void set_date(const std::string& date);

    std::string get_info() const;
private:
    static inline int counter = 0;
    int id_;
    std::string title_;
    std::string description_;
    std::string date_;

};

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_TASK_H_
