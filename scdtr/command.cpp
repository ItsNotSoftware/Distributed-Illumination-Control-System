#include "command.hpp"

#include "communication.hpp"
#include "led.hpp"
#include "luxmeter.hpp"

extern Luxmeter luxmeter;
extern LED led;

void process_luminair_cmd(LuminaireCmd &cmd) {
    std::string response;

    switch (cmd.request) {
        case Request::SET_VAL:
            response = "Setting duty cycle to " + std::to_string(cmd.value);
            LOGGER_SEND_INFO(response.c_str());

            led.set_duty_cycle(cmd.value);
            break;

        case Request::GET_VAL:
            // TODO
            break;

        case Request::GET_REF:
            // TODO
            break;

        default:
            LOGGER_SEND_ERROR("Invalid request");
            break;
    }
}

void command_handle(Command &cmd) {
    if (std::holds_alternative<LuminaireCmd>(cmd)) {
        LOGGER_SEND_INFO("Luminaire command received");

        LuminaireCmd lum_cmd = std::get<LuminaireCmd>(cmd);
        process_luminair_cmd(lum_cmd);

    } else if (std::holds_alternative<ContollerCmd>(cmd)) {
        LOGGER_SEND_INFO("Controller command received");
    } else if (std::holds_alternative<MonitorCmd>(cmd)) {
        LOGGER_SEND_INFO("Monitor command received");
    } else {
        LOGGER_SEND_ERROR("Unknown command received");
    }
}