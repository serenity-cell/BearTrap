#ifndef HONEYPOT_HPP
#define HONEYPOT_HPP

#include <boost/asio.hpp>

#include <boost/asio/generic/detail/endpoint.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <vector>
#include "logger.hpp"

class HoneyPot { 
private:
    boost::asio::io_context io;
    std::vector<std::shared_ptr <boost::asio::ip::tcp::acceptor>> Vacceptor;
    Logger logs;

    // class declerations
    int honey_port;
    std::string honey_service;
    std::string honey_banner;

    

    void acceptConnections(std::shared_ptr<boost::asio::ip::tcp::socket> pSocket, const boost::system::error_code& error, int vector_size);

    public:
    HoneyPot(int port, std::string service, std::string banner);
    ~HoneyPot();

    void startListening();
    
};

#endif