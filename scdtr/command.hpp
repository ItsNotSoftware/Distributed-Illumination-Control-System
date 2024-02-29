#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <variant>

enum class Request { SET_VAL, GET_VAL, SET_REF, GET_REF, START, STOP };
enum class Unit { DC, LUX };
enum class Target { KD, KP, KI, ANTI_WINDUP, FEEDBACK };

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

typedef std::variant<LuminaireCmd, ContollerCmd> Command;

void command_handle(Command &command);

#endif