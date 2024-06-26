#ifndef LED_HPP
#define LED_HPP

#include <Arduino.h>

constexpr uint16_t DAC_RANGE = 4095;

/* Class to control the light LED. */
class LED {
   public:
    /**
     * Initializes PWM.
     *
     * @param pin Pin connected to the LED.
     */
    LED(uint8_t pin);

    /**
     * Sets the PWM value [0,4096].
     *
     * @param range PWM range.
     */
    void set_pwm_range(uint16_t range);

    /**
     * Sets the pwm duty cycle.
     *
     * @param duty_cycle Duty cycle [0,1].
     */
    inline void set_duty_cycle(float duty_cycle) { set_pwm_range((float)DAC_RANGE * duty_cycle); };

    /**
     * Sets the luminosity percentage.
     *
     * @param percentage Luminosity percentage [0,100].
     */
    inline void set_luminosity_percentage(float percentage) { set_duty_cycle(percentage / 100.0); };

    /**
     * Gets the current duty cycle.
     *
     * @return Duty cycle [0,1].
     */
    float get_duty_cycle();

   private:
    uint8_t pin;  // Pin connected to the LED.
    float pwm_val;
};

#endif