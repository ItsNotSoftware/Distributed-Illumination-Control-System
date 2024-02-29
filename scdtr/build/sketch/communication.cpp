#line 1 "/Users/diogoferreira/CodeWs/Distributed-Illumination-Control-System/scdtr/communication.cpp"
#include "communication.hpp"

#include "command.hpp"
#include "command_fifo.hpp"

#define CHECK_ID(x) \
    if (std::stoi(x) != id) return;

extern CommandFifo fifo;
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

    switch (args[0][0]) {
        case 'd':
            CHECK_ID(args[1]);
            // LuminaireCmd x = {
            //     .request = Request::SET_VAL, .unit = Unit::DC, .value = std::stof(args[2])};
            // Command cmd = x;

            // fifo.push(cmd);
            break;
        case 'g':
            break;
        case 'r':
            break;
        case 'o':
            break;
        case 'a':
            break;
        case 's':
            break;
        case 'S':
            break;
        default:
            LOGGER_SEND_ERROR("Invalid cmd arg[0]");
            break;
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
