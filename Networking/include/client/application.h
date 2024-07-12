#ifndef CLIENT_NETWORKING_INCLUDE_CLIENT_APPLICATION_H_
#define CLIENT_NETWORKING_INCLUDE_CLIENT_APPLICATION_H_

#include "client.h"
#include "user_interface.h"

class Application {
public:
    Application(TCP::Client& client);

    void run();

    void stop();

private:
    void reading_messages();
    TCP::Client& client_;
};

#endif //CLIENT_NETWORKING_INCLUDE_CLIENT_APPLICATION_H_
