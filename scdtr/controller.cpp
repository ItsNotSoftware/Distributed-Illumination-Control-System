#include "include/controller.hpp"

#include "include/communication.hpp"
#include "include/led.hpp"

Controller::Controller(float Kp, float Ki, float Kd, float b, float c, float target)
    : prev_timestamp(0),
      anti_windup(true),
      target(target),
      feedback(true),
      Kp(Kp),
      Ki(Ki),
      Kd(Kd),
      b(b),
      c(c),
      integral(0) {}

// Setters
void Controller::set_Kp(float Kp) { this->Kp = Kp; }
void Controller::set_Ki(float Ki) { this->Ki = Ki; }
void Controller::set_Kd(float Kd) { this->Kd = Kd; }
void Controller::set_target(float target) { this->target = target; }
void Controller::set_anti_windup_state(bool enable) { anti_windup = enable; }
void Controller::set_feedback(bool enable) { feedback = enable; }
void Controller::set_c(float c) { this->c = c; }
void Controller::set_b(float b) { this->b = b; }

// Getters
bool Controller::get_feedback() { return feedback; }
bool Controller::get_anti_windup_state() { return anti_windup; }
float Controller::get_target() { return target; }
float Controller::get_Kp() { return Kp; }
float Controller::get_Ki() { return Ki; }
float Controller::get_Kd() { return Kd; }
float Controller::get_c() { return c; }
float Controller::get_b() { return b; }

uint8_t Controller::compute_pwm_signal(float y, uint32_t current_time) {
    if (!feedback) {
        return target * 17;  // target * system_gain^-1
    }

    // Calculate dt and update time
    float dt = current_time - prev_timestamp;
    prev_timestamp = current_time;

    float proportional_error = target * b - y;
    float integral_error = target - y;
    float derivative_error = target * c - y;

    integral += integral_error * dt;

    // PID terms
    float P = Kp * proportional_error;
    float I = Ki * integral;
    float D = Kd * (derivative_error - prev_derivative_error) / dt;

    prev_derivative_error = derivative_error;

    uint8_t pwm_signal = std::round(P + I + D);

    // Anti-windup
    if (anti_windup) {
        if (pwm_signal > DAC_RANGE) {
            pwm_signal = DAC_RANGE;
            integral = 0;
        } else if (pwm_signal < 0.0) {
            pwm_signal = 0.0;
            integral = 0;
        }
    } else {
        // clamp the pwm signal
        pwm_signal = pwm_signal > DAC_RANGE ? DAC_RANGE : pwm_signal < 0.0 ? 0.0 : pwm_signal;
    }

    return pwm_signal;
}