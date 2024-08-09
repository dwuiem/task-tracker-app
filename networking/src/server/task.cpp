#include <utility>

#include "server/task.h"

Task::Task(int id, std::string title,
    std::optional<std::string> description,
    std::optional<time_t> deadline,
    time_t creation_time,
    int creator_id) :
        id_(id),
        title_(std::move(title)),
        description_(std::move(description)),
        deadline_(deadline),
        creation_time_(creation_time),
        creator_id_(creator_id) {}

std::string Task::get_title() const {
    return title_;
}

Task::time_t Task::get_creation_time() const {
    return creation_time_;
}

std::optional<std::string> Task::get_description() const {
    return description_;
}

std::optional<Task::time_t> Task::get_deadline() const {
    return deadline_;
}

int Task::get_creator_id() const {
    return creator_id_;
}

int Task::get_id() const {
    return id_;
}
