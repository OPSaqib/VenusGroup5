//All libraries to help us:
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <libpynq.h>
#include <pinmap.h>
#include <switchbox.h>
#include <sys/time.h>
#include <iic.h>
#include "vl53l0x.h"
#include <string.h>
#include <arm_shared_memory_system.h>
#include <json-c/json.h>
#include <json-c/json_object.h>
#include <platform.h>
#include <stepper.h>

//ColorSensor init variables:
struct timespec start, current;
#define measurements 15
int red_values[measurements];
int green_values[measurements];
int blue_values[measurements];

int red = 0;
int blue = 0;
int green = 0;

//DistanceSensor init:
extern int vl53l0x_example_dual();
vl53x sensorA;
vl53x sensorB;

int distanceSensorA = 0;
int distanceSensorB = 0;

//Movement init:
//

//Algorithm init:
#define MAX_COORDINATES 100
int x = 0; //store current x co-ordinate
int y = 0; //store current y co-ordinate

//Store the visited co-ordinates:
typedef struct {
    int x;
    int y;
} VisitedCoordinates;

VisitedCoordinates visitedCoordinates[MAX_COORDINATES];
int numElements = 0; //globally store how many elements entered

typedef struct {
    int x;
    int y;
    char str[100]; // Allocate enough space for the string
} CoordinateDetails;

CoordinateDetails coordinateDetails[MAX_COORDINATES];

//ex to add an element do: visitedCoordinates[numElements] = (VisitedCoordinates){x, y};
//Replace x,y with coordinate of elemet you want to add
//Then don't forget to do: numElements++;

//COLORSENSOR:

void setupColorSensor() {
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

    gpio_init();

    // Set pins to output mode for controlling the color sensor
    gpio_set_direction(IO_A0, GPIO_DIR_OUTPUT);
    gpio_set_direction(IO_A1, GPIO_DIR_OUTPUT);
    gpio_set_direction(IO_A2, GPIO_DIR_OUTPUT);
    gpio_set_direction(IO_A3, GPIO_DIR_OUTPUT);
    gpio_set_direction(IO_AR4, GPIO_DIR_INPUT);

    // Initially S0 is high and S1 is low:
    gpio_set_level(IO_A0, GPIO_LEVEL_HIGH);
    gpio_set_level(IO_A1, GPIO_LEVEL_LOW);

    //uart_init(UART0);
    //uart_reset_fifos(UART0);

    adc_init();
}

// Get red value (COLORSENSOR):
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

// Get green value (COLORSENSOR):
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

// Get blue value (COLORSENSOR):
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

// Function to get RGB values 10 times and store in an array (COLORSENSOR):
void get_rgb_values(int arr_size) {

    for (int i = 0; i < arr_size; i++) {

        int red = process_red_value();
        sleep_msec(100);

        int green = process_green_value();
        sleep_msec(100);

        int blue = process_blue_value();

        sleep_msec(100);

        red_values[i] = red;
        green_values[i] = green;
        blue_values[i] = blue;
    }
}

// Function to swap two elements
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Partition function
int partition(int array[], int low, int high) {
    int pivot = array[high];
    int i = (low - 1);

    for (int j = low; j < high; j++) {
        if (array[j] <= pivot) {
            i++;
            swap(&array[i], &array[j]);
        }
    }
    swap(&array[i + 1], &array[high]);
    return (i + 1);
}

// QuickSort function
void quickSort(int array[], int low, int high) {
    if (low < high) {
        int pi = partition(array, low, high);

        quickSort(array, low, pi - 1);
        quickSort(array, pi + 1, high);
    }
}

void sortRGBArrays() {

    quickSort(red_values, 0, measurements - 1);
    quickSort(green_values, 0, measurements - 1);
    quickSort(blue_values, 0, measurements - 1);
    
}

int bestRedValue() {

    if (measurements % 2 == 0) {
        return red_values[measurements / 2];
    } else {
        return green_values[(measurements + 1) / 2];   //shoudn't there be red_values[]??
    }
}

