#line 1 "/Users/diogoferreira/CodeWs/scdtr/scdtr.ino"
#include <Arduino.h>

#include "command.hpp"
#include "communication.hpp"
#include "led.hpp"
#include "luxmeter.hpp"
#include "thread_safe_fifo.hpp"

constexpr uint32_t LOOP_PERIOD = 1000;  // ms

Luxmeter luxmeter(A0);
LED led(15);
volatile ThreadSafeFifo fifo;

uint32_t prev_t = 0;

#line 17 "/Users/diogoferreira/CodeWs/scdtr/scdtr.ino"
void setup();
#line 18 "/Users/diogoferreira/CodeWs/scdtr/scdtr.ino"
void setup1();
#line 28 "/Users/diogoferreira/CodeWs/scdtr/scdtr.ino"
void loop();
#line 46 "/Users/diogoferreira/CodeWs/scdtr/scdtr.ino"
void loop1();
#line 17 "/Users/diogoferreira/CodeWs/scdtr/scdtr.ino"
void setup() { Serial.begin(115200); }
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
    uint32_t curr_t = millis();

    // Control loop at fixed loop rate
    if (curr_t - prev_t >= LOOP_PERIOD) {
        prev_t = curr_t;

        Serial.printf("LUX: %f\n", luxmeter.get_lux());
    }

    delay(LOOP_PERIOD);
}

/**
 * [Core #1 loop]:
 *  - Serial communication.
 *  - CAN communication.
 */
void loop1() { SerialCom::command_handle(); }

