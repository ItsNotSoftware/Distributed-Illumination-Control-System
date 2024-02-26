#line 1 "/Users/diogoferreira/CodeWs/scdtr/communication.cpp"
#include "communication.hpp"



namespace SerialCom {
void command_handle() {
    if (Serial.available() > 0) {
        std::string msg = Serial.readString().c_str();

        // Auxiliar varible to split string into words
        std::istringstream iss(msg);
        std::vector<std::string> args;
        std::string word;

        // Split the message into words
        while (iss >> word) {
            args.push_back(word);
        }

        Serial.println("\nReceived msg: \n");
        for (auto &arg : args) {
            Serial.println(arg.c_str());
        }
        Serial.println("\n");
    }
}
}  // namespace SerialCom
