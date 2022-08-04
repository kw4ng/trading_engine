//
// Created by kevin on 8/4/2022.
//

#ifndef BASIC_SERVER_CLIENT_TCP_CLIENT_H
#define BASIC_SERVER_CLIENT_TCP_CLIENT_H

#include <queue>

#include <boost/asio.hpp>

using messageHandler = std::function<void(std::string)>;

class TCPClient {
public:
    TCPClient(const std::string& address, int port);

    void run();

    void stop();

    void post(const std::string& message);

private:
    void async_read();

    void on_read(boost::system::error_code ec, size_t bytes_transferred);

    void async_write();

    void on_write(boost::system::error_code ec, size_t bytes_transferred);

public:
    messageHandler on_message;

private:
    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket;

    boost::asio::ip::tcp::resolver::results_type _endpoints;

    boost::asio::streambuf _streambuf { 65536 };

    std::queue<std::string> _outgoing_messages;
};


#endif //BASIC_SERVER_CLIENT_TCP_CLIENT_H