int bestGreenValue() {

    if (measurements % 2 == 0) {
        return green_values[measurements / 2];
    } else {
        return green_values[(measurements + 1) / 2];
    }
}

int bestBlueValue() {

    if (measurements % 2 == 0) {
        return blue_values[measurements / 2];
    } else {
        return blue_values[(measurements + 1) / 2];
    }
}
 void setCertainColour(){
    
 }
void updateColourSensorValues() {
    get_rgb_values(measurements);
    sortRGBArrays();

    red = bestRedValue();
    blue = bestBlueValue();
    green = bestGreenValue();
    setCertainColour();
}

//NOW DISTANCE SENSOR:

int setupDistanceSensors(void) {
    switchbox_set_pin(IO_AR_SCL, SWB_IIC0_SCL);
    switchbox_set_pin(IO_AR_SDA, SWB_IIC0_SDA);
    iic_init(IIC0);

    int i;
    //Setup Sensor A
    printf("Initialising Sensor A:\n");

    //Change the Address of the VL53L0X
    uint8_t addrA = 0x69;
    i = tofSetAddress(IIC0, 0x29, addrA);
    printf("---Address Change: ");
    if(i != 0)
    {
        printf("Fail\n");
        return 1;
    }
    printf("Succes\n");
    
    i = tofPing(IIC0, addrA);
    printf("---Sensor Ping: ");
    if(i != 0)
    {
        printf("Fail\n");
        return 1;
    }
    printf("Succes\n");

    //Initialize the sensor

    i = tofInit(&sensorA, IIC0, addrA, 0);
    if (i != 0)
    {
        printf("---Init: Fail\n");
        return 1;
    }

    uint8_t model, revision;

    tofGetModel(&sensorA, &model, &revision);
    printf("---Model ID - %d\n", model);
    printf("---Revision ID - %d\n", revision);
    printf("---Init: Succes\n");
    fflush(NULL);

    printf("\n\nNow Power Sensor B!!\nPress \"Enter\" to continue...\n");
    getchar();

    //Setup Sensor B
    printf("Initialising Sensor B:\n");

    //Use the base addr of 0x29 for sensor B
    //It no longer conflicts with sensor A.
    uint8_t addrB = 0x29;   
    i = tofPing(IIC0, addrB);
    printf("---Sensor Ping: ");
    if(i != 0)
    {
        printf("Fail\n");
        return 1;
    }
    printf("Succes\n");

    //Initialize the sensor

    i = tofInit(&sensorB, IIC0, addrB, 0);
    if (i != 0)
    {
        printf("---Init: Fail\n");
        return 1;
    }

    tofGetModel(&sensorB, &model, &revision);
    printf("---Model ID - %d\n", model);
    printf("---Revision ID - %d\n", revision);
    printf("---Init: Succes\n");
    fflush(NULL); //Get some output even if the distance readings hang
    printf("\n");

    //iic_destroy(IIC0);
    //pynq_destroy();
    //return EXIT_SUCCESS;
}

void updateDistanceSensorA() {
    distanceSensorA = tofReadDistance(&sensorA);
}

void updateDistanceSensorB() {
    distanceSensorB = tofReadDistance(&sensorB);
}

//MOVEMENT:
//NOTE: stepper_set_speed(left, right) and stepper_steps(left, right) ie left tire is left tuple and 
//right tire is the right tuple

//NOTE: stepper_set_speed(left, right) and stepper_steps(left, right) ie left tire is left tuple and 
//right tire is the right tuple

void enable_stepper() {
  stepper_init();
  stepper_enable();
}

void disable_stepper() {
  stepper_destroy();
}

void forwards() {
  enable_stepper();
  stepper_set_speed(3072, 3072);
  stepper_steps(-200, -200); //CAN BE MODIFIED
  
  while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
  disable_stepper();
}

