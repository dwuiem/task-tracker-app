#include "server/session_authorization.h"

void SessionAuthorization::authorize_user(const std::string &username) {
    if (!boost::regex_match(username, username_pattern_)) throw InvalidUsernameException(ON_AUTH_FAIL);
    try {
        user = UserStorage::get_user(username);
    } catch (const UserNotFoundException&) {
        user = std::make_shared<User>(username);
        UserStorage::add_user(user);
    }
}
