#include "led.hpp"

LED::LED(uint8_t pin) : pin(pin) {
    pinMode(pin, OUTPUT);
    analogWriteFreq(60000);
}

void LED::set_pwm_range(uint8_t range) { analogWrite(pin, range); }
