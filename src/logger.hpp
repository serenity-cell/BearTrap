#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address.hpp>
#include <string>
#include <fstream>

class Logger {
    std::ofstream save_file;
    
public:
    Logger();
    ~Logger();
    void logCSV(boost::asio::ip::address arg_ip_address, std::string arg_service, std::string arg_time);

};

#endif