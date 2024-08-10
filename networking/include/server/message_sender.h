#ifndef MESSAGE_SENDER_H
#define MESSAGE_SENDER_H

#include <functional>
#include <string>

#include "user.h"

enum class MessageType {
    INFO,
    NOTIFY,
    EXCEPTION
};

class MessageSender {
public:
    explicit MessageSender(std::function<void(const User& user, const std::string& message)> notifier);
    virtual ~MessageSender() = default;

    virtual void send(const std::string& message, MessageType message_type = MessageType::INFO) = 0;
    std::function<void(const User& user, const std::string& message)> notify_user;
};

#endif //MESSAGE_SENDER_H
