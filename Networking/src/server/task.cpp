#include "server/task.h"

Task::Task(std::string title) : title_(std::move(title)), id_(counter++) {
    description_ = "-";
    date_ = "-";
}

Task::Task(const std::vector<std::string>& args) : id_(counter++) {
    switch (args.size()) {
        case 1:
            title_ = args[0];
            break;
        case 2:
            title_ = args[0];
            description_ = args[1];
            break;
        case 3:
            title_ = args[0];
            description_ = args[1];
            date_ = args[2];
            break;
        default:
            throw std::invalid_argument("No such amount of args");
    }
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
    std::string s = "Task ID: " + std::to_string(id_) + "\n"
                    "Title: " + title_ + "\n"
                    "Description: " + description_ + "\n"
                    "Deadline: " + date_;
    return s;
}
