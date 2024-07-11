#include <iostream>
#include <thread>

#include "client/Client.h"
#include "client/UserInterface.h"
#include "client/Application.h"

int main() {
    while (true) {
        TCP::Client client{"localhost", 1337};
        Application app(client);
        app.run();
    }
    return 0;
}
