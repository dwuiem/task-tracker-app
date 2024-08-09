#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_CLIENT_AUTHORIZATION_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_CLIENT_AUTHORIZATION_H_

#include <memory>
#include <string>
#include <unordered_map>

#include <boost/regex.hpp>

#include "database.h"

class SessionAuthorization {
public:
    SessionAuthorization() = default;
protected:
    void authorize_user(const std::string& username);
    std::optional<User> user;
private:
    static inline const boost::regex username_pattern_{"^[a-zA-Z][a-zA-Z0-9_.]{2,15}$"};
};

class InvalidUsernameException : public std::exception {
public:
    explicit InvalidUsernameException(std::string message) : message_(std::move(message)) {}
    const char *what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_CLIENT_AUTHORIZATION_H_
