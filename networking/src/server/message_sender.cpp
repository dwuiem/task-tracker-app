#include <utility>

#include "../../include/server/message_sender.h"

MessageSender::MessageSender(std::function<void(const User &user, const std::string &message)> notifier) :
notify_user(std::move(notifier)) {}