//turn the robot right by: right speed X, steps Y, then left steps Y/2, speed X*2 (so goes 1/2 as fast)
void right() {
  enable_stepper();
  stepper_set_speed(6144, 3072);
  stepper_steps(-1400, -200); //CAN BE MODIFIED
  while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
  disable_stepper();
}

//TODO:
//turn the robot left by: left speed X, steps Y, then right steps Y/2, speed X*2 (so goes 1/2 as fast)
void left() {
  enable_stepper();
  stepper_set_speed(3072, 6144);
  stepper_steps(-200, -1350); //CAN BE MODIFIED
  while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
  disable_stepper();
}

//COMMUNICATION:
//NOTE: for sending, use string of type: uint8_t byte[]
void setupCommunication() {
    //set UART pins
    switchbox_set_pin(IO_AR0, SWB_UART0_RX);
    switchbox_set_pin(IO_AR1, SWB_UART0_TX);
    uart_init(UART0);
    uart_reset_fifos(UART0);
}

void uart_send_array(const int uart, uint8_t *buf, uint32_t l) {
    for (uint8_t x = 0; x < l; x++) {
        uart_send(uart, buf[x]);
    }
}

int communication_send(uint8_t byte[]) {
    if (uart_has_space(UART0))
    {
        //uint8_t byte[] = "Hello!\n";                      // Define string as an array of type uint8_t
        //might need to use strcpy(byte, "Hello!") or sprintf(byte, "Hello!") or sprintf(byte, "Hello! & number %i", some_int) etc.

        // Send length and than string (byte)
        uint32_t num = (sizeof(byte)*8);                    // Size of the string in bytes (in 32bit format)
        uint8_t length[4];
        length[0] = (uint8_t)(num & 0xFF);              
        length[1] = (uint8_t)((num >> 8) & 0xFF);          
        length[2] = (uint8_t)((num >> 16) & 0xFF);         
        length[3] = (uint8_t)((num >> 24) & 0xFF);          
            
        uart_send_array(UART0, &length[0], 4);              
        uart_send_array(UART0, &byte[0], num);
        return 1;                                           //return 1 == send success
    } else 
    {
        return 0;                                           //return 0 == send fail ---> try sending again
    }    
}

// 0 = y_increasing (going up), 1 = y_decreasing (going down)
void updateCoordinate(char situation, int z) {
    if (numElements < MAX_COORDINATES) {
        visitedCoordinates[numElements].x = x;
        visitedCoordinates[numElements].y = y;

        if (z == 0) {
            coordinateDetails[numElements].x = x;
            coordinateDetails[numElements].y = y + 1;
            strcpy(coordinateDetails[numElements].str, situation);
        } else {
            coordinateDetails[numElements].x = x;
            coordinateDetails[numElements].y = y - 1;
            strcpy(coordinateDetails[numElements].str, situation);
        }
        numElements++;
    } else {
        printf("Error: Maximum number of coordinates reached.\n");
    }
}

//For calling this do:
//NOW CALL THE METHODS, ADD A MSC DELAY (sleep_msec(100)) ie between calling of methods

//ALGORITHM MAIN PART STARTS HERE:

char investigateCoordinate() {
    char result[100]; // Allocate enough memory for the longest expected string

    // Update sensor values
    updateColourSensorValues();
    updateDistanceSensorA();
    updateDistanceSensorB();

    if (distanceSensorA <= 250) {
        strcpy(result, "HillAhead");
    } 
    else if (distanceSensorB <= 50) {
        if (red > 0 && green > 0 && blue == 0) {
            strcpy(result, "3x3BlockRed");
        }
        else if (red == 0 && green > 0 && blue > 0) {
            strcpy(result, "3x3BlockBlue");
        }
        else if (red == 0 && green > 0 && blue == 0) {
            strcpy(result, "3x3BlockGreen");
        }
    }
    else if (distanceSensorB <= 100) {
        if (red > 0 && green > 0 && blue == 0) {
            strcpy(result, "6x6BlockRed");
        }
        else if (red == 0 && green > 0 && blue > 0) {
            strcpy(result, "6x6BlockBlue");
        }
        else if (red == 0 && green > 0 && blue == 0) {
            strcpy(result, "6x6BlockGreen");
        }
    }
    else if (red > 0 && green == 0 && blue == 0) {
        strcpy(result, "TapeOrCliff");
    } else {
        strcpy(result, "Nothing");
    }

    return result;
}

