#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <Arduino.h>

class Controller {
   public:
    Controller(float Kp, float Ki, float Kd, float target);

    void set_Kp(float Kp);
    void set_Ki(float Ki);
    void set_Kd(float Kd);
    void set_anti_windup_state(bool enable);
    void set_target(float target);

    float compute_pwm_signal(float luminusity, uint32_t current_time);

   private:
    uint32_t prev_timestamp;
    bool anti_windup = true;
    float target;
    float Kp, Ki, Kd;
    float integral;
    float prev_error;
};

#endif
