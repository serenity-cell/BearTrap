#include "logger.hpp"
#include <fstream>
#include <iostream>
#include <ostream>

Logger::Logger() {
    save_file.open("data.csv", std::ios::app);
}

Logger::~Logger() {
    if (save_file.is_open()) {
        save_file.close();
    }

}

void Logger::logCSV(boost::asio::ip::address ip_address, std::string service, std::string time) {

    if (!save_file.is_open()) {
        std::cerr <<  "ERROR: couldn't open storage file";
        return;
    }

    save_file << ip_address << "," << service << "," << time << "\n";


}