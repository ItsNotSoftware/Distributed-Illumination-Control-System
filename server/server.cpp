#include <array>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

// Set to false to disable console echo
constexpr bool ECHO_SERIAL = true;
constexpr bool ECHO_TCP = true;

constexpr unsigned short PORT = 8080;

using boost::asio::ip::tcp, std::cout, std::endl;

/* TCP <-> Serial bridge */
class TcpSerialServer {
   public:
    TcpSerialServer(boost::asio::io_context& io_context, const std::string& serial_port)
        : io_context_(io_context),
          acceptor_(io_context, tcp::endpoint(tcp::v4(), PORT)),
          socket_(io_context),
          serial_(io_context, serial_port) {
        cout << "Server started at port " << PORT << endl;
        setup_serial();
        start_accept();
        serial_start_receive();
    }

   private:
    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    boost::asio::serial_port serial_;
    std::array<char, 1024> tcp_rcv_buffer_;
    std::array<char, 1024> serial_rcv_buffer_;

    void setup_serial() {
        serial_.set_option(boost::asio::serial_port_base::baud_rate(9600));
        serial_.set_option(boost::asio::serial_port_base::character_size(8));
        serial_.set_option(
            boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
        serial_.set_option(boost::asio::serial_port_base::stop_bits(
            boost::asio::serial_port_base::stop_bits::one));
    }

    void start_accept() {
        acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
            if (!ec) {
                tcp_start_receive();
            }
            start_accept();  // Listen for new connections
        });
    }

    void tcp_start_receive() {
        socket_.async_read_some(
            boost::asio::buffer(tcp_rcv_buffer_),
            [this](boost::system::error_code ec, std::size_t bytes_transferred) {
                if (!ec) {
                    std::string received_message(tcp_rcv_buffer_.data(), bytes_transferred);
                    if constexpr (ECHO_TCP) cout << "[TCP]: " << received_message << endl;
                    serial_send(received_message);
                    tcp_start_receive();
                }
            });
    }

    void serial_start_receive() {
        serial_.async_read_some(
            boost::asio::buffer(serial_rcv_buffer_),
            [this](boost::system::error_code ec, std::size_t bytes_transferred) {
                if (!ec) {
                    std::string received_message(serial_rcv_buffer_.data(), bytes_transferred);
                    if constexpr (ECHO_SERIAL) cout << "[SERIAL]: " << received_message << endl;
                    tcp_send(received_message);
                    serial_start_receive();
                }
            });
    }

    void serial_send(const std::string& message) {
        boost::asio::async_write(serial_, boost::asio::buffer(message),
                                 [](boost::system::error_code, std::size_t) {});
    }

    void tcp_send(const std::string& message) {
        boost::asio::async_write(socket_, boost::asio::buffer(message),
                                 [](boost::system::error_code, std::size_t) {});
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: server <serial_port>" << endl;
        return 1;
    }

    try {
        boost::asio::io_context io_context;
        TcpSerialServer server(io_context, argv[1]);
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
