#include <server/user_storage.h>

std::shared_ptr<User> UserStorage::get_user(const std::string &username) {
    if (!user_exists(username)) throw UserNotFoundException("No such user");
    return user_map.at(username);
}

void UserStorage::add_user(const std::shared_ptr<User>& user) {
    user_map[user->get_name()] = user;
}

bool UserStorage::user_exists(const std::string &username) noexcept {
    return user_map.contains(username);
}
