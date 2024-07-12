#include <iostream>
#include <thread>

#include "client/client.h"
#include "client/user_interface.h"
#include "client/application.h"

int main() {
    while (true) {
        TCP::Client client{"localhost", 1337};
        Application app(client);
        app.run();
    }
    return 0;
}
