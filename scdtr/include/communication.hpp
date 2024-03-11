#ifndef COMMUNICATION_HPP
#define COMMUNICATION_HPP

#include <Arduino.h>

#include <sstream>
#include <string>
#include <vector>

// custom messages for a serial monitor LOGGER
#define LOGGER_SEND_VAL(id, val) Serial.println("[VALUE]: " + String(id) + " " + String(val))
#define LOGGER_SEND_INFO(info) Serial.println("[INFO]: " + String(info))
#define LOGGER_SEND_ERROR(error) Serial.println("[ERROR]: " + String(error))
#define LOGGER_SEND_WARNING(warning) Serial.println("[WARNING]: " + String(warning))
#define LOGGER_SEND_CONTROLLER_DATA(t, u, y1, y2)                                            \
    Serial.println("[CONTROLLER]: " + String(t) + " " + String(u) + " " + String(y1) + " " + \
                   String(y2))

#define SEND_ACK() Serial.println("ack")
#define SEND_ERR() Serial.println("err")

namespace SerialCom {
void read();
}

#endif