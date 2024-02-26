#line 1 "/Users/diogoferreira/CodeWs/scdtr/luxmeter.hpp"
#ifndef LUXMETER_HPP
#define LUXMETER_HPP

#include <Arduino.h>

#include <array>
#include <cmath>
#include <numeric>

constexpr uint16_t ADC_RANGE = 4096;
constexpr uint16_t N_AVG_POINTS = 8;  // number of points in running average

// LDR voltage to lux conversion parameters
constexpr float m = -0.8;
constexpr float b = 6.12;

/* Class to measure and manage LDR */
class Luxmeter {
   public:
    /**
     * Initializes ADC.
     *
     * @param pin Pin connected to the LDR.
     */
    Luxmeter(uint8_t pin);

    /**
     * Get measured voltage in mV.
     *
     * @return Voltage in mV.
     */
    uint16_t get_mv();

    /**
     * Read ADC and add measument to running average.
     *
     * @param n_samples(optional) Number of samples to take.
     * @param delay_ms(optional) Delay between samples in ms.
     *
     * @return Resistance in ohms.
     */
    void sample();
    void sample(uint8_t n_samples, uint16_t delay_ms);

    /**
     * Read ADC value and convert to LUX.
     *
     * @return LUX value.
     */
    float get_lux();

    /**
     * Get resistance value of LDR.
     *
     * @param v_mv LDR voltage in mV.
     *
     * @return Resistance in ohms.
     */
    float get_LDR_resistance(uint16_t v_mv);

   private:
    uint8_t pin;
    std::array<uint16_t, N_AVG_POINTS> run_avg = {0};  // running average array
    uint16_t run_avg_idx = 0;

    /**
     * Convert ADC value to voltage in mV.
     *
     * @param adc ADC value.
     *
     * @return Voltage in mV.
     */
    uint16_t adc_to_mv(uint16_t adc);
};

#endif