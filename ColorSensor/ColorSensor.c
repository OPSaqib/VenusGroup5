#include <libpynq.h>
#include <pinmap.h>
#include <switchbox.h>
#include <unistd.h> // Include the usleep function
#include <time.h>

// Pin numbers related to color sensor:
#define S0_PIN 4
#define S1_PIN 5
#define S2_PIN 7
#define S3_PIN 6
#define OUT_PIN  0

void setup() {
    // Initialize PYNQ board and switchbox
    pynq_init();
    switchbox_init();

    // Colorsensor init with switchbox:
    switchbox_set_pin(S0_PIN, SWB_GPIO);
    switchbox_set_pin(S1_PIN, SWB_GPIO);
    switchbox_set_pin(S2_PIN, SWB_GPIO);
    switchbox_set_pin(S3_PIN, SWB_GPIO);

    // Configure OUT_PIN as input
    switchbox_set_pin(OUT_PIN, SWB_GPIO);

    gpio_init();

    // Set pins to output mode for controlling the color sensor
    gpio_set_direction(S0_PIN, GPIO_DIR_OUTPUT);
    gpio_set_direction(S1_PIN, GPIO_DIR_OUTPUT);
    gpio_set_direction(S2_PIN, GPIO_DIR_OUTPUT);
    gpio_set_direction(S3_PIN, GPIO_DIR_OUTPUT);
    gpio_set_direction(OUT_PIN, GPIO_DIR_OUTPUT);

    // Initially S0 is high and S1 is low:
    gpio_set_level(S0_PIN, GPIO_LEVEL_HIGH);
    gpio_set_level(S1_PIN, GPIO_LEVEL_LOW);

    // Start serial communication for debugging
    uart_init(UART0);
    uart_reset_fifos(UART0);
}

// Measure pulse duration on a specific pin

int pulseIn() {
    // Record the start time
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    gpio_set_level(OUT_PIN, GPIO_LEVEL_LOW);

    // Wait for the specified state
    while (gpio_get_level(OUT_PIN) != GPIO_LEVEL_HIGH) {}

    // Record the end time
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    // Calculate the pulse duration in microseconds
    long duration_us = (end_time.tv_sec - start_time.tv_sec) * 1000000L +
                       (end_time.tv_nsec - start_time.tv_nsec) / 1000L;

    return (int)duration_us;
}

// Get red value:
int process_red_value() {
    gpio_set_level(S2_PIN, GPIO_LEVEL_LOW);
    gpio_set_level(S3_PIN, GPIO_LEVEL_LOW);

    int pulse_duration = pulseIn();

    return pulse_duration;
}

// Get green value:
int process_green_value() {
    gpio_set_level(S2_PIN, GPIO_LEVEL_HIGH);
    gpio_set_level(S3_PIN, GPIO_LEVEL_HIGH);

    int pulse_duration = pulseIn();

    return pulse_duration;
}

// Get blue value:
int process_blue_value() {
    gpio_set_level(S2_PIN, GPIO_LEVEL_LOW);
    gpio_set_level(S3_PIN, GPIO_LEVEL_HIGH);

    int pulse_duration = pulseIn();

    return pulse_duration;
}

// Main method to be run:
void loop() {
    // Run the code for testing:
    for (int i = 1; i <= 5; i++) {
        int red_val = process_red_value();
        int green_val = process_green_value();
        int blue_val = process_blue_value();

        printf("r = %d\n", red_val); // Use printf with correct format string
        printf("g = %d\n", green_val);
        printf("b = %d\n", blue_val);
        printf("\n");
    }
}

// Run the code:
int main(void) {
    setup();
    loop();
    return 0;
}
