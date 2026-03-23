#include "honeypot.hpp"
#include "logger.hpp"
#include <asm-generic/socket.h>
#include <boost/asio/generic/detail/endpoint.hpp>
#include <boost/asio/impl/read.hpp>
#include <boost/asio/impl/write.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/system/detail/error_code.hpp>
#include <iostream>
#include <iomanip>
#include <memory>
#include <string>

HoneyPot::HoneyPot (int input_port, std::string input_service, std::string input_banner) 
: acceptor(io), socket(io){
    honey_port = input_port;
    honey_service = input_service;
    honey_banner = input_banner;

}

HoneyPot::~HoneyPot() {
    if (acceptor.is_open()) {
        acceptor.close();
        acceptor.cancel();
    }
}

void HoneyPot::startListening() {
    boost::asio::ip::tcp::endpoint endPoint (boost::asio::ip::tcp::v4(), honey_port);

    // setting up acceptor
    acceptor.open(boost::asio::ip::tcp::v4());
    acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(endPoint);
    acceptor.listen();

    auto pSocket = std::make_shared<boost::asio::ip::tcp::socket>(io);
    acceptor.async_accept(*pSocket, [this, pSocket](const boost::system::error_code& error) {
        acceptConnections(pSocket, error);}
    );

    io.run();
}

void HoneyPot::acceptConnections(std::shared_ptr<boost::asio::ip::tcp::socket> pSocket, const boost::system::error_code& error) {

    auto readBuffer = std::make_shared<std::array<char, 1024>>();

    // read callback
    std::function <void (const boost::system::error_code& error, std::size_t bytes_transferred)> readCallback = [this, pSocket, readBuffer] 
    (const boost::system::error_code& error, std::size_t bytes_transferred) {
        
        std::string data(readBuffer->begin(), readBuffer->begin() + bytes_transferred);
        std::cout << "received: " << data << "\n";

        auto newPsocket = std::make_shared<boost::asio::ip::tcp::socket>(io);

        acceptor.async_accept(*newPsocket, [this, newPsocket](const boost::system::error_code& error) {
        acceptConnections(newPsocket, error);} 
        );
    };

    // write callback
    std::function <void (const boost::system::error_code& error, std::size_t bytes_transferred)> writeCallback =
    [pSocket, readCallback, readBuffer] (const boost::system::error_code& error, std::size_t bytes_transferred) {

        pSocket->async_read_some( boost::asio::buffer(*readBuffer), 
        readCallback );
    };

    if (error.value() == 0) {

        // delcaring variables for output and storing 
        auto now = std::time(nullptr);
        auto ip_address = pSocket->remote_endpoint().address();
        std::string timeStr = std::ctime(&now);
        timeStr.pop_back(); 


        // outputting data of the intruder ip, what they tried to intrude and the timestamp
        std::cout << std::left
            << std::setw(16) << ip_address
            << std::setw(14) << "service hit: "
            << std::setw(6)  << honey_service
            << std::setw(1) << "["
            << std::setw(6) << timeStr << "]"
            << std::endl;

        logs.logCSV(ip_address, honey_service, timeStr);


        boost::asio::async_write(*pSocket, boost::asio::buffer(honey_banner), writeCallback);
    }
    else {
        return;
    }

};
