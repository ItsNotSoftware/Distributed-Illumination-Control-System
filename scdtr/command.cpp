#include "command.hpp"

#include "communication.hpp"
#include "controller.hpp"
#include "led.hpp"
#include "luxmeter.hpp"

// lumminair
extern Luxmeter luxmeter;
extern LED led;
extern Controller controller;
extern uint8_t id;
extern bool contoller_active;

static std::string id_str = std::to_string(id) + " ";

// controller

void luminair_setters(Target target, float value) {
    switch (target) {
        case Target::DC:
            led.set_duty_cycle(value);
            break;

        case Target::REF:
            controller.set_target(value);
            break;

        case Target::OCCUPANCY:
            contoller_active = value == 1;
            break;

        case Target::ANTI_WINDUP:
            controller.set_anti_windup_state(value == 1);
            break;

        case Target::FEEDBACK:
            controller.set_feedback(value == 1);
            break;

        default:
            SEND_ERR();
            LOGGER_SEND_ERROR("Invalid target");
            return;
    }

    SEND_ACK();
}

void luminair_getters(Target target) {
    std::string response;

    switch (target) {
        case Target::DC:
            response = "d " + id_str + std::to_string(led.get_duty_cycle());
            break;

        case Target::REF:
            response = "r " + id_str + std::to_string(controller.get_target());
            break;

        case Target::LUX:
            response = "l " + id_str + std::to_string(luxmeter.get_lux());
            break;

        case Target::OCCUPANCY:
            response = "o " + id_str + std::to_string(contoller_active);
            break;

        case Target::ANTI_WINDUP:
            response = "a " + id_str + std::to_string(controller.get_anti_windup_state() ? 1 : 0);
            break;

        case Target::FEEDBACK:
            response = "k " + id_str + std::to_string(controller.get_feedback() ? 1 : 0);
            break;

        default:
            LOGGER_SEND_ERROR("Invalid target");
            return;
    }

    Serial.println(response.c_str());
}

void process_luminair_cmd(LuminaireCmd &cmd) {
    std::string response;

    switch (cmd.request) {
        case Request::SET:
            luminair_setters(cmd.target, cmd.value);
            break;

        case Request::GET:
            luminair_getters(cmd.target);
            break;

        default:
            SEND_ERR();
            LOGGER_SEND_ERROR("Invalid request");
            break;
    }
}

void command_handle(Command &cmd) {
    if (std::holds_alternative<LuminaireCmd>(cmd)) {
        LOGGER_SEND_INFO("Luminaire command received");

        LuminaireCmd lum_cmd = std::get<LuminaireCmd>(cmd);
        process_luminair_cmd(lum_cmd);

    } else if (std::holds_alternative<MonitorCmd>(cmd)) {
        LOGGER_SEND_INFO("Monitor command received");
    } else {
        LOGGER_SEND_ERROR("Unknown command received");
    }
}