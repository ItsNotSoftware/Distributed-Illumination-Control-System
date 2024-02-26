# 1 "/Users/diogoferreira/CodeWs/scdtr/scdtr.ino"
# 2 "/Users/diogoferreira/CodeWs/scdtr/scdtr.ino" 2

# 4 "/Users/diogoferreira/CodeWs/scdtr/scdtr.ino" 2
# 5 "/Users/diogoferreira/CodeWs/scdtr/scdtr.ino" 2
# 6 "/Users/diogoferreira/CodeWs/scdtr/scdtr.ino" 2
# 7 "/Users/diogoferreira/CodeWs/scdtr/scdtr.ino" 2
# 8 "/Users/diogoferreira/CodeWs/scdtr/scdtr.ino" 2

constexpr uint32_t LOOP_PERIOD = 1000; // ms

Luxmeter luxmeter(A0);
LED led(15);
volatile ThreadSafeFifo fifo;

uint32_t prev_t = 0;

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
