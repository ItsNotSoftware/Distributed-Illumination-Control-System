#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <variant>

enum class Request { SET_VAL, GET_VAL, SET_REF, GET_REF, ON, OFF };
enum class Unit { DC, LUX };
enum class Target { KD, KP, KI, ANTI_WINDUP, FEEDBACK, OCCUPANCY };
enum class TargetM {
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
    Unit unit;
    float value;
};

struct ContollerCmd {
    Request request;
    Target target;
    float value;
};

struct MonitorCmd {
    Request request;
    TargetM monitor;
    char variable;
};

typedef std::variant<LuminaireCmd, ContollerCmd, MonitorCmd> Command;

void command_handle(Command &command);

#endif