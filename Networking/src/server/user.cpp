#include "server/user.h"

User::User(std::string name) : name_(std::move(name)) {}

void User::add_task(const std::shared_ptr<Task>& task) {
    tasks_.push_back(task);
}
