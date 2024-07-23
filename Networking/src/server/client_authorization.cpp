#include "server/client_authorization.h"

ClientAuthorization::ClientAuthorization(TCP::Connection connection)  : connection_(connection) {}

void ClientAuthorization::authorize_user(const std::string& username) {
    if (!boost::regex_match(username, username_pattern_)) throw InvalidUsernameException(ON_AUTH_FAIL);
    if (user_exists(username)) {
        user = user_map.at(username);
    } else {
        user = std::make_shared<User>(username);
        user_map[username] = user;
    }
}

bool ClientAuthorization::user_exists(const std::string& username) noexcept {
    return user_map.find(username) != user_map.end();
}
