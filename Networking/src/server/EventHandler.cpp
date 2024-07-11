//
// Created by denis on 11.07.2024.
//

#include "server/EventHandler.h"

void EventHandler::onConnect(std::string ipAddress) {
    std::cout << "Client " << ipAddress << " has joined" << std::endl;
}

void EventHandler::onDisconnect(std::string ipAddress) {
    std::cout << "Client " << ipAddress << " has left" << std::endl;
}

void EventHandler::onClientMessage(std::string ipAddress, std::string message) {
    std::cout << "Client " << ipAddress << " sent message:" << std::endl;
    std::cout << "~ " << message;
}
