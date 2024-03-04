#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <Arduino.h>

class Controller {
   public:
    Controller(float Kp, float Ki, float Kd, float target);

    void set_Kp(float Kp);
    void set_Ki(float Ki);
    void set_Kd(float Kd);
    void set feedback(bool enable);
    void set_anti_windup_state(bool enable);
    void set_target(float target);

    bool get_feedback();
    float get_target();
    float get_Kp();
    float get_Ki();
    float get_Kd();

    float compute_pwm_signal(float luminusity, uint32_t current_time);

   private:
    bool anti_windup;
    bool feedback;
    float target;
    float Kp, Ki, Kd;
    float integral;
    float prev_error;
    uint32_t prev_timestamp;
};

#endif
