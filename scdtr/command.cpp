#include "command.hpp"

#include "led.hpp"
#include "luxmeter.hpp"

extern Luxmeter luxmeter;
extern LED led;

void command_handle(Command &cmd) {
    if (std::holds_alternative<LuminaireCmd>) {
        LOGGER_SEND_INFO("Luminaire command received");
    } else if (std::holds_alternative<ContollerCmd>) {
        LOGGER_SEND_INFO("Controller command received");
    } else {
        LOGGER_SEND_ERROR("Unknown command received");
    }
}