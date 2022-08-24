#ifndef BASIC_SERVER_CLIENT_TCP_SERVER_H
#define BASIC_SERVER_CLIENT_TCP_SERVER_H

#include <functional>
#include <optional>
#include <unordered_set>

#include <boost/asio.hpp>
#include "tcp_connection.h"
#include "exchange/exchange.h"

enum class IPv {
    v4,
    v6
};

class TCPServer {

    using on_join_handler = std::function<void(TCPConnection::pointer)>;
    using on_leave_handler = std::function<void(TCPConnection::pointer)>;
    using on_client_message_handler = std::function<void(std::string)>;
    using on_client_order_handler = std::function<void(std::string)>;

public:
    TCPServer(IPv ip_version, int port);

    int run();

    // sends message to all connected clients
    void broadcast(const std::string& message);

    void add_order(orderType type, orderSide side, const std::string &ticker, int volume, float price);

    void cancel_order(int id);

private:
    void start_accept();

public:
    on_join_handler on_join;
    on_leave_handler on_leave;
    on_client_message_handler on_client_message;

    on_client_order_handler on_client_order;

private:
    IPv _ip_version;
    int _port;

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::acceptor _acceptor;

    std::optional<boost::asio::ip::tcp::socket> _socket;

    std::unordered_set<TCPConnection::pointer> _connections {};

    Exchange exchange;
};


#endif //BASIC_SERVER_CLIENT_TCP_SERVER_H
