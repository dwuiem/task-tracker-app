#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_EVENT_HANDLER_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_EVENT_HANDLER_H_

#include <string>
#include <iostream>

class EventHandler {
public:
    static void on_connect(std::string ip_address);
    static void on_disconnect(std::string ip_address);
    static void on_command(std::string ip_address, std::string command);
};

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_EVENT_HANDLER_H_
