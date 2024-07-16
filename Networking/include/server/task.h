#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_TASK_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_TASK_H_

#include <string>
#include <utility>
#include <vector>
#include <exception>
#include <stdexcept>

class Task {
public:
    Task() = default;
    explicit Task(std::string title);
    Task(const std::vector<std::string>& args);

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
