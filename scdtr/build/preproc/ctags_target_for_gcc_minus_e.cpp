# 1 "/Users/diogoferreira/CodeWs/Distributed-Illumination-Control-System/scdtr/scdtr.ino"
# 2 "/Users/diogoferreira/CodeWs/Distributed-Illumination-Control-System/scdtr/scdtr.ino" 2

# 4 "/Users/diogoferreira/CodeWs/Distributed-Illumination-Control-System/scdtr/scdtr.ino" 2
# 5 "/Users/diogoferreira/CodeWs/Distributed-Illumination-Control-System/scdtr/scdtr.ino" 2
# 6 "/Users/diogoferreira/CodeWs/Distributed-Illumination-Control-System/scdtr/scdtr.ino" 2
# 7 "/Users/diogoferreira/CodeWs/Distributed-Illumination-Control-System/scdtr/scdtr.ino" 2
# 8 "/Users/diogoferreira/CodeWs/Distributed-Illumination-Control-System/scdtr/scdtr.ino" 2
# 9 "/Users/diogoferreira/CodeWs/Distributed-Illumination-Control-System/scdtr/scdtr.ino" 2
# 10 "/Users/diogoferreira/CodeWs/Distributed-Illumination-Control-System/scdtr/scdtr.ino" 2
# 11 "/Users/diogoferreira/CodeWs/Distributed-Illumination-Control-System/scdtr/scdtr.ino" 2

constexpr uint32_t CONTROLLER_INTERVAL = 500; // ms
constexpr uint32_t ADC_SAMPLE_INTERVAL = 1; // ms

Luxmeter luxmeter(A0);
LED led(15);
CommandFifo fifo;
Command cmd;

uint8_t id = 0;
uint32_t curr_time = 0;
uint32_t prev_controller_t = 0;
uint32_t prev_adc_t = 0;

float l = 0;

void setup() {
    Serial.begin(115200);
    pinMode(25, OUTPUT); // Error LED
    digitalWrite(25, LOW);
    delay(100);

    // Check if the system was reset by the watchdog
    if (watchdog_caused_reboot()) {
        digitalWrite(25, HIGH);
        Serial.println("[ERROR]: " + String("Watchdog caused a reboot"));
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
        Serial.println("[VALUE]: " + String("LUX") + " " + String(luxmeter.get_lux()));
    }

    // Task 2: ADC sampling
    if (curr_time - prev_adc_t >= ADC_SAMPLE_INTERVAL) {
        prev_adc_t = curr_time; // Update the timeo
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
