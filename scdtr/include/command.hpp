#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <variant>

enum class Request { SET, GET, TURN_ON, TURN_OFF };
enum class Target { KD, KP, KI, B, C, REF, DC, LUX, EXT_LUX, ANTI_WINDUP, FEEDBACK, OCCUPANCY };
enum class Monitor {
    INST_POWER_COMSUMPTION,
    AVG_POWER_CONSUMPTION,
    TIME_SINSE_RESTART,
    START_STREAM,
    STOP_STREAM,
    BUFFER,
    AVG_VISIBILITY_ERROR,
    AVG_FLICKER_ERROR,
};

struct LuminaireCmd {
    Request request;
    Target target;
    float value;
};

struct MonitorCmd {
    Monitor monitor;
    char variable;
};

typedef std::variant<LuminaireCmd, MonitorCmd> Command;

/*
 * Handle the command
 *
 * @param command The command to be handled
 */
void command_handle(Command &command);

#endif