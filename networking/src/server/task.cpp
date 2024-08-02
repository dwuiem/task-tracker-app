#include <utility>

#include "server/task.h"

Task::Task(std::string title, std::optional<std::string> description, std::optional<time_t> deadline, std::shared_ptr<User> creator) :
                                                                               id_(counter++),
                                                                               title_(std::move(title)),
                                                                               description_(std::move(description)),
                                                                               deadline_(deadline),
                                                                               creator_(std::move(creator)) {
    creation_time_ = boost::posix_time::second_clock::local_time();
}

void Task::set_title(const std::string& title) {
    title_ = title;
}
void Task::set_description(const std::string& description) {
    description_ = description;
}

void Task::add_collaborator(const std::shared_ptr<User>& collaborator) {
    collaborators_.insert(collaborator);
}

int Task::get_id() const {
    return id_;
}

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

std::shared_ptr<User> Task::get_creator() const {
    return creator_;
}

std::unordered_set<std::shared_ptr<User>> Task::get_collaborators() const {
    return collaborators_;
}
