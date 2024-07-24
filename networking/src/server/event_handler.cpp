#include "server/event_handler.h"

void EventHandler::on_connect(std::string ip_address) {
    std::cout << "client " << ip_address << " has joined" << std::endl;
}

void EventHandler::on_disconnect(std::string ip_address) {
    std::cout << "client " << ip_address << " has left" << std::endl;
}

void EventHandler::on_command(std::string ip_address, std::string command) {
    std::cout << "client " << ip_address << " sent command:" << std::endl;
    std::cout << "~ " << command << std::endl;
}
