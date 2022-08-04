#include <iostream>
#include <thread>

#include <networking/client/tcp_client.h>


int main(int argc, char** argv) {
//
//    try {
//        boost::asio::io_context io_context;
//
//        tcp::resolver resolver { io_context };
//
//        auto endpoints = resolver.resolve("127.0.0.1", "1337");
//
//        tcp::socket socket { io_context };
//        boost::asio::connect(socket, endpoints);
//
//        while(true) {
//            // listen for messages
//            std::array<char, 128> buffer {};
//            boost::system::error_code error;
//
//            size_t len = socket.read_some(boost::asio::buffer(buffer), error);
//
//            if (error == boost::asio::error::eof) {
//                // clean connection cut off
//                break;
//            } else if (error) {
//                throw boost::system::system_error(error);
//            }
//
//            std::cout.write(buffer.data(), len);
//        }
//
//    } catch(std::exception& e) {
//        std::cerr << e.what() << std::endl;
//    }

    TCPClient client("localhost", 1337);

    client.on_message = [](const std::string& message) {
        std::cout << message;
    };

    std::thread t([&client] () {
        client.run();
    });

    while(true) {
        std::string message;
        getline(std::cin, message);

        if (message == "\\q") {
            break;
        }

        message += "\n";

        client.post(message);
    }

    client.stop();
    t.join();

    return 0;
}
