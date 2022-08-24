#include <iostream>

#include "networking/server/tcp_server.h"


using boost::asio::ip::tcp;

TCPServer::TCPServer(IPv ip_version, int port) : _ip_version(ip_version), _port(port),
        _acceptor(_io_context, tcp::endpoint(ip_version == IPv::v4 ? tcp::v4() : tcp::v6(), _port)) {}

int TCPServer::run() {
    std::cout << "Starting trading engine server... \n" << std::endl;

    try {
        start_accept();
        _io_context.run();
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}

void TCPServer::broadcast(const std::string& message) {
    for (auto& connection : _connections) {
        connection->post(message);
    }
}

void TCPServer::add_order(orderType type, orderSide side, const std::string &ticker, int volume, float price = 0) {
    exchange.add_order(type, ticker, price, volume, side);
}

void TCPServer::cancel_order(int id) {
    exchange.cancel_order(id);
}

void TCPServer::start_accept() {
    // create new socket
    _socket.emplace(_io_context);

    // async accept the connection
    _acceptor.async_accept(*_socket, [this](const boost::system::error_code& error) {
        // create connection
        auto connection = TCPConnection::create(std::move(*_socket));

        if (on_join) {
            on_join(connection);
        }

        _connections.insert(connection);

        if (!error) {
            connection->start(
                [this](const std::string& message) {
                    if (on_client_message) {
                        on_client_message(message);
                    }
                },

                [this](const std::string& message) {
                    if (on_client_order) {
                        on_client_order(message);
                    }
                },

                [&, weak = std::weak_ptr(connection)] {
                    if (auto shared = weak.lock(); shared && _connections.erase(shared)) {
                        if (on_leave) {
                            on_leave(shared);
                        }
                    }
                }
            );
        }

        start_accept();
    });

}
