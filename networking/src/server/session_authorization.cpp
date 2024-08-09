#include "server/session_authorization.h"

void SessionAuthorization::authorize_user(const std::string &username) {
    if (!boost::regex_match(username, username_pattern_)) {
        throw InvalidUsernameException("User name is incorrect. Please retry");
    }
    Database& db = Database::get_instance();
    if (!db.user_exists(username)) {
        db.add_user(username);
    }
    user = db.get_user_by_name(username);
}
