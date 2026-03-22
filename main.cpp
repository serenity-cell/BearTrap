#include "src/honeypot.hpp"
#include <cstring>
#include <iostream>

void userOptions(int &arg_count, char **&arg_vector, int &port_input, std::string &service, std::string &banner) {
    for (int i = 1; i < arg_count; i++) {

        // grabs the ip address
        if (strcmp(arg_vector[i], "-port") == 0) {
        port_input = std::stoi(arg_vector[i + 1]);
        }

        // grabs the port input
        if (strcmp(arg_vector[i], "-service") == 0) {
        service = arg_vector[i + 1];
        }
        
        if (strcmp(arg_vector[i], "-banner") == 0) {
        banner =  arg_vector[i + 1];
        }
    }
}

// the main loop for the scanner
int main(int arg_count, char *arg_vector[]) {

    // initializing variables that will be passed to the scanner
    std::string service = "";
    int port_input = -1;
    std::string banner = "";

    // the cli commands and available arguments
    userOptions(arg_count, arg_vector, port_input, service, banner);

    if (port_input == -1 || service.empty() || banner.empty()) {
        std::cerr << "Usage: ./BearTrap -port <port> -service <name> -banner <string>\n";
        return 1;
    }

    HoneyPot honeyPot(port_input, service, banner);
    honeyPot.startListening();
}