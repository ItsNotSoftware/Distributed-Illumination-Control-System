#include <array>
#include <boost/asio.hpp>
#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

constexpr unsigned short PORT = 8080;
using boost::asio::ip::udp, std::cout, std::endl;

class UdpServer {
   public:
    UdpServer(boost::asio::io_context& io_context)
        : socket_(io_context, udp::endpoint(udp::v4(), PORT)) {
        cout << "Server started at port " << PORT << endl;
        start_receive();
    }

   private:
    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    std::array<char, 1024> rcv_buffer_;

    void start_receive() {
        socket_.async_receive_from(
            boost::asio::buffer(rcv_buffer_), remote_endpoint_,
            std::bind(&UdpServer::receive_handler, this, boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred));
    }

    void receive_handler(const boost::system::error_code& error, std::size_t bytes_transferred) {
        if (!error) {
            std::string received_message(rcv_buffer_.data(), bytes_transferred);
            cout << "Received message: " << received_message << endl;

            start_receive();
        }
    }

    void send_handler(std::shared_ptr<std::string> /*message*/,
                      const boost::system::error_code& /*error*/,
                      std::size_t /*bytes_transferred*/) {}
};

int main() {
    try {
        boost::asio::io_context io_context;
        UdpServer server(io_context);
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << endl;
    }

    return 0;
}
