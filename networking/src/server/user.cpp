#include "server/user.h"

User::User(int id, std::string name): id_(id), name_(std::move(name)) {}

std::string User::get_name() const {
    return name_;
}

int User::get_id() const {
    return id_;
}
