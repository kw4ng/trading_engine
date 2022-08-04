#include "networking/client/tcp_client.h"

TCPClient::TCPClient(const std::string &address, int port) : _socket(_io_context) {
    boost::asio::ip::tcp::resolver resolver(_io_context);
    _endpoints = resolver.resolve(address, std::to_string(port));
}

void TCPClient::run() {
    boost::asio::async_connect(_socket, _endpoints, [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint ep) {
        if (!ec) {
            async_read();
        }
    });

    _io_context.run();
}

void TCPClient::stop() {
    boost::system::error_code ec;
    _socket.close(ec);

    if (ec) {
        // process error
    }
}

void TCPClient::post(const std::string &message) {
    bool queue_idle = _outgoing_messages.empty();
    _outgoing_messages.push(message);

    if (queue_idle) {
        async_write();
    }
}

void TCPClient::async_read() {
    boost::asio::async_read_until(_socket, _streambuf, "\n", [this](boost::system::error_code ec, size_t bytes_transferred) {
        on_read(ec, bytes_transferred);
    });
}

void TCPClient::on_read(boost::system::error_code ec, size_t bytes_transferred) {
    if (ec) {
        stop();
        return;
    }

    std::stringstream message;
    message << std::istream(&_streambuf).rdbuf();
    on_message(message.str());
    async_read();
}

void TCPClient::async_write() {
    boost::asio::async_write(_socket, boost::asio::buffer(_outgoing_messages.front()), [this](boost::system::error_code ec, size_t bytes_transferred) {
        on_write(ec, bytes_transferred);
    });
}

void TCPClient::on_write(boost::system::error_code ec, size_t bytes_transferred) {
    if (ec) {
        stop();
        return;
    }

    _outgoing_messages.pop();

    if (!_outgoing_messages.empty()) {
        async_write();
    }
}
