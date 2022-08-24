#ifndef BASIC_SERVER_CLIENT_TCP_CONNECTION_H
#define BASIC_SERVER_CLIENT_TCP_CONNECTION_H

#include <memory>
#include <queue>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

using messageHandler = std::function<void(std::string)>;
using orderHandler = std::function<void(std::string)>;
using errorHandler = std::function<void()>;

class TCPConnection : public std::enable_shared_from_this<TCPConnection> {
public:
    using pointer = std::shared_ptr<TCPConnection>;
    static pointer create(boost::asio::ip::tcp::socket&& socket) {
        return pointer(new TCPConnection(std::move(socket)));
    }

    inline const std::string& get_username() const {
         return _username;
    }

    tcp::socket& socket() {
        return _socket;
    }

    void start(messageHandler&& message_handler, orderHandler&& order_handler, errorHandler&& error_handler);
    // posts message to client
    void post(const std::string& message);

private:
    explicit TCPConnection(boost::asio::ip::tcp::socket&& socket);

    // wait for a new message from client
    void async_read();
    void on_read(boost::system::error_code ec, size_t bytes_transferred);

    void async_write();
    void on_write(boost::system::error_code ec, size_t bytes_transferred);

private:
    tcp::socket _socket;
    std::string _username;

    std::queue<std::string> _outgoing_messages;

    boost::asio::streambuf _streambuf { 65536 };

    messageHandler _message_handler;
    orderHandler _order_handler;
    errorHandler _error_handler;

};


#endif //BASIC_SERVER_CLIENT_TCP_CONNECTION_H
