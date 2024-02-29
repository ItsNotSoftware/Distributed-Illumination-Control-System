#line 1 "/Users/diogoferreira/CodeWs/Distributed-Illumination-Control-System/scdtr/luxmeter.cpp"
#include "luxmeter.hpp"

Luxmeter::Luxmeter(uint8_t pin) : pin(pin) {
    pinMode(pin, INPUT);
    analogReadResolution(12);
}

uint16_t Luxmeter::get_mv() {
    // Average
    uint32_t sum = std::accumulate(run_avg.begin(), run_avg.end(), 0);
    uint16_t avg = sum / N_AVG_POINTS;

    return adc_to_mv(avg);
}

uint16_t Luxmeter::adc_to_mv(uint16_t adc) {
    uint32_t x = adc * 3300;
    return x / ADC_RANGE;
}

void Luxmeter::sample() {
    run_avg[run_avg_idx++] = analogRead(pin);
    run_avg_idx %= N_AVG_POINTS;
}

void Luxmeter::sample(uint8_t n_samples, uint16_t delay_ms) {
    for (uint8_t i = 0; i < n_samples; i++) {
        sample();
        delay(delay_ms);
    }
}

float Luxmeter::get_LDR_resistance(uint16_t v_mv) {
    float v = (float)v_mv / 1000;
    if (v == 0) {
        return 10'000;
    }

    return (3.3f * 10'000 / v) - 10'000;
}

float Luxmeter::get_lux() {
    float R = get_LDR_resistance(get_mv());
    float exp = (std::log10(R) - b) / m;

    return std::pow(10, exp);
}
