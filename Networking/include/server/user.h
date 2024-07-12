#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_USER_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_USER_H_

#include <iostream>

class User {
public:
    User() = default;
    User(std::string name);
private:
    std::string name_;
};

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_USER_H_
