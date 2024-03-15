#include "include/communication.hpp"

#include "hardware/gpio.h"
#include "include/command.hpp"
#include "include/thread_safe_fifo.hpp"
#include "pico/stdlib.h"

// Check if lumminair id matches
#define CHECK_ID(x)           \
    if (std::stoi(x) != id) { \
        SEND_ERR();           \
        return;               \
    }

extern ThreadSafeFifo<Command> fifo0;
extern uint8_t id;

CAN::CAN(uint8_t interrupt_pin)
    : interrupt_pin(interrupt_pin), can0(spi0, 17, 19, 16, 18, 10'000'000) {}

inline std::string error_to_str(MCP2515::ERROR err) {
    switch (err) {
        case MCP2515::ERROR::ERROR_FAIL:
            return "ERROR_FAIL";
        case MCP2515::ERROR::ERROR_ALLTXBUSY:
            return "ERROR_ALLTXBUSY";
        case MCP2515::ERROR::ERROR_FAILINIT:
            return "ERROR_FAILINIT";
        case MCP2515::ERROR::ERROR_FAILTX:
            return "ERROR_FAILTX";
        case MCP2515::ERROR::ERROR_NOMSG:
            return "ERROR_NOMSG";
    }

    return "UNKNOWN ERROR";
}

void CAN::setup() {
    can0.reset();
    can0.setBitrate(CAN_1000KBPS);
    can0.setNormalMode();
}

void CAN::send_msg(can_frame &msg) {
    LOGGER_SEND_INFO("Sending CAN message");

    auto err = can0.sendMessage(&msg);

    if (err) {
        std::string e_msg = "CAN -> " + error_to_str(err);
        LOGGER_SEND_ERROR(e_msg.c_str());
    }
}

bool CAN::receive_msg(can_frame &msg) {
    auto err = can0.readMessage(&msg);

    if (err == MCP2515::ERROR::ERROR_OK) {  // message received
        return true;
    } else if (err == MCP2515::ERROR::ERROR_NOMSG) {  // no message
        return false;
    }

    // CAN error
    std::string e_msg = "CAN -> " + error_to_str(err);
    LOGGER_SEND_ERROR(e_msg.c_str());

    return false;
}

