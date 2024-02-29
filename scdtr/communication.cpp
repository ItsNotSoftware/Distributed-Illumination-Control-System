#include "communication.hpp"

#include "command.hpp"
#include "command_fifo.hpp"

// Check if lumminair id matches
#define CHECK_ID(x) \
    if (std::stoi(x) != id) return

extern CommandFifo fifo0;
extern uint8_t id;

void process_input(std::vector<std::string> &args) {
    if (args.size() != 3) {
        LOGGER_SEND_ERROR("Invalid message");
        return;
    }

    if (args[0].size() != 1) {
        LOGGER_SEND_ERROR("Invalid cmd arg[0]");
        return;
    }

    Command cmd;

    switch (args[0][0]) {
        case 'd':  // Set Lumminair duty cycle
            CHECK_ID(args[1]);

            cmd = LuminaireCmd{
                .request = Request::SET_VAL, .unit = Unit::DC, .value = std::stof(args[2])};
            break;

        case 'g':  // Getters
            CHECK_ID(args[2]);

            switch (args[1][0]) {
                case 'd':  // Duty cycle
                    cmd = LuminaireCmd{.request = Request::GET_VAL, .unit = Unit::DC, .value = 0};
                    break;

                case 'r':  // Reference
                    cmd = LuminaireCmd{.request = Request::GET_REF, .unit = Unit::LUX, .value = 0};
                    break;

                case 'l':  // Luminosity
                    cmd = LuminaireCmd{.request = Request::GET_VAL, .unit = Unit::LUX, .value = 0};
                    break;

                case 'o':  // Occupancy
                    cmd = ContollerCmd{
                        .request = Request::GET_VAL, .target = Target::OCCUPANCY, .value = 0};
                    break;

                case 'a':  // Anti-windup
                    cmd = ContollerCmd{
                        .request = Request::GET_VAL, .target = Target::ANTI_WINDUP, .value = 0};
                    break;

                case 'k':  // Feedback
                    cmd = ContollerCmd{
                        .request = Request::GET_VAL, .target = Target::FEEDBACK, .value = 0};
                    break;

                case 'x':
                    cmd = LuminaireCmd{.request = Request::GET_VAL, .unit = Unit::LUX, .value = 0};
                    break;

                default:
                    LOGGER_SEND_ERROR("Invalid cmd arg[1]");
                    return;
            }
            break;

        case 'r':  // Set reference
            CHECK_ID(args[1]);

            cmd = LuminaireCmd{
                .request = Request::SET_REF, .unit = Unit::LUX, .value = std::stof(args[2])};
            break;

        case 'o':  // Set occupancy
            CHECK_ID(args[1]);

            cmd = ContollerCmd{.request = Request::SET_VAL,
                               .target = Target::OCCUPANCY,
                               .value = std::stof(args[2])};
            break;

        case 'a':  // Set anti-windup
            CHECK_ID(args[1]);

            cmd = ContollerCmd{.request = Request::SET_VAL,
                               .target = Target::ANTI_WINDUP,
                               .value = std::stof(args[2])};
            break;

        case 'k':
            CHECK_ID(args[1]);

            cmd = ContollerCmd{.request = Request::SET_VAL,
                               .target = Target::FEEDBACK,
                               .value = std::stof(args[2])};
            break;

        case 's':
            break;

        case 'S':
            break;

        default:
            LOGGER_SEND_ERROR("Invalid cmd arg[0]");
            return;

            fifo0.push(cmd);
    }
}

namespace SerialCom {
void read() {
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
}  // namespace SerialCom
