#include <iostream>
#include <thread>

#include "client/client.h"
#include "client/user_interface.h"
#include "client/application.h"

int main() {
    const int default_port = 1337;
    while (true) {
        TCP::Client client{"localhost", default_port};
        Application app(client);
        app.run();
    }
    return 0;
}
