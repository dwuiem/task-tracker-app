#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_USER_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_USER_H_

#include <iostream>
#include <utility>
#include <memory>
#include <vector>

#include "task.h"

class User {
public:
    User() = default;
    explicit User(std::string  name);
    void add_task(const std::shared_ptr<Task>& task);
    std::vector<Task> get_all_tasks();
private:
    std::string name_;
    std::vector<std::shared_ptr<Task>> tasks_{};
};

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_USER_H_
