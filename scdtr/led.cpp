#include "led.hpp"

LED::LED(uint8_t pin) : pin(pin) {
    pinMode(pin, OUTPUT);
    analogWriteFreq(60000);
}

void LED::set_pwm_range(uint8_t range) {
    pwm_val = range;
    analogWrite(pin, range);
}

float LED::get_duty_cycle() { return pwm_val / DAC_RANGE; }