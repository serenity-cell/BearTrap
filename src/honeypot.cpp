#include "honeypot.hpp"
#include <boost/asio/generic/detail/endpoint.hpp>
#include <boost/asio/impl/read.hpp>
#include <boost/asio/impl/write.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/system/detail/error_code.hpp>
#include <iostream>
#include <memory>
#include <string>

/*
the io_context        ← engine (you know this)
the acceptor          ← tcp::acceptor, listens on a port
port number           ← which port to fake
service name          ← what service to pretend to be
fake banner           ← string to send when someone connects
log file              ← where to save connection attempts

What Methods It Needs
constructor           ← takes port, service name, banner
startListening()      ← opens acceptor, starts async_accept


acceptConnection()    ← callback when someone connects
                         reads what they send
                         sends fake banner back
                         logs everything
                         calls acceptConnection() again
*/

HoneyPot::HoneyPot (int input_port, std::string input_service, std::string input_banner) 
: acceptor(io), socket(io){
    honey_port = input_port;
    honey_service = input_service;
    honey_banner = input_banner;
}

void HoneyPot::startListening() {
    boost::asio::ip::tcp::endpoint endPoint (boost::asio::ip::tcp::v4(), honey_port);

    // setting up acceptor
    acceptor.open(boost::asio::ip::tcp::v4());
    acceptor.bind(endPoint);
    acceptor.listen();

    auto pSocket = std::make_shared<boost::asio::ip::tcp::socket>(io);

    acceptor.async_accept(*pSocket, [this, pSocket](const boost::system::error_code& error) {
        acceptConnections(pSocket, error);}
    );

    io.run();
}

void HoneyPot::acceptConnections(std::shared_ptr<boost::asio::ip::tcp::socket> pSocket, const boost::system::error_code& error) {
    // read callback
    std::function <void (const boost::system::error_code& error, std::size_t bytes_transferred)> readCallback = [this, pSocket] 
    (const boost::system::error_code& error, std::size_t bytes_transferred) {
        
        std::cout << pSocket->remote_endpoint().address() << "\n";

        auto newPsocket = std::make_shared<boost::asio::ip::tcp::socket>(io);

    };

    // write callback
    std::function <void (const boost::system::error_code& error)> writeCallback = [pSocket, readCallback] (const boost::system::error_code& error) {
        auto readBuffer = std::make_shared<std::array<char, 1024>>();

        boost::asio::async_read(*pSocket, boost::asio::buffer(*readBuffer), 
        readCallback );

    };


    auto newPsocket = std::make_shared<boost::asio::ip::tcp::socket>(io);
    if (error.value() == 0) {

        boost::asio::async_write(*pSocket, boost::asio::buffer(honey_banner), writeCallback);

        acceptor.async_accept(*newPsocket, [this, newPsocket](const boost::system::error_code& error) {
        acceptConnections(newPsocket, error);} 
        );
    }
    else {
        return;
    }

};
