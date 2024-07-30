#ifndef MESSAGE_SENDER_H
#define MESSAGE_SENDER_H

#include <memory>
#include <string>

#include "user.h"

class MessageSender {
public:
    virtual ~MessageSender() = default;

    virtual void send(const std::string& message) = 0;
    virtual void send_to_user(std::shared_ptr<User> user, const std::string& message) = 0;
};

#endif //MESSAGE_SENDER_H