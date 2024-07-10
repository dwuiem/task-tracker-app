#include <iostream>
#include <thread>

#include "client/Client.h"

void readConsoleInput(TCP::Client& client) {
    while (true) {
        std::string message;
        std::getline(std::cin, message);
        if (message == "\\q") {
            std::cout << "You have left the server\n";
            client.stop();
            break;
        }
        client.send(message);
    }
}

int main() {
    while (true) {
        std::string command;
        std::cout << "Enter \"join\" to connect server\n";
        while (std::getline(std::cin, command)) {
            if (command == "join") {
                break;
            } else {
                std::cout << "Incorrect command\n";
            }
        }

        TCP::Client client{"localhost", 1337};

        std::promise<int> promise;
        std::future<int> future = promise.get_future();

        std::thread connectionThread([&client, &promise] {
            std::cout << "Connecting ...\n";
            client.run(promise);
        });

        bool connected = future.get();

        if (connected) {
            readConsoleInput(client);
        } else {
            std::cout << "Failed to connect\n";
        }
        connectionThread.join();
    }
    return 0;
}
