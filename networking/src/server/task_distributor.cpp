#include <utility>

#include "server/task_distributor.h"

void TaskDistributor::add_task(const std::shared_ptr<Task>& task, std::vector<std::shared_ptr<User>> users) {
    std::lock_guard<std::mutex> lock(mtx);
    distribution_map[task] = users;
    for (auto user : users) {
        user->add_task(task);
    }
}
std::vector<std::shared_ptr<User>> TaskDistributor::get_collaborators(const std::shared_ptr<Task>& task) const {
    return distribution_map.at(task);
}
