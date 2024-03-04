#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <variant>

enum class Request { SET, GET, TURN_ON, TURN_OFF };
enum class Target { KD, KP, KI, REF, DC, LUX, ANTI_WINDUP, FEEDBACK, OCCUPANCY };
enum class Monitor {
    INST_POWER_COMSUMPTION,
    AVG_POWER_CONSUMPTION,
    TIME_SINSE_RESTART,
    STREAM,
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
    Request request;
    Monitor monitor;
    char variable;
};

typedef std::variant<LuminaireCmd, MonitorCmd> Command;

void command_handle(Command &command);

#endif