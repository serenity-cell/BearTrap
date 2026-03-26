#include "honeypot.hpp"
#include "logger.hpp"
#include <boost/asio/generic/detail/endpoint.hpp>
#include <boost/asio/impl/read.hpp>
#include <boost/asio/impl/write.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/system/detail/error_code.hpp>
#include <iostream>
#include <iomanip>
#include <memory>
#include <string>

HoneyPot::HoneyPot (int input_port, std::string input_service, std::string input_banner) {
    honey_port = input_port;
    honey_service = input_service;
    honey_banner = input_banner;


    const size_t count = 5;
    Vacceptor.reserve(count);
    
    for (size_t i = 0; i < count; ++i) {

        auto acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), honey_port + i));
        Vacceptor.push_back(acceptor);
    }

}

HoneyPot::~HoneyPot() {
    for (size_t i = 0; i < Vacceptor.size(); i++ ) {
        if (Vacceptor[i]->is_open()) {
            Vacceptor[i]->cancel();
            Vacceptor[i]->close();

        
        }
    }
}

void HoneyPot::startListening() {

    for (int i = 0; i < Vacceptor.size(); i++) {
        auto pSocket = std::make_shared<boost::asio::ip::tcp::socket>(io);
        Vacceptor[i]->async_accept(*pSocket, [this,pSocket, i](const boost::system::error_code& error) {
        acceptConnections(pSocket, error, i);}
        );
    }
    io.run();
}

void HoneyPot::acceptConnections(std::shared_ptr<boost::asio::ip::tcp::socket> pSocket, const boost::system::error_code& error, int i) {

    auto readBuffer = std::make_shared<std::array<char, 1024>>();

    // read callback
    std::function <void (const boost::system::error_code& error, std::size_t bytes_transferred)> readCallback = [this, pSocket, readBuffer, i] 
    (const boost::system::error_code& error, std::size_t bytes_transferred) {
        
        std::string data(readBuffer->begin(), readBuffer->begin() + bytes_transferred);
        std::cout << "received: " << data << "\n";

        auto newPsocket = std::make_shared<boost::asio::ip::tcp::socket>(io);

            Vacceptor[i]->async_accept(*newPsocket, [this, newPsocket, i](const boost::system::error_code& error) {
                acceptConnections(newPsocket, error, i);} 
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
