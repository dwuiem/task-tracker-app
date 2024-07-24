#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_TASK_DISTRIBUTOR_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_TASK_DISTRIBUTOR_H_

#include "task.h"
#include "user.h"

#include <unordered_map>
#include <memory>
#include <mutex>

class TaskDistributor {
public:
    static TaskDistributor& get_instance() {
        static TaskDistributor instance;
        return instance;
    }
    void add_task(const std::shared_ptr<Task>& task, std::vector<std::shared_ptr<User>> users);
    std::vector<std::shared_ptr<User>> get_collaborators(const std::shared_ptr<Task>& task) const;
private:
    TaskDistributor() = default;
    std::unordered_map<std::shared_ptr<Task>, std::vector<std::shared_ptr<User>>> distribution_map;
    mutable std::mutex mtx;
};

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_TASK_DISTRIBUTOR_H_
