#include "command.hpp"

#include <numeric>

#include "communication.hpp"
#include "controller.hpp"
#include "led.hpp"
#include "luxmeter.hpp"
#include "ring_buffer.hpp"

// lumminair
extern Luxmeter luxmeter;
extern LED led;
extern Controller controller;
extern uint8_t id;
extern bool contoller_active;
extern RingBuffer<float, 100> lux_buffer;
extern RingBuffer<float, 100> dutycycle_buffer;
extern uint32_t curr_time;

static std::string id_str = std::to_string(id) + " ";

// controller

inline void luminair_setters(Target target, float value) {
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

inline void luminair_getters(Target target) {
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

        case Target::EXT_LUX:
            led.set_pwm_range(0);     // Turn off the led
            delay(100);               // Wait for the led to turn off
            luxmeter.sample(16, 20);  // 16 samples, 20ms delay

            response = "l " + id_str + std::to_string(luxmeter.get_lux());
            break;

        default:
            LOGGER_SEND_ERROR("Invalid target");
            return;
    }

    Serial.println(response.c_str());
}

inline void process_luminair_cmd(LuminaireCmd &cmd) {
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

inline void process_monitor_cmf(MonitorCmd &cmd) {
    std::string response;
    switch (cmd.monitor) {
        case Monitor::INST_POWER_COMSUMPTION:
            response = "p " + id_str + std::to_string(led.get_duty_cycle()) + " x Pmax";
            break;

        case Monitor::AVG_POWER_CONSUMPTION:
            float sum =
                std::accumulate(dutycycle_buffer.buffer.begin(), dutycycle_buffer.buffer.end(), 0);
            float avg = sum / dutycycle_buffer.buffer.size();

            response = "e " + id_str + std::to_string(avg) + " x Pmax";
            break;

        case Monitor::TIME_SINSE_RESTART:
            response = "t " + id_str + std::to_string(curr_time) + "ms";
            break;

        case Monitor::START_STREAM:
            if (cmd.variable == 'l') {
                stream_lux = true;
            } else if (cmd.variable == 'd') {
                stream_dutycycle = true;
            }
            break;

        case Monitor::STOP_STREAM:
            if (cmd.variable == 'l') {
                stream_lux = false;
            } else if (cmd.variable == 'd') {
                stream_dutycycle = false;
            }
            break;

        case Monitor::BUFFER:
            if (cmd.variable == 'l') {
                response = "b l " + id_str + lux_buffer.to_str();
                Serial.println(response.c_str());
            } else if (cmd.variable == 'd') {
                response = "b d " + id_str + dutycycle_buffer.to_str();
                Serial.println(response.c_str());
            }

        default:
            SEND_ERR();
            LOGGER_SEND_ERROR("Invalid monitor");
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