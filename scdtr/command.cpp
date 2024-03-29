#include "include/command.hpp"

#include <numeric>

#include "include/communication.hpp"
#include "include/controller.hpp"
#include "include/led.hpp"
#include "include/luxmeter.hpp"
#include "include/ring_buffer.hpp"

// Global variables
extern Luxmeter luxmeter;
extern LED led;
extern Controller controller;
extern uint8_t id;
extern bool contoller_active;
extern bool stream_lux;
extern bool stream_dutycycle;
extern RingBuffer<float, 100> lux_buffer;
extern RingBuffer<float, 100> dutycycle_buffer;
extern uint32_t curr_time;

static std::string id_str = std::to_string(id) + " ";  // ID string

/*
 * Setters for the luminair
 *
 * @param target The target to be set
 * @param value The value to be set
 */
inline void luminair_setters(Target target, float value) {
    switch (target) {
        case Target::KD:
            controller.set_Kd(value);
            break;

        case Target::KP:
            controller.set_Kp(value);
            break;

        case Target::KI:
            controller.set_Ki(value);
            break;

        case Target::B:
            controller.set_b(value);
            break;

        case Target::DC:
            led.set_duty_cycle(value);
            break;

        case Target::REF:
            controller.set_target(luxmeter.lux_to_mv(value));
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

/*
 * Getters for the luminair
 *
 * @param target The target to be set
 */
inline void luminair_getters(Target target) {
    std::string response;

    switch (target) {
        case Target::KD:
            response = "d " + id_str + std::to_string(controller.get_Kd());
            break;

        case Target::KP:
            response = "p " + id_str + std::to_string(controller.get_Kp());
            break;

        case Target::KI:
            response = "i " + id_str + std::to_string(controller.get_Ki());
            break;

        case Target::B:
            response = "b " + id_str + std::to_string(controller.get_b());
            break;

        case Target::C:
            response = "c " + id_str + std::to_string(controller.get_c());
            break;

        case Target::DC:
            response = "d " + id_str + std::to_string(led.get_duty_cycle());
            break;

        case Target::REF:
            response = "r " + id_str + std::to_string(luxmeter.mv_to_lux(controller.get_target()));
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
            led.set_pwm_range(0);  // Turn off the led

            // Turn off the led and sampel ADC
            for (int i = 0; i < N_AVG_POINTS; i++) {
                delay(10);
                luxmeter.sample();
                watchdog_update();
            }

            response = "l " + id_str + std::to_string(luxmeter.get_lux());
            break;

        default:
            LOGGER_SEND_ERROR("Invalid target");
            return;
    }

    Serial.println(response.c_str());
}

/*
 * Process the luminair command
 *
 * @param cmd The command to be processed
 */
inline void process_luminair_cmd(LuminaireCmd& cmd) {
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

void process_monitor_cmd(MonitorCmd& cmd) {
    std::string response;
    float flicker_error, visibility_error, sum, avg, duty_cycle;

    switch (cmd.monitor) {
        case Monitor::INST_POWER_COMSUMPTION:
            duty_cycle = led.get_duty_cycle();
            response = "p " + id_str + std::to_string(duty_cycle) + " x Pmax";
            Serial.println(response.c_str());
            break;

        case Monitor::AVG_POWER_CONSUMPTION:
            sum =
                std::accumulate(dutycycle_buffer.buffer.begin(), dutycycle_buffer.buffer.end(), 0);
            avg = sum / dutycycle_buffer.buffer.size();

            response = "e " + id_str + std::to_string(avg) + " x Pmax";
            Serial.println(response.c_str());
            break;

        case Monitor::TIME_SINSE_RESTART:
            response = "t " + id_str + std::to_string(millis() / 1000);
            Serial.println(response.c_str());
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
            break;

        case Monitor::AVG_VISIBILITY_ERROR:
            visibility_error = 0;

            for (int i = 0; i < lux_buffer.buffer.size() - 1; i++) {
                visibility_error += std::abs(lux_buffer.buffer[i + 1] - controller.get_target());
            }
            response = "v " + id_str + std::to_string(visibility_error / lux_buffer.buffer.size());
            Serial.println(response.c_str());

            break;

        case Monitor::AVG_FLICKER_ERROR:
            flicker_error = 0;

            for (int i = 0; i < dutycycle_buffer.buffer.size() - 1; i++) {
                flicker_error +=
                    std::abs(dutycycle_buffer.buffer[i + 1] - dutycycle_buffer.buffer[i - 1]);
            }

            response =
                "f " + id_str + std::to_string(flicker_error / dutycycle_buffer.buffer.size());
            Serial.println(response.c_str());

        default:
            SEND_ERR();
            LOGGER_SEND_ERROR("Invalid monitor");
            break;
    }
}

void command_handle(Command& cmd) {
    // Check the type of the command
    if (std::holds_alternative<LuminaireCmd>(cmd)) {
        LOGGER_SEND_INFO("Luminaire command received");
        process_luminair_cmd(std::get<LuminaireCmd>(cmd));
    } else if (std::holds_alternative<MonitorCmd>(cmd)) {
        LOGGER_SEND_INFO("Monitor command received");
        process_monitor_cmd(std::get<MonitorCmd>(cmd));
    } else {
        LOGGER_SEND_ERROR("Unknown command received");
    }
}