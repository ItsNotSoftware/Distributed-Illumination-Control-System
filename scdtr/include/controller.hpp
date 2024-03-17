#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <Arduino.h>

class Controller {
   public:
    Controller(float Kp, float Ki, float Kd, float b, float c, float target);

    // Setters
    void set_Kp(float Kp);
    void set_Ki(float Ki);
    void set_Kd(float Kd);
    void set_feedback(bool enable);
    void set_anti_windup_state(bool enable);
    void set_target(float target);
    void set_c(float c);
    void set_b(float b);

    // Getters
    float get_target();
    float get_Kp();
    float get_Ki();
    float get_Kd();
    bool get_feedback();
    bool get_anti_windup_state();
    float get_c();
    float get_b();

    /*
     * Compute the PWM signal based on the luminosity and the current time.
     *
     * @param luminosity The current luminosity.
     * @param current_time The current time.
     *
     * @return The PWM signal.
     */
    uint16_t compute_pwm_signal(float luminusity, uint32_t current_time);

   private:
    bool anti_windup;
    bool feedback;
    float target;
    float Kp, Ki, Kd;
    float b, c;
    float integral;
    float prev_derivative_error;
    uint32_t prev_timestamp;
};

#endif
