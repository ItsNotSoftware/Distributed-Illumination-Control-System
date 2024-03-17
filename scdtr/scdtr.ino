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

#define SEND_CAN_TEST 0
#define RCV_CAN_TEST 0

constexpr uint32_t CONTROLLER_INTERVAL = 10;  // ms
constexpr uint32_t ADC_SAMPLE_INTERVAL = 1;   // ms

Luxmeter luxmeter(A0);
LED led(15);

Controller controller(4.5, 0.09, 0, 1, 1, 250);
bool contoller_active = true;

RingBuffer<float, 100> lux_buffer;
RingBuffer<float, 100> dutycycle_buffer;

bool steam_lux = false;
bool stream_dutycycle = false;

ThreadSafeFifo<Command> fifo0;  // FIFO for IPC
Command cmd0;                   // Command object

CAN can_handler(20);

uint8_t id = 0;  // lumminair id

// Time keeping for scheduling
uint32_t curr_time = 0;
uint32_t prev_controller_t = 0;
uint32_t prev_adc_t = 0;

can_frame can_msg;  // !!!

void setup() {
    Serial.begin(115200);
    analogWriteFreq(60000);
    analogWriteResolution(12);

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
void setup1() { can_handler.setup(); }

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

        uint16_t mv = luxmeter.get_mv();
        float lux = luxmeter.mv_to_lux(mv);

        if (contoller_active) {
            uint16_t u = controller.compute_pwm_signal(mv, curr_time);
            led.set_pwm_range(u);

            // LOGGER_SEND_CONTROLLER_DATA(curr_time, u, mv, controller.get_target());
            // LOGGER_SEND_VAL("lux", lux);

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
 *      Task1 -> CAN  communication (if msg is recived).
 *      Task2 -> Serial communication (if msg is recived).
 */
void loop1() {
    USB::handle();

#if RCV_CAN_TEST  // Test if the can bus is working
    if (can_handler.receive_msg(can_msg)) {
        std::string msg = "can -> " + std::to_string(can_msg.can_id) + ' ' +
                          std::to_string(can_msg.data[0]) + ' ' + std::to_string(can_msg.data[1]) +
                          ' ' + std::to_string(can_msg.data[2]) + ' ' +
                          std::to_string(can_msg.data[3]) + ' ' + std::to_string(can_msg.data[4]) +
                          ' ' + std::to_string(can_msg.data[5]) + ' ' +
                          std::to_string(can_msg.data[6]) + ' ' + std::to_string(can_msg.data[7]);
        LOGGER_SEND_INFO(msg.c_str());
    }
#endif

#if SEND_CAN_TEST
    can_msg.can_id = 0x123;
    can_msg.len = 8;
    can_msg.data[0] = 0x12;
    can_msg.data[1] = 0x34;
    can_msg.data[2] = 0x56;
    can_msg.data[3] = 0x78;
    can_msg.data[4] = 0x9A;
    can_msg.data[5] = 0xBC;
    can_msg.data[6] = 0xDE;
    can_msg.data[7] = 0xF0;

    can_handler.send_msg(can_msg);

#endif
}
