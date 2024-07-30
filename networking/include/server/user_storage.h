#ifndef USER_STORAGE_H
#define USER_STORAGE_H

#include <unordered_map>
#include <string>

#include <server/user.h>

class UserStorage {
public:
    static std::shared_ptr<User> get_user(const std::string &username);
    static bool user_exists(const std::string& username) noexcept;

    static void add_user(const std::shared_ptr<User>& user);
private:
    static inline std::unordered_map<std::string, std::shared_ptr<User>> user_map;
};

class UserNotFoundException : public std::exception {
public:
    explicit UserNotFoundException(std::string message) : message_(std::move(message)) {}
    const char *what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};

#endif //USER_STORAGE_H
