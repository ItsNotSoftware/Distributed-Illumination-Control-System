#include <array>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

// Set to false to disable console echo
constexpr bool ECHO_SERIAL = true;
constexpr bool ECHO_UDP = true;

constexpr unsigned short PORT = 8080;

using boost::asio::ip::udp, std::cout, std::endl;

/* UDP <-> Serial bridge */
class UdpSerialServer {
   public:
    /*
     * Constructor to start the server and bind to the given serial port
     *
     * @param io_context: The boost::asio::io_context to use
     * @param serial_port: The serial port to bind to
     */
    UdpSerialServer(boost::asio::io_context& io_context, const std::string& serial_port)
        : io_context_(io_context),
          socket_(io_context, udp::endpoint(udp::v4(), PORT)),
          serial_(io_context, serial_port) {
        cout << "Server started at port " << PORT << endl;
        setup_serial();
        udp_start_receive();
        serial_start_receive();
    }

   private:
    boost::asio::io_context& io_context_;
    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    boost::asio::serial_port serial_;

    // Buffers for receiving data
    std::array<char, 1024> sock_rcv_buffer_;
    std::array<char, 1024> serial_rcv_buffer_;

    /*
     * Setup the serial port with the following settings:
     * - Baud rate: 9600
     * - Character size: 8
     * - Parity: None
     * - Stop bits: 1
     */
    void setup_serial() {
        serial_.set_option(boost::asio::serial_port_base::baud_rate(9600));
        serial_.set_option(boost::asio::serial_port_base::character_size(8));
        serial_.set_option(
            boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
        serial_.set_option(boost::asio::serial_port_base::stop_bits(
            boost::asio::serial_port_base::stop_bits::one));
    }

    /*
     * Start receiving UDP messages
     */
    void udp_start_receive() {
        socket_.async_receive_from(
            boost::asio::buffer(sock_rcv_buffer_), remote_endpoint_,
            std::bind(&UdpSerialServer::udp_receive_handler, this, boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred));
    }

    /*
     * Handle received UDP messages
     *
     * @param error: The error code
     * @param bytes_transferred: The number of bytes transferred
     */
    void udp_receive_handler(const boost::system::error_code& error,
                             std::size_t bytes_transferred) {
        if (!error) {
            std::string received_message(sock_rcv_buffer_.data(), bytes_transferred);

            if constexpr (ECHO_UDP) cout << "[UDP]: " << received_message << endl;

            // Send received UDP message to serial
            serial_send(received_message);

            udp_start_receive();
        }
    }

    /*
     * Start receiving serial messages
     */
    void serial_start_receive() {
        serial_.async_read_some(boost::asio::buffer(serial_rcv_buffer_),
                                std::bind(&UdpSerialServer::serial_receive_handler, this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
    }

    /*
     * Handle received serial messages
     *
     * @param error: The error code
     * @param bytes_transferred: The number of bytes transferred
     */
    void serial_receive_handler(const boost::system::error_code& error,
                                std::size_t bytes_transferred) {
        if (!error) {
            std::string received_message(serial_rcv_buffer_.data(), bytes_transferred);

            if constexpr (ECHO_SERIAL) cout << "[SERIAL]: " << received_message << endl;

            // Send received serial message over UDP
            udp_send(received_message);

            serial_start_receive();
        }
    }

    /*
     * Send a message over the serial port
     *
     * @param message: The message to send
     */
    void serial_send(const std::string& message) {
        boost::asio::async_write(serial_, boost::asio::buffer(message),
                                 [](const boost::system::error_code&, std::size_t) {});
    }

    /*
     * Send a message over UDP
     *
     * @param message: The message to send
     */
    void udp_send(const std::string& message) {
        auto message_to_send = std::make_shared<std::string>(message);

        socket_.async_send_to(boost::asio::buffer(*message_to_send), remote_endpoint_,
                              [this, message_to_send](const boost::system::error_code& error,
                                                      std::size_t bytes_transferred) {
                                  this->udp_send_handler(message_to_send, error, bytes_transferred);
                              });
    }

    /*
     * Handle the result of sending a message over UDP
     *
     * @param message: The message to send
     * @param error: The error code
     * @param bytes_transferred: The number of bytes transferred
     */
    void udp_send_handler(std::shared_ptr<std::string> /*message*/,
                          const boost::system::error_code& /*error*/,
                          std::size_t /*bytes_transferred*/) {}
};

int main(int argc, char* argv[]) {
    // Check for correct number of arguments
    if (argc != 2) {
        std::cerr << "Usage: server <serial_port>" << endl;
        return 1;
    }

    try {
        boost::asio::io_context io_context;
        UdpSerialServer server(io_context, argv[1]);
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << endl;
    }

    return 0;
}
