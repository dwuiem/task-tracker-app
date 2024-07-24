#include <utility>

#include "server/task.h"

Task::Task(std::string title, std::string description, std::string date) : id_(counter++),
                                                                           title_(std::move(title)),
                                                                           description_(std::move(description)),
                                                                           date_(std::move(date)) {

}

void Task::set_title(const std::string& title) {
    title_ = title;
}
void Task::set_description(const std::string& description) {
    description_ = description;
}
void Task::set_date(const std::string& date) {
    date_ = date;
}
std::string Task::get_info() const {
    std::string s = "### Task ID: " + std::to_string(id_) + "\n"
                    "### Title: " + title_ + "\n"
                    "### Description: " + description_ + "\n"
                    "### Deadline: " + date_;
    return s;
}
