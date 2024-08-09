#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_USER_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_USER_H_

#include <string>

class User {
public:
    explicit User(int id, std::string name);
    std::string get_name() const;
    int get_id() const;
private:
    int id_;
    std::string name_;
};

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_USER_H_
