#include <libpynq.h>
#include <pinmap.h>
#include <switchbox.h>

// Pin numbers related to color sensor:
#define S0_PIN 5
#define S1_PIN 4
#define S2_PIN 7
#define S3_PIN 6
#define OUT_PIN  8

void setup() {
    // Initialize PYNQ board and switchbox
    pynq_init();
    switchbox_init();

    // Colorsensor init with switchbox:
    switchbox_set_pin(S0_PIN, SWB_GPIO);
    switchbox_set_pin(S1_PIN, SWB_GPIO);
    switchbox_set_pin(S2_PIN, SWB_GPIO);
    switchbox_set_pin(S3_PIN, SWB_GPIO);
    switchbox_set_pin(OUT_PIN, SWB_PWM0);

    // Set pins to output mode for controlling the color sensor
    gpio_init(S0_PIN, GPIO_OUT);
    gpio_init(S1_PIN, GPIO_OUT);
    gpio_init(S2_PIN, GPIO_OUT);
    gpio_init(S3_PIN, GPIO_OUT);

    // Initially S0 is high and S1 is low:
    gpio_set_level(S0_PIN, GPIO_LEVEL_HIGH);
    gpio_set_level(S1_PIN, GPIO_LEVEL_LOW);

    // Start serial communication for debugging
    uart_init(UART0);
    uart_reset_fifos(UART0);
    Serial.begin(9600);
}

// Main method to be run:
void loop() {
    // Run the code for testing:
    for (int i = 1; i <= 5; i++) {
        int red_val = process_red_value();
        int green_val = process_green_value();
        int blue_val = process_blue_value();

        Serial.println("r = " + String(red_val));
        Serial.println("g = " + String(green_val));
        Serial.println("b = " + String(blue_val));
        Serial.println("____________________________");
    }
}

// Get red value:
int process_red_value() {
    gpio_set_level(S2_PIN, GPIO_LEVEL_LOW);
    gpio_set_level(S3_PIN, GPIO_LEVEL_LOW);

    int analog_value = pulseIn(OUT_PIN, GPIO_LEVEL_LOW, 1000000L);

    sleep_msec(100);
    return analog_value;
}

// Get green value:
int process_green_value() {
    gpio_set_level(S2_PIN, GPIO_LEVEL_HIGH);
    gpio_set_level(S3_PIN, GPIO_LEVEL_HIGH);

    int analog_value = pulseIn(OUT_PIN, GPIO_LEVEL_LOW, 1000000L);

    sleep_msec(100);
    return analog_value;
}

// Get blue value:
int process_blue_value() {
    gpio_set_level(S2_PIN, GPIO_LEVEL_LOW);
    gpio_set_level(S3_PIN, GPIO_LEVEL_HIGH);

    int analog_value = pulseIn(OUT_PIN, GPIO_LEVEL_LOW, 1000000L);

    sleep_msec(100);
    return analog_value;
}

// Measure pulse duration on a specific pin
int pulseIn(uint8_t pin, gpio_level_t state, unsigned long timeout) {
    // Initialize PWM
    pwm_init(PWM0, timeout);

    // Wait for the specified state
    unsigned long start_time = timer_get_ticks();
    while (gpio_get_level(pin) != state) {
        if (timer_get_ticks() - start_time >= timeout) {
            // Timeout reached
            pwm_destroy(PWM0); // Clean up PWM
            return 0; // Return 0 to indicate timeout
        }
    }

    // Measure pulse length
    unsigned long pulse_start_time = timer_get_ticks();
    while (gpio_get_level(pin) == state) {
        if (timer_get_ticks() - pulse_start_time >= timeout) {
            // Timeout reached
            pwm_destroy(PWM0); // Clean up PWM
            return 0; // Return 0 to indicate timeout
        }
    }
    unsigned long pulse_length = timer_get_ticks() - pulse_start_time;

    // Clean up PWM
    pwm_destroy(PWM0);

    return pulse_length;
}

// Run the code:
int main() {
    setup();
    loop();
    return 0;
}