namespace USB {
void process_input(std::vector<std::string> &args) {
    if (args.size() != 3 && args.size() != 4) {
        std::string msg = "Invalid message size " + std::to_string(args.size());
        LOGGER_SEND_ERROR(msg.c_str());
        return;
    }

    if (args[0].size() != 1) {
        LOGGER_SEND_ERROR("Invalid cmd arg[0]");
        return;
    }

    Command cmd;

    switch (args[0][0]) {
        case 'P':  // Set Kp
            CHECK_ID(args[1]);

            cmd = LuminaireCmd{
                .request = Request::SET, .target = Target::KP, .value = std::stof(args[2])};
            break;

        case 'I':  // Set Ki
            CHECK_ID(args[1]);

            cmd = LuminaireCmd{
                .request = Request::SET, .target = Target::KI, .value = std::stof(args[2])};
            break;

        case 'D':  // Set Kd
            CHECK_ID(args[1]);

            cmd = LuminaireCmd{
                .request = Request::SET, .target = Target::KD, .value = std::stof(args[2])};
            break;

        case 'B':  // Set b
            CHECK_ID(args[1]);

            cmd = LuminaireCmd{
                .request = Request::SET, .target = Target::B, .value = std::stof(args[2])};
            break;

        case 'C':  // Set c
            CHECK_ID(args[1]);

            cmd = LuminaireCmd{
                .request = Request::SET, .target = Target::C, .value = std::stof(args[2])};
            break;

        case 'd':  // Set Lumminair duty cycle
            CHECK_ID(args[1]);

            cmd = LuminaireCmd{
                .request = Request::SET, .target = Target::DC, .value = std::stof(args[2])};
            break;

        case 'g':  // Getters
            CHECK_ID(args[2]);

            switch (args[1][0]) {
                case 'P':  // Kp
                    CHECK_ID(args[2]);

                    cmd = LuminaireCmd{.request = Request::GET, .target = Target::KP, .value = 0};
                    break;

                case 'I':  // Ki
                    CHECK_ID(args[2]);

                    cmd = LuminaireCmd{.request = Request::GET, .target = Target::KI, .value = 0};
                    break;

                case 'D':  // Kd
                    CHECK_ID(args[2]);

                    cmd = LuminaireCmd{.request = Request::GET, .target = Target::KD, .value = 0};
                    break;

                case 'B':  // b
                    CHECK_ID(args[2]);

                    cmd = LuminaireCmd{.request = Request::GET, .target = Target::B, .value = 0};
                    break;

                case 'C':  // c
                    CHECK_ID(args[2]);

                    cmd = LuminaireCmd{.request = Request::GET, .target = Target::C, .value = 0};
                    break;

                case 'd':  // Duty cycle
                    CHECK_ID(args[2]);

                    cmd = LuminaireCmd{.request = Request::GET, .target = Target::DC, .value = 0};
                    break;

                case 'r':  // Reference
                    CHECK_ID(args[2]);

                    cmd = LuminaireCmd{.request = Request::GET, .target = Target::REF, .value = 0};
                    break;

                case 'l':  // Luminosity
                    CHECK_ID(args[2]);

                    cmd = LuminaireCmd{.request = Request::GET, .target = Target::LUX, .value = 0};
                    break;

                case 'o':  // Occupancy
                    CHECK_ID(args[2]);

                    cmd = LuminaireCmd{
                        .request = Request::GET, .target = Target::OCCUPANCY, .value = 0};
                    break;

                case 'a':  // Anti-windup
                    CHECK_ID(args[2]);

                    cmd = LuminaireCmd{
                        .request = Request::GET, .target = Target::ANTI_WINDUP, .value = 0};
                    break;

                case 'k':  // Feedback
                    CHECK_ID(args[2]);

                    cmd = LuminaireCmd{
                        .request = Request::GET, .target = Target::FEEDBACK, .value = 0};
                    break;

                case 'x':  // Instantaneous luminosity
                    CHECK_ID(args[2]);

                    cmd = LuminaireCmd{
                        .request = Request::GET, .target = Target::EXT_LUX, .value = 0};
                    break;

                case 'p':  // Instantaneous power consumption
                    CHECK_ID(args[2]);

                    cmd = MonitorCmd{.monitor = Monitor::INST_POWER_COMSUMPTION};
                    break;

                case 't':  // Time since restart
                    CHECK_ID(args[2]);

                    cmd = MonitorCmd{.monitor = Monitor::TIME_SINSE_RESTART, .variable = 0};
                    break;

                case 'b':  // Buffer
                    CHECK_ID(args[2]);

                    cmd = MonitorCmd{.monitor = Monitor::BUFFER, .variable = 0};
                    break;

                case 'e':  // Avg power consumption
                    CHECK_ID(args[2]);

                    cmd = MonitorCmd{.monitor = Monitor::AVG_POWER_CONSUMPTION, .variable = 0};
                    break;

                case 'v':  // Avg visibility error
                    CHECK_ID(args[2]);

                    cmd = MonitorCmd{.monitor = Monitor::AVG_VISIBILITY_ERROR, .variable = 0};
                    break;

                case 'f':  // Avg flicker error
                    CHECK_ID(args[2]);

                    cmd = MonitorCmd{.monitor = Monitor::AVG_FLICKER_ERROR, .variable = 0};
                    break;

                default:
                    SEND_ERR();
                    LOGGER_SEND_ERROR("Invalid cmd arg[1]");
                    return;
            }
            break;

        case 'r':  // Set reference
            CHECK_ID(args[1]);

            cmd = LuminaireCmd{
                .request = Request::SET, .target = Target::REF, .value = std::stof(args[2])};
            break;

        case 'o':  // Set occupancy
            CHECK_ID(args[1]);

            cmd = LuminaireCmd{
                .request = Request::SET, .target = Target::OCCUPANCY, .value = std::stof(args[2])};
            break;

        case 'a':  // Set anti-windup
            CHECK_ID(args[1]);

            cmd = LuminaireCmd{.request = Request::SET,
                               .target = Target::ANTI_WINDUP,
                               .value = std::stof(args[2])};
            break;

        case 'k':  // Set feedback
            CHECK_ID(args[1]);

            cmd = LuminaireCmd{
                .request = Request::SET, .target = Target::FEEDBACK, .value = std::stof(args[2])};
            break;

        case 's':  // Start stream
            CHECK_ID(args[2]);

            cmd = MonitorCmd{.monitor = Monitor::START_STREAM, .variable = args[1][0]};
            break;

        case 'S':  // Stop stream
            CHECK_ID(args[2]);

            cmd = MonitorCmd{.monitor = Monitor::STOP_STREAM, .variable = args[1][0]};
            break;

        default:
            SEND_ERR();
            LOGGER_SEND_ERROR("Invalid cmd arg[0]");
            return;
    }

    LOGGER_SEND_INFO("Pushing cmd to fifo0");
    fifo0.push(cmd);
}

void handle() {
    if (Serial.available() > 0) {
        std::string msg = Serial.readString().c_str();

        // Auxiliar varible to split string into words
        std::istringstream iss(msg);
        std::vector<std::string> args;
        std::string word;

        // Split the message into words
        while (iss >> word) {
            args.push_back(word);
        }

        std::string info = "recived msg -> " + msg;
        LOGGER_SEND_INFO(info.c_str());

        process_input(args);
    }
}
}  // namespace USB