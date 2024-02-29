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

// FIFOs for IPC
CommandFifo fifo0;  // FIFO for core #0
CommandFifo fifo1;  // FIFO for core #1

Command cmd0;
Command cmd1;

uint8_t id = 0;
uint32_t curr_time = 0;
uint32_t prev_controller_t = 0;
uint32_t prev_adc_t = 0;

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
 *     Task1 -> Contorller (100Hz).
 *     Task2 -> ADC sampling (1kHz).
 *     Task3 -> Command handling (if fifo has a command).
 */
void loop() {
    curr_time = millis();

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