void forward_y_increasing() {
    forwards();
    sleep_msec(100);
    y = y + 1;
}

void forward_y_decreasing() {
    forwards();
    sleep_msec(100);
    y = y - 1;
}

void yplus_direction_movement() {
    char coordinateDetails = investigateCoordinate();
    
    if (coordinateDetails == "Nothing") {
        checkUnexploredRegion();
        updateCoordinate(coordinateDetails, 0);
        void forward_y_increasing();
    }
    else if (coordinateDetails == "3x3BlockRed" || coordinateDetails == "3x3BlockBlue" 
        || coordinateDetails == "3x3BlockGreen" || coordinateDetails == "6x6BlockRed" 
        || coordinateDetails == "6x6BlockBlue" || coordinateDetails == "6x6BlockGreen") {
            right();
            x = x + 1;
            y = y + 1;
            updateCoordinate(coordinateDetails, 0);
            left();
            y = y + 1;
            updateCoordinate(coordinateDetails, 0);
            left();
            x = x - 1;
            updateCoordinate(coordinateDetails, 0);
            yplus_direction_movement(); //continue with forward movement
    } else {
        checkUnexploredRegionUpwards();
        right();
        x = x + 1;
        y = y + 1;
        updateCoordinate(coordinateDetails, 0);
        right();
        y = y - 1;
        updateCoordinate(coordinateDetails, 0);
        yminus_direction_movement();
    }
}

void yminus_direction_movement() {
    char*coordinateDetails = investigateCoordinate();

    if (coordinateDetails == "Nothing") {
        checkUnexploredRegionDownwards();
        updateCoordinate(coordinateDetails, 1);
        void forward_y_increasing();
    } else if (coordinateDetails == "3x3BlockRed" || coordinateDetails == "3x3BlockBlue" 
        || coordinateDetails == "3x3BlockGreen" || coordinateDetails == "6x6BlockRed" 
        || coordinateDetails == "6x6BlockBlue" || coordinateDetails == "6x6BlockGreen") {
            left();
            x = x + 1;
            y = y - 1;
            updateCoordinate(coordinateDetails, 1);
            right();
            y = y - 1;
            updateCoordinate(coordinateDetails, 1);
            right();
            x = x - 1;
            updateCoordinate(coordinateDetails, 1);
            yminus_direction_movement(); //continue with forward movement
    } else {
        checkUnexploredRegion();
        left();
        x = x + 1;
        y = y - 1;
        updateCoordinate(coordinateDetails, 1);
        right();
        y = y + 1;
        updateCoordinate(coordinateDetails, 1);
        yplus_direction_movement();
    }
}

//**To Be Finished**(MAY BE DITCHED IF DEEMEND UNECESSARY)//
void checkUnexploredRegionUpwards() {
    for (int i = 0; i < numElements; i++) {
        int test;
        int test2;
        test = visitedCoordinates[i].x;
        test2 = visitedCoordinates[i].y;
    }
}

//**To Be Finished**(MAY BE DITCHED IF DEEMEND UNECESSARY)//
void checkUnexploredRegionDownwards() {
    for (int i = 0; i < numElements; i++) {
        int test;
        int test2;
        test = visitedCoordinates[i].x;
        test2 = visitedCoordinates[i].y;
    }
}

void alg() {
    yplus_direction_movement();
}

//Actually execute everything:
int main(void) {
    setupColorSensor();
    setupDistanceSensors();
    setupCommunication();
    //just do the init coordinate (0,0):
    updateCoordinate();
    alg(); //RUN THE DESIRED ALGORITHM
    pynq_destroy();
    return EXIT_SUCCESS;
}
