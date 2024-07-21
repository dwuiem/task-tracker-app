#include "server/user.h"

User::User(std::string name) : name_(std::move(name)), id_(counter++) {}

void User::add_task(const std::shared_ptr<Task>& task) {
    tasks_.push_back(task);
}
std::vector<std::shared_ptr<Task>> User::get_all_tasks() const {
    return tasks_;
}
std::string User::get_name() const {
    return name_;
}
