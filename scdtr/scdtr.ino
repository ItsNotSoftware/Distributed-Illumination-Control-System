#include <Arduino.h>

#include "hardware/resets.h"
#include "hardware/watchdog.h"
#include "include/command.hpp"
#include "include/communication.hpp"
#include "include/controller.hpp"
#include "include/led.hpp"
#include "include/luxmeter.hpp"
#include "include/ring_buffer.hpp"
#include "include/thread_safe_fifo.hpp"
#include "pico/stdlib.h"

constexpr uint32_t CONTROLLER_INTERVAL = 10;  // ms
constexpr uint32_t ADC_SAMPLE_INTERVAL = 1;   // ms

Luxmeter luxmeter(A0);
LED led(15);

Controller controller(5.25, 0.05, 0.1, 1, 1, 250);
bool contoller_active = true;

RingBuffer<float, 100> lux_buffer;
RingBuffer<float, 100> dutycycle_buffer;

bool steam_lux = false;
bool stream_dutycycle = false;

ThreadSafeFifo<Command> fifo0;  // FIFO for IPC
Command cmd0;                   // Command object

uint8_t id = 0;  // lumminair id

// Time keeping for scheduling
uint32_t curr_time = 0;
uint32_t prev_controller_t = 0;
uint32_t prev_adc_t = 0;

void setup() {
    Serial.begin(115200);
    pinMode(PICO_DEFAULT_LED_PIN, OUTPUT);  // Error LED
    delay(100);
    digitalWrite(PICO_DEFAULT_LED_PIN, LOW);
    delay(100);

    // Check if the system was reset by the watchdog
    if (watchdog_caused_reboot()) {
        digitalWrite(PICO_DEFAULT_LED_PIN, HIGH);
        LOGGER_SEND_ERROR("Watchdog caused a reboot");
    }

    // Setting up a wachdog timmer
    watchdog_enable(CONTROLLER_INTERVAL * 3, 1);
}
void setup1() {}

/* Stream the lux and dutycycle data to the serial port */
inline void stream() {
    if (stream_dutycycle) {
        std::string msg = "s d " + std::to_string(id) + ' ' +
                          std::to_string(dutycycle_buffer.pop()) + ' ' + std::to_string(curr_time);

        Serial.println(msg.c_str());
    }
    if (steam_lux) {
        std::string msg = "s l " + std::to_string(id) + ' ' + std::to_string(lux_buffer.pop()) +
                          ' ' + std::to_string(curr_time);

        Serial.println(msg.c_str());
    }
}

/**
 * [Core #0 loop]:
 *     Task1 -> Contorller (100Hz).
 *     Task2 -> ADC sampling (1kHz).
 *     Task3 -> Command handling (if fifo has a command).
 */
void loop() {
    curr_time = millis();

    // Task 1: Controller
    if (curr_time - prev_controller_t >= CONTROLLER_INTERVAL) {
        prev_controller_t = curr_time;

        float mv = luxmeter.get_mv();
        float lux = luxmeter.mv_to_lux(mv);

        if (contoller_active) {
            uint8_t u = controller.compute_pwm_signal(mv, curr_time);
            led.set_pwm_range(u);

            LOGGER_SEND_CONTROLLER_DATA(curr_time, u, mv, controller.get_target());

            lux_buffer.push(lux);
            dutycycle_buffer.push(u / DAC_RANGE);

            stream();
        }
    }

    // Task 2: ADC sampling
    if (curr_time - prev_adc_t >= ADC_SAMPLE_INTERVAL) {
        prev_adc_t = curr_time;  // Update the timer

        luxmeter.sample();
    }

    // Task 3: Command handling
    if (fifo0.pop(cmd0)) {
        command_handle(cmd0);
    }

    watchdog_update();
}

/**
 * [Core #1 loop]:
 *      Task1 -> Serial communication (if msg is recived).
 */
void loop1() { SerialCom::read(); }

// note: a = 15.564501347708887; b = -1.2545202156334199
// node: response = 820ms
