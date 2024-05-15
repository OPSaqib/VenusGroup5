#include <stdio.h>
#include <time.h>
#include <unistd.h> // Include the usleep function
#include <stdint.h>
#include <stdlib.h>
#include <libpynq.h>
#include <pinmap.h>
#include <switchbox.h>
#include <sys/time.h>

// Pin numbers related to color sensor:
//#define S0_PIN 4
//#define S1_PIN 5
//#define S2_PIN 7
//#define S3_PIN 6
//#define OUT_PIN  0

struct timespec start, current;

// Define the RGB structure
typedef struct {
    int red;
    int green;
    int blue;
} RGB;

void setup() {
    // Initialize PYNQ board and switchbox
    printf("Entered setup");

    pynq_init();
    switchbox_init();

    // Colorsensor init with switchbox:
    switchbox_set_pin(IO_A0, SWB_GPIO);
    switchbox_set_pin(IO_A1, SWB_GPIO);
    switchbox_set_pin(IO_A2, SWB_GPIO);
    switchbox_set_pin(IO_A3, SWB_GPIO);
    switchbox_set_pin(IO_AR4, SWB_GPIO);

    // Configure OUT_PIN as input
    //switchbox_set_pin(OUT_PIN, SWB_GPIO);

    gpio_init();

    // Set pins to output mode for controlling the color sensor
    gpio_set_direction(IO_A0, GPIO_DIR_OUTPUT);
    gpio_set_direction(IO_A1, GPIO_DIR_OUTPUT);
    gpio_set_direction(IO_A2, GPIO_DIR_OUTPUT);
    gpio_set_direction(IO_A3, GPIO_DIR_OUTPUT);
    gpio_set_direction(IO_AR4, GPIO_DIR_INPUT);

    //gpio_set_direction(OUT_PIN, GPIO_DIR_OUTPUT);

    // Initially S0 is high and S1 is low:
    gpio_set_level(IO_A0, GPIO_LEVEL_HIGH);
    gpio_set_level(IO_A1, GPIO_LEVEL_LOW);

    //gpio_set_level(OUT_PIN, GPIO_LEVEL_HIGH);

    //gpio_level_t test = gpio_get_level(OUT_PIN);

    //if (test == GPIO_LEVEL_LOW) {
        //printf("S0 pin is high as set");
    //} else {
        //printf("Error");
    //}

    // Start serial communication for debugging
    uart_init(UART0);
    uart_reset_fifos(UART0);

    adc_init();
}

// Measure pulse duration on a specific pin

int pulseIn() {
    // Record the start time
    //struct timespec start_time;
    //clock_gettime(CLOCK_MONOTONIC, &start_time);

    //gpio_set_level(OUT_PIN, GPIO_LEVEL_LOW);

    // Wait for the specified state
    //while (gpio_get_level(OUT_PIN) != GPIO_LEVEL_HIGH) {}

    // Record the end time
    //struct timespec end_time;
    //clock_gettime(CLOCK_MONOTONIC, &end_time);

    // Calculate the pulse duration in microseconds
    //long duration_us = (end_time.tv_sec - start_time.tv_sec) * 1000000L +
      //                 (end_time.tv_nsec - start_time.tv_nsec) / 1000L;

    return 10000;
}

// Get red value:
int process_red_value() {
    gpio_set_level(IO_A2, GPIO_LEVEL_LOW);
    gpio_set_level(IO_A3, GPIO_LEVEL_LOW);

    // Set the pin to low to start the measurement
    gpio_set_level(IO_AR4, GPIO_LEVEL_LOW);

    // Wait for the signal to go low
    while (gpio_get_level(IO_AR4) != GPIO_LEVEL_LOW) {}

    // Measure the time before the signal goes high
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Wait for the signal to go high
    while (gpio_get_level(IO_AR4) != GPIO_LEVEL_HIGH) {}

    // Measure the time again when the signal goes high
    clock_gettime(CLOCK_MONOTONIC, &current);

    //in mircoseconds:
    int interval = (current.tv_sec - start.tv_sec) * 1e6 + (current.tv_nsec - start.tv_nsec) / 1e3;

    return interval;
}

// Get green value:
int process_green_value() {
    gpio_set_level(IO_A2, GPIO_LEVEL_HIGH);
    gpio_set_level(IO_A3, GPIO_LEVEL_HIGH);

    // Set the pin to low to start the measurement
    gpio_set_level(IO_AR4, GPIO_LEVEL_LOW);

    // Wait for the signal to go low
    while (gpio_get_level(IO_AR4) != GPIO_LEVEL_LOW) {}

    // Measure the time before the signal goes high
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Wait for the signal to go high
    while (gpio_get_level(IO_AR4) != GPIO_LEVEL_HIGH) {}

    // Measure the time again when the signal goes high
    clock_gettime(CLOCK_MONOTONIC, &current);

    //in mircoseconds:
    int interval = (current.tv_sec - start.tv_sec) * 1e6 + (current.tv_nsec - start.tv_nsec) / 1e3;

    return interval;
}

// Get blue value:
int process_blue_value() {
    gpio_set_level(IO_A2, GPIO_LEVEL_LOW);
    gpio_set_level(IO_A3, GPIO_LEVEL_HIGH);

    // Set the pin to low to start the measurement
    gpio_set_level(IO_AR4, GPIO_LEVEL_LOW);

    // Wait for the signal to go low
    while (gpio_get_level(IO_AR4) != GPIO_LEVEL_LOW) {}

    // Measure the time before the signal goes high
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Wait for the signal to go high
    while (gpio_get_level(IO_AR4) != GPIO_LEVEL_HIGH) {}

    // Measure the time again when the signal goes high
    clock_gettime(CLOCK_MONOTONIC, &current);

    //in mircoseconds:
    int interval = (current.tv_sec - start.tv_sec) * 1e6 + (current.tv_nsec - start.tv_nsec) / 1e3;

    return interval;
}

// Main method to be run:
void loop() {

    printf("Entered loop");

    // Run the code for testing:
    for (int i = 1; i <= 10; i++) {
        int red_val = process_red_value();
        sleep_msec(100);
        int green_val = process_green_value();
        sleep_msec(100);
        int blue_val = process_blue_value();
        sleep_msec(100);

        printf("red_value = %d\n", red_val); // Use printf with correct format string
        printf("g = %d\n", green_val);
        printf("b = %d\n", blue_val);
        printf("\n");
    }
}

// Run the code:
int main() {
    printf("entered main");
    setup();
    loop();
    return 0;
}
