//
// Created by denis on 10.07.2024.
//

#ifndef CLIENT_NETWORKING_INCLUDE_CLIENT_APPLICATION_H_
#define CLIENT_NETWORKING_INCLUDE_CLIENT_APPLICATION_H_

#include "Client.h"
#include "UserInterface.h"

class Application {
public:
    Application(TCP::Client& client);

    void run();

    void stop();

private:
    void readMessages();
    TCP::Client& _client;
};

#endif //CLIENT_NETWORKING_INCLUDE_CLIENT_APPLICATION_H_
