#include <utility>

#include "server/task.h"

Task::Task(std::string title, std::string description, std::string date, std::shared_ptr<User> creator) :
                                                                               id_(counter++),
                                                                               title_(std::move(title)),
                                                                               description_(std::move(description)),
                                                                               date_(std::move(date)),
                                                                               creator_(std::move(creator)) {}

void Task::set_title(const std::string& title) {
    title_ = title;
}
void Task::set_description(const std::string& description) {
    description_ = description;
}
void Task::set_date(const std::string& date) {
    date_ = date;
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

std::string Task::get_description() const {
    return description_;
}

std::string Task::get_date() const {
    return date_;
}

std::shared_ptr<User> Task::get_creator() const {
    return creator_;
}

std::unordered_set<std::shared_ptr<User>> Task::get_collaborators() const {
    return collaborators_;
}

std::string Task::get_info() const {
    std::string text = "### Task ID: " + std::to_string(id_) + "\n"
                        "### Title: " + title_ + "\n"
                        "### Description: " + description_ + "\n"
                        "### Deadline: " + date_ + "\n"
                        "(*) Creator: " + creator_->get_name() + "\n";
    if (!collaborators_.empty()) {
        text += "(*) Collaborators: ";
        for (const auto& collaborator : collaborators_) {
            text += collaborator->get_name() + " ";
        }
        text += "\n";
    }
    return text;
    }
