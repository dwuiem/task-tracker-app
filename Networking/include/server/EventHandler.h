//
// Created by denis on 11.07.2024.
//

#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_EVENTHANDLER_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_EVENTHANDLER_H_

#include <string>
#include <iostream>

class EventHandler {
public:
    static void onConnect(std::string ipAddress);
    static void onDisconnect(std::string ipAddress);
    static void onClientMessage(std::string ipAddress, std::string message);
};

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_EVENTHANDLER_H_
