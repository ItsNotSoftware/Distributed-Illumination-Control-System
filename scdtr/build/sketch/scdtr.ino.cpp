#line 1 "/Users/diogoferreira/CodeWs/Distributed-Illumination-Control-System/scdtr/scdtr.ino"
#include <Arduino.h>

#include "command.hpp"
#include "command_fifo.hpp"
#include "communication.hpp"
#include "hardware/resets.h"
#include "hardware/watchdog.h"
#include "led.hpp"
#include "luxmeter.hpp"
#include "pico/stdlib.h"

constexpr uint32_t CONTROLLER_INTERVAL = 500;  // ms
constexpr uint32_t ADC_SAMPLE_INTERVAL = 1;    // ms

Luxmeter luxmeter(A0);
LED led(15);
CommandFifo fifo;
Command cmd;

uint8_t id = 0;
uint32_t curr_time = 0;
uint32_t prev_controller_t = 0;
uint32_t prev_adc_t = 0;

float l = 0;

#line 27 "/Users/diogoferreira/CodeWs/Distributed-Illumination-Control-System/scdtr/scdtr.ino"
void setup();
#line 42 "/Users/diogoferreira/CodeWs/Distributed-Illumination-Control-System/scdtr/scdtr.ino"
void setup1();
#line 52 "/Users/diogoferreira/CodeWs/Distributed-Illumination-Control-System/scdtr/scdtr.ino"
void loop();
#line 79 "/Users/diogoferreira/CodeWs/Distributed-Illumination-Control-System/scdtr/scdtr.ino"
void loop1();
#line 27 "/Users/diogoferreira/CodeWs/Distributed-Illumination-Control-System/scdtr/scdtr.ino"
void setup() {
    Serial.begin(115200);
    pinMode(PICO_DEFAULT_LED_PIN, OUTPUT);  // Error LED
    digitalWrite(PICO_DEFAULT_LED_PIN, LOW);
    delay(100);

    // Check if the system was reset by the watchdog
    if (watchdog_caused_reboot()) {
        digitalWrite(PICO_DEFAULT_LED_PIN, HIGH);
        LOGGER_SEND_ERROR("Watchdog caused a reboot");
    }

    // Setting up a wachdog timmer
    watchdog_enable(CONTROLLER_INTERVAL * 4, 1);
}
void setup1() {}

/**
 * [Core #0 loop]:
 *  - LDR measurments.
 *  - Luminosity controller.
 *  - LED actuation.
 *
 * @note This loop is executed at a fixed rate (LOOP_PERIOD).
 */
void loop() {
    curr_time = millis();

    if (fifo.pop(cmd)) {
        command_handle(cmd);
    }

    // Task 1: Controller
    if (curr_time - prev_controller_t >= CONTROLLER_INTERVAL) {
        prev_controller_t = curr_time;
        LOGGER_SEND_VAL("LUX", luxmeter.get_lux());
    }

    // Task 2: ADC sampling
    if (curr_time - prev_adc_t >= ADC_SAMPLE_INTERVAL) {
        prev_adc_t = curr_time;  // Update the timeo
        luxmeter.sample();
    }

    watchdog_update();
}

/**
 * [Core #1 loop]:
 *  - Serial communication.
 *  - CAN communication.
 */
void loop1() { SerialCom::read(); }

