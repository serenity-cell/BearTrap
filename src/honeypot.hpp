#ifndef HONEYPOT_HPP
#define HONEYPOT_HPP

#include <boost/asio.hpp>

#include <boost/asio/generic/detail/endpoint.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/tcp.hpp>

class HoneyPot { 
private:
    boost::asio::io_context io;
    boost::asio::ip::tcp::acceptor acceptor;
    boost::asio::ip::tcp::socket socket;

    // class declerations
    int honey_port;
    std::string honey_service;
    std::string honey_banner;

    void acceptConnections(std::shared_ptr<boost::asio::ip::tcp::socket> pSocket, const boost::system::error_code& error);
    
    public:
    HoneyPot(int port, std::string service, std::string banner);
    ~HoneyPot();

    void startListening();
    
};

#endif