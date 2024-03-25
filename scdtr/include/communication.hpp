#ifndef COMMUNICATION_HPP
#define COMMUNICATION_HPP

#include <Arduino.h>

#include <sstream>
#include <string>
#include <vector>

#include "mcp2515.hpp"

// Custom messages for a serial monitor LOGGER
#define LOGGER_SEND_VAL(id, val) Serial.println("[VALUE]: " + String(id) + " " + String(val))
#define LOGGER_SEND_INFO(info) Serial.println("[INFO]: " + String(info))
#define LOGGER_SEND_ERROR(error) Serial.println("[ERROR]: " + String(error))
#define LOGGER_SEND_WARNING(warning) Serial.println("[WARNING]: " + String(warning))
#define LOGGER_SEND_CONTROLLER_DATA(t, u, y1, y2)                                            \
    std::string msg = "[CONTROLLER]: " + std::to_string(t) + ' ' + std::to_string(u) + ' ' + \
                      std::to_string(y1) + ' ' + std::to_string(y2);                         \
    Serial.println(msg.c_str())

#define SEND_ACK() Serial.println("ack")
#define SEND_ERR() Serial.println("err")

class CAN {
   public:
    CAN(uint8_t interrupt_pin);

    /*
     * Initialize can module
     */
    void setup();

    /*
     * Send a can message
     *
     * @param can_frame
     */
    void send_msg(can_frame &msg);

    /*
     * Recive a can message
     *
     * @param revference to can_frame to fill with new message
     *
     * @return true if a message was recived
     */
    bool receive_msg(can_frame &msg);

   private:
    uint8_t interrupt_pin;
    MCP2515 can0;
};

namespace USB {

/*
 * Handle Serial input
 */
void handle();
}  // namespace USB

#endif