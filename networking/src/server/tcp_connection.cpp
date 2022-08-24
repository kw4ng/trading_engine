#include <iostream>
#include "networking/server/tcp_connection.h"

TCPConnection::TCPConnection(boost::asio::ip::tcp::socket&& socket) : _socket(std::move(socket)) {
    boost::system::error_code ec;

    std::stringstream name;
    name << _socket.remote_endpoint();

    _username = name.str();
}

void TCPConnection::start(messageHandler&& message_handler, orderHandler&& order_handler, errorHandler&& error_handler) {
//    auto strong_this = shared_from_this();
//
//    boost::asio::async_write(_socket, boost::asio::buffer(_message),
//                                [strong_this](const boost::system::error_code &error, size_t bytes_transferred) {
//         if (error) {
//             std::cout << "Failed to send message!\n";
//         } else {
//             std::cout << "Sent " << bytes_transferred << " bytes of data!\n";
//         }
//     });
//
//
//    boost::asio::streambuf buffer;
//
//    _socket.async_receive(buffer.prepare(512), [this](const boost::system::error_code& error, size_t bytes_transferred) {
//        if (error == boost::asio::error::eof) {
//            // clean connection cut off
//            std::cout << "Client disconnected properly!\n";
//        } else if (error) {
//            std::cout << "Client disconnected in bad way!\n";
//        }
//    });
        _message_handler = std::move(message_handler);
        _order_handler = std::move(order_handler);
        _error_handler = std::move(error_handler);

        async_read();
}

void TCPConnection::post(const std::string& message) {
    bool queue_idle = _outgoing_messages.empty();
    _outgoing_messages.push(message);

    if (queue_idle) {
        async_write();
    }
}

void TCPConnection::async_read() {
    boost::asio::async_read_until(_socket, _streambuf, "\n", [self = shared_from_this()]
    (boost::system::error_code ec, size_t bytes_transferred) {
        self->on_read(ec, bytes_transferred);
    });
}

void TCPConnection::on_read(boost::system::error_code ec, size_t bytes_transferred) {
    if (ec) {
        _socket.close();

        // add error handler
        _error_handler();
        return;
    }

    std::stringstream message;
    message << _username << ": " << std::istream(&_streambuf).rdbuf();

    std::cout << message.str();

    // add message handler
    _message_handler(message.str());
    _order_handler(message.str());
    async_read();
}

void TCPConnection::async_write() {
    boost::asio::async_write(_socket, boost::asio::buffer(_outgoing_messages.front()), [self = shared_from_this()]
            (boost::system::error_code ec, size_t bytes_transferred) {
        self->on_write(ec, bytes_transferred);
    });
}

void TCPConnection::on_write(boost::system::error_code ec, size_t bytes_transferred) {
    if (ec) {
        _socket.close();

        // add error handler
        _error_handler();
        return;
    }

    _outgoing_messages.pop();

    if (!_outgoing_messages.empty()) {
        async_write();
    }
}