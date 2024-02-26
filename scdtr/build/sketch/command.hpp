#line 1 "/Users/diogoferreira/CodeWs/scdtr/command.hpp"
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <variant>

enum class Request { SET_VAL, GET_VAL, SET_REF, GET_REF, START, STOP };
enum class Unit { DC, LUX };
enum class Gain { KD, KP, KI };

struct LuminaireCmd {
    Request request;
    Unit unit;
    float value;
};

struct OcuppancyCmd {
    Request request;
    bool value;
};

typedef std::variant<LuminaireCmd, OcuppancyCmd> Command;

#endif