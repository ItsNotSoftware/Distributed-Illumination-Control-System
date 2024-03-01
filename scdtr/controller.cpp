#include "controller.hpp"

#include "led.hpp"

Controller::Controller(float Kp, float Ki, float Kd, float target)
    : prev_timestamp(0), anti_windup(true), target(target), Kp(Kp), Ki(Ki), Kd(Kd), integral(0) {}

void Controller::set_Kp(float Kp) { this->Kp = Kp; }
void Controller::set_Ki(float Ki) { this->Ki = Ki; }
void Controller::set_Kd(float Kd) { this->Kd = Kd; }
void Controller::set_target(float target) { this->target = target; }
void Controller::set_anti_windup_state(bool enable) { anti_windup = enable; }

float Controller::compute_pwm_signal(float luminosity, uint32_t current_time) {
    // Calculate dt and update time
    float dt = current_time - prev_timestamp;
    prev_timestamp = current_time;

    float error = target - luminosity;
    integral += error * dt;

    // PID terms
    float P = Kp * error;
    float I = Ki * integral;
    float D = Kd * (error - prev_error) / dt;

    prev_error = error;

    float pwm_signal = P + I + D;

    // Anti-windup
    if (anti_windup) {
        if (pwm_signal > DAC_RANGE) {
            pwm_signal = DAC_RANGE;
            integral -= error * dt;
        } else if (pwm_signal < 0.0) {
            pwm_signal = 0.0;
            integral -= error * dt;
        }
    } else {
        // clamp the pwm signal
        pwm_signal = pwm_signal > DAC_RANGE ? DAC_RANGE : pwm_signal < 0.0 ? 0.0 : pwm_signal;
    }

    return pwm_signal;
}