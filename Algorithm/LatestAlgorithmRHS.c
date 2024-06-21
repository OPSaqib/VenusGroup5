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
#include <string.h> // Include for strcpy and strcmp

//ColorSensor init variables:
struct timespec start, current;
#define measurements 15
int red_values[measurements];
int green_values[measurements];
int blue_values[measurements];

int red = 0;
int blue = 0;
int green = 0;

//0 means avoided nothing, 1 means avoided something
int avoid = 0;

//0 means not done, 1 means done
int smallblockdone = 0;

//DistanceSensor init:
extern int vl53l0x_example_dual();
vl53x sensorA;
vl53x sensorB;

int distanceSensorA = 0;
int distanceSensorB = 0;

//Movement init:
//

//IR init:
#define IR_SENSOR_PINA ADC5
#define IR_SENSOR_PINB ADC4
#define NUMBER_IR_VALUES 1
#define BLACK 0
#define WHITE 1
// Delay in seconds
#define DELAY 1.0 

typedef struct ir_values{
    int value;
    struct ir_values *next;
}ir_val;

//0 == black, 1 == white;
int IRSensorA;
int IRSensorB;

//Communication INIT:
bool START = false;

//Algorithm init:
#define MAX_COORDINATES 250
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

// 0 == unchecked, 1 == checked already
int checkUnexploredRegionHill = 0;

int checkUnexploredRegionCliff = 0;

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

    //if (measurements % 2 == 0) {
        //return red_values[measurements / 2];
    //} else {
        //return red_values[(measurements + 1) / 2];
    //}

    int sum = 0;

    for (int i = 0; i < measurements; i++) {
        sum = sum + red_values[i];
    }

    int returnval = sum / measurements;

    return returnval;
}

int bestGreenValue() {

    //if (measurements % 2 == 0) {
        //return green_values[measurements / 2];
    //} else {
        //return green_values[(measurements + 1) / 2];
    //}

    int sum = 0;

    for (int i = 0; i < measurements; i++) {
        sum = sum + green_values[i];
    }

    int returnval = sum / measurements;

    return returnval;
}

int bestBlueValue() {

    //if (measurements % 2 == 0) {
        //return blue_values[measurements / 2];
    //} else {
        //return blue_values[(measurements + 1) / 2];
    //}

    int sum = 0;

    for (int i = 0; i < measurements; i++) {
        sum = sum + blue_values[i];
    }

    int returnval = sum / measurements;

    return returnval;
}

void updateColourSensorValues() {
    get_rgb_values(measurements);
    //sortRGBArrays();

    red = bestRedValue();
    blue = bestBlueValue();
    green = bestGreenValue();

    //print for testing:
    printf("red: %d\n", red);
    printf("blue: %d\n", blue);
    printf("green: %d\n", green);
}

//METHODS FOR IR SENSOR:
ir_val *appendNode(ir_val *head, int value){
  //building new link
  ir_val * newNode = (ir_val *) malloc(sizeof(ir_val));
  newNode->value = value;
  newNode->next = NULL;
  //check if new link is start of the chain
  if(head == NULL){
    return newNode;
  }
  ir_val * searchLast = head;
  while(searchLast->next!=NULL){
    searchLast = searchLast->next;
  }
  searchLast->next = newNode;
  return head;
}

ir_val *freeNode(ir_val *head){
  //check if a chain exists
  if(head==NULL){return NULL;}
  //delete chain of lenght 1
  if(head->next==NULL){free(head); return NULL;}

  ir_val *current = head;
  ir_val *next = head->next;
  printf("remove curr\n\n");
  free(current);
  //current=NULL;

  return next;
}

int senseVal = 400;

int prevADC4val = 300;

int getIRValues(const adc_channel_t channel) {
    printf("Entered loop\n");

    int curr_index_IR_VALUES = 0;
    int ir_avg = 0;
    struct timeval start_time, current_time;
    double elapsed_time;
    // Saves initial time when we enter the loop
    gettimeofday(&start_time, NULL);

    while (curr_index_IR_VALUES < NUMBER_IR_VALUES) {
        gettimeofday(&current_time, NULL);
        elapsed_time = (current_time.tv_sec - start_time.tv_sec) +
                       (current_time.tv_usec - start_time.tv_usec) / 1000000.0;

        // If the amount of seconds, stored in "DELAY", has passed - take a measurement from the sensor
        if (elapsed_time >= DELAY) {
            int ir_sensor_input = adc_read_channel_raw(channel);

            if (ir_sensor_input > 1000) {
                prevADC4val = 200;

                while (ir_sensor_input > 1000) {
                    ir_sensor_input = adc_read_channel_raw(channel);
                }
            }

            if (channel == ADC5) {
                
                //if previous was white then set it up properly

                if (avoid == 1) {
                    senseVal = 300;
                } else { //didnt avoid anything use previous value
                    senseVal = prevADC4val - 75;
                }

            } else { //channel is ADC4 (right IR Sensor)
                senseVal = 405;
            }

            if (ir_sensor_input < senseVal) {
                printf("Black %d\n", ir_sensor_input);
                ir_avg += BLACK;
            } else {
                printf("White %d\n", ir_sensor_input);
                ir_avg += WHITE;

                if (channel == ADC5) {
                    prevADC4val = ir_sensor_input;
                }
            }

            curr_index_IR_VALUES++;
            // Reset the start time
            gettimeofday(&start_time, NULL); 
        }
    }

    if (ir_avg == 0) {
        printf("AVERAGE = BLACK %d\n\n", ir_avg);
        return 0;
    } else {
        printf("AVERAGE = WHITE %d\n\n", ir_avg);
        return 1;
    }

    // After saving n values from the sensor display the average
    //if (ir_avg < NUMBER_IR_VALUES / 2) {
        //printf("AVERAGE = BLACK %d\n\n", ir_avg);
        //return 0;
    //} else {
        //printf("AVERAGE = WHITE %d\n\n", ir_avg);
        //return 1;
    //}
}

//NOTE: 0 IS BLACK AND 1 IS WHITE

void updateIRSensorA() {
    IRSensorA = getIRValues(IR_SENSOR_PINA);
}

void updateIRSensorB() {
    IRSensorB = getIRValues(IR_SENSOR_PINB);
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

    return 0; //to indicate successfull running of the function

    //iic_destroy(IIC0);
    //pynq_destroy();
    //return EXIT_SUCCESS;
}

void updateDistanceSensorA() {
    iic_init(IIC0);
    distanceSensorA = tofReadDistance(&sensorA);
    iic_destroy(IIC0);

    printf("distanceSensorA: %d\n", distanceSensorA);
}

void updateDistanceSensorB() {
    iic_init(IIC0);
    distanceSensorB = tofReadDistance(&sensorB);
    iic_destroy(IIC0);

    printf("distanceSensorB: %d\n", distanceSensorB);
}

//Send data to the map from the struct, stored in the struct:
//typedef struct {
    //int x;
    //int y;
    //char str[100]; // Allocate enough space for the string
//} CoordinateDetails;
//CoordinateDetails coordinateDetails[MAX_COORDINATES];
//hence basically a 3 tuple array coordinateDetails({x, y, string})

//Communication:
//NOTE: For sending, use: sendData(string, number);
//      Works for both string and number of variable length!
//      Only send string and 1 number ---> can be modified to send more numbers!!!

void setupCommunication() {
    // Set UART pins
    switchbox_set_pin(IO_AR0, SWB_UART0_RX);
    switchbox_set_pin(IO_AR1, SWB_UART0_TX);

    // Initialize UART
    uart_init(UART0);
    uart_reset_fifos(UART0);
}

// for recieving messages --> needed for Starting the robot or recieving info from the second robot
void uart_read_array(const int uart, uint8_t *buf, uint8_t l) {
    for (uint8_t x = 0; x < l; x++) {
        buf[x] = uart_recv(uart);
    }
}

void uart_send_array(const int uart, uint8_t *buf, uint32_t l) {
    for (uint8_t x = 0; x < l; x++) {
        uart_send(uart, buf[x]);
    }
}

void compareMSG(char *msg) {
    //for input use sth. like x_coordinate and y_coordinate or whatever
    int int_in_msgA, int_in_msgB;
    char str_in_msg[50];                                         
    sscanf(msg, "%s %i %i", str_in_msg, &int_in_msgA, &int_in_msgB);              //reads the words and numbers from the message separatelly and sets them to the correct variable type
    if (strncmp(str_in_msg, "START", 5) == 0)                                     // needed for starting the robot remotely
    {
        START = true;
    }
}

void recieveData() {
    if (uart_has_data(UART0))
    {
        uint8_t size = 0;
        uart_read_array(UART0, &size, 4);                           //first part of the message is the size of the message
        unsigned char array[size];
        uart_read_array(UART0, &array[0], size);                    //second part of the message is the message itself
        printf("Data recieved: %.*s\n", size, array);               //print for check!
            
        // Convert message to different var (need char* insted of unsigned char* --> for sscanf())
        char msg[size];
        int i = 0;
        while (array[i] != '\0')
        {
            msg[i] = array[i];
            i++;
        }

        // Process message and convert to motors instructions
        compareMSG(msg);
    }
}

void sendData(char* string) {
    if(uart_has_space(UART0)) {
        size_t byte_size = strlen(string) + 1;  // Add 1 for the null terminator
        uint8_t *byte = (uint8_t *)malloc(byte_size);
        if (byte == NULL) {
            printf("Memory allocation failed\n");
            sendData(string);  // Try again if memory allocation fails
            return;
        }
        snprintf((char *)byte, byte_size, "%s", string);
    
        int byte_size_int = (int)byte_size;  // Size of the message
        uint32_t num = (byte_size_int * 8);  // Convert to bytes (in 32bit format)
        uint8_t length[4];

        // Extract each byte
        length[0] = (uint8_t)(num & 0xFF);           // Least significant byte
        length[1] = (uint8_t)((num >> 8) & 0xFF);    // Second byte
        length[2] = (uint8_t)((num >> 16) & 0xFF);   // Third byte
        length[3] = (uint8_t)((num >> 24) & 0xFF);   // Most significant byte
            
        uart_send_array(UART0, &length[0], 4);              
        uart_send_array(UART0, &byte[0], num);

        free(byte);
        printf("Sent data to sever\n");
        return;
    } else {
        sendData(string);  // Try again if uart is full and cannot send data!
    }
    return;
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

void right() {
  stepper_set_speed(-6144, -3072);
  stepper_steps(-1450, -200); //CAN BE MODIFIED
  while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
}

void left() {
  stepper_set_speed(-3072, -6144);
  stepper_steps(-200, -1450); //CAN BE MODIFIED
  while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
}

void forwards() {
  stepper_set_speed(-3072, -3072);
  stepper_steps(-150, -150); //CAN BE MODIFIED, WAS ORIGINALLY (-100, -100)
  while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
}

void backwardsmain() {
  stepper_set_speed(-3072, -3072);
  stepper_steps(650, 650); //CAN BE MODIFIED
  while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
}

void backwards2() {
  stepper_set_speed(-3072, -3072);
  stepper_steps(800, 800); //CAN BE MODIFIED
  while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
}

// 0 = y_increasing (going up), 1 = y_decreasing (going down)
void updateCoordinate(char* situation, int z) {
    if (numElements < MAX_COORDINATES) {
        visitedCoordinates[numElements].x = x;
        visitedCoordinates[numElements].y = y;

        if (z == 0) {
            coordinateDetails[numElements].x = x;
            coordinateDetails[numElements].y = y + 1;
            strcpy(coordinateDetails[numElements].str, situation);

            int temp_y = y + 1;

            int length = snprintf(NULL, 0, "X%dY%dS%s", x, temp_y, situation);
    
            // Allocate memory for the final string (+1 for null terminator)
            char* formattedString = (char*)malloc(length + 1);
            if (formattedString == NULL) {
                printf("Memory allocation failed\n");
            }
    
            // Format the string, sends a string of the format XaYbSs
            snprintf(formattedString, length + 1, "X%dY%dS%s", x, temp_y, situation);

            sendData(formattedString);
        } else {
            coordinateDetails[numElements].x = x;
            coordinateDetails[numElements].y = y - 1;
            strcpy(coordinateDetails[numElements].str, situation);

            int temp_y = y - 1;

            int length = snprintf(NULL, 0, "X%dY%dS%s", x, temp_y, situation);
    
            // Allocate memory for the final string (+1 for null terminator)
            char* formattedString = (char*)malloc(length + 1);
            if (formattedString == NULL) {
                printf("Memory allocation failed\n");
            }
    
            // Format the string, sends a string of the format XaYbSs
            snprintf(formattedString, length + 1, "X%dY%dS%s", x, temp_y, situation);

            sendData(formattedString);
        }

        //print for checking:
        printf("Coordinate %d: x = %d, y = %d, situation = %s\n", numElements, coordinateDetails[numElements].x, coordinateDetails[numElements].y, coordinateDetails[numElements].str);

        //SEND TO THE SERVER:
        //send x, y, situation to the server
        // Calculate the length of the final string

        //TODO: CHANGE THIS 

        numElements++;
    } else {
        printf("Error: Maximum number of coordinates reached.\n");
    }
}

void sendmaxCoordinates() {

    int maxX = coordinateDetails[0].x;
    int maxY = coordinateDetails[0].y;

    for (int i = 1; i < numElements; i++) {
        if (coordinateDetails[i].x > maxX) {
            maxX = coordinateDetails[i].x;
        }
        if (coordinateDetails[i].y > maxY) {
            maxY = coordinateDetails[i].y;
        }
    }

    // Print the maximum x and y values
    printf("Max X: %d\n", maxX);
    printf("Max Y: %d\n", maxY);

    static char situation[100];
    strcpy(situation, "Finished");

    int length = snprintf(NULL, 0, "X%dY%dS%s", x, y, situation);
    
    // Allocate memory for the final string (+1 for null terminator)
    char* formattedString = (char*)malloc(length + 1);
    if (formattedString == NULL) {
        printf("Memory allocation failed\n");
    }
    
    // Format the string, sends a string of the format XaYbSs
    snprintf(formattedString, length + 1, "X%dY%dS%s", x, y, situation);

    sendData(formattedString);
}

//For calling this do:
//NOW CALL THE METHODS, ADD A MSC DELAY (sleep_msec(100)) ie between calling of methods

//ALGORITHM MAIN PART STARTS HERE:

char* investigateCoordinate() {
    static char result[100]; // Allocate enough memory for the longest expected string

    // Update sensor values
    updateColourSensorValues();
    updateDistanceSensorA();
    updateDistanceSensorB();
    updateIRSensorA();
    updateIRSensorB();

    if (distanceSensorA <= 190 && distanceSensorA != 0) {
        strcpy(result, "Hill");
    } 
    else if (IRSensorA == 0 && IRSensorB == 0) {
        //we know its the final movement so just do evertyhing once more.............
        strcpy(result, "lastmovement");
    }
    else if (IRSensorA == 0) {
        strcpy(result, "TapeOrCliff");
    }
    else if (distanceSensorB <= 31) {
        if (red > 100 && green < 100 && blue < 100) {
            strcpy(result, "6x6BlockRed");
        }
        else if (red < 100 && green > 100 && blue < 100) {
            strcpy(result, "6x6BlockGreen");
        }
        else if (red < 100 && green > 100 && blue > 100) {
            strcpy(result, "6x6BlockBlue");
        } else {
            strcpy(result, "6x6BlockBlack");
        }
    }
    else if (distanceSensorB <= 45) {
        if (red > 100 && green < 100 && blue < 100) {
            strcpy(result, "3x3BlockRed");
        }
        else if (red < 100 && green > 100 && blue < 100) {
            strcpy(result, "3x3BlockGreen");
        }
        else if (red < 100 && green > 100 && blue > 100) {
            strcpy(result, "3x3BlockBlue");
        } else {
            strcpy(result, "3x3BlockBlack");
        }
    }
    else {
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

    if (y <= 1) {
        y = 1;
    } else {
        y = y - 1;
    }
}

void yplus_direction_movement();

void yminus_direction_movement();

void robot_finished();

char selectedStr[100];
char lastNode[100];

//**To Be Finished**(MAY BE DITCHED IF DEEMEND UNECESSARY)//
void checkUnexploredRegionUpwardsHill() {

    strcpy(lastNode, coordinateDetails[numElements - 1].str);

    if (strcmp(lastNode, "TapeOrCliff") == 0 && checkUnexploredRegionHill == 0) {
        for (int i = 0; i < numElements; i++) {

            printf("Entered checkUnexploredRegionUpwardsHill\n");

            strcpy(selectedStr, coordinateDetails[i].str);

            if (strcmp(selectedStr, "Hill") == 0 && checkUnexploredRegionHill == 0) {
                if (coordinateDetails[numElements - 1].y > coordinateDetails[i].y) {

                    printf("Entered checkUnexploredRegionUpwardsHill\n");

                    left();
                    sleep_msec(100);
                    x = x - 1;

                    forwards();
                    sleep_msec(100);
                    forwards();
                    sleep_msec(100);

                    left();
                    y = y - 1;
                    sleep_msec(100);
                    checkUnexploredRegionHill = 1;
                    yminus_direction_movement();
                }
            }
        }
    }
}

void checkUnexploredRegionUpwardsCliff() {
    strcpy(lastNode, coordinateDetails[numElements - 1].str);

    if (strcmp(lastNode, "TapeOrCliff") == 0 && checkUnexploredRegionCliff == 0) {
        for (int i = 0; i < numElements; i++) {

            strcpy(selectedStr, coordinateDetails[i].str);

            if (strcmp(selectedStr, "TapeOrCliff") == 0 && checkUnexploredRegionCliff == 0) {
                if (coordinateDetails[numElements - 1].y > coordinateDetails[i].y) {
                    
                    printf("Entered checkUnexploredRegionUpwardsCliff\n");

                    left();
                    sleep_msec(100);
                    x = x - 1;

                    int k = coordinateDetails[i].y - coordinateDetails[numElements - 1].y;

                    for (int i = 0; i < k; i++) {
                        forwards();
                        sleep_msec(100);
                        x = x - 1;
                        k++;
                    }

                    left();
                    y = y - 1;
                    sleep_msec(100);
                    checkUnexploredRegionCliff = 1;
                    yminus_direction_movement();
                }
            }
        }
    }

}

void yplus_direction_movement() {
    char* coordinateDetails = investigateCoordinate();
    
    if (strcmp(coordinateDetails, "Nothing") == 0) {
        updateCoordinate(coordinateDetails, 0);
        forward_y_increasing();
        avoid = 0;
        yplus_direction_movement();
    }
    else if ((strcmp(coordinateDetails, "3x3BlockRed") == 0) || (strcmp(coordinateDetails, "3x3BlockBlue") == 0) 
        || (strcmp(coordinateDetails, "3x3BlockGreen") == 0) || (strcmp(coordinateDetails, "3x3BlockWhite") == 0)
        || (strcmp(coordinateDetails, "6x6BlockRed") == 0)   || (strcmp(coordinateDetails, "6x6BlockBlue") == 0) 
        || (strcmp(coordinateDetails, "6x6BlockGreen") == 0)) {
            updateCoordinate(coordinateDetails, 0);
            backwardsmain();
            sleep_msec(100);
            right();
            sleep_msec(100);
            //x = x + 1;
            //y = y + 1;
            //updateCoordinate("Nothing", 0);
            left();
            sleep_msec(100);
            forwards();
            sleep_msec(100);
            forwards();
            sleep_msec(100);
            forwards();
            sleep_msec(100);
            forwards();
            sleep_msec(100);
            forwards();
            sleep_msec(100);
            //y = y + 1;
            //updateCoordinate("Nothing", 0);
            left();
            sleep_msec(100);
            //x = x - 1;
            //updateCoordinate("Nothing", 0);
            right();
            sleep_msec(100);
            y = y + 1;
            //updateCoordinate("Nothing", 0);
            avoid = 1;
            yplus_direction_movement(); //continue with forward movement
    } else if (strcmp(coordinateDetails, "lastmovement") == 0) {
        sendmaxCoordinates();
        //robot_finished();
        printf("Entered LastMovement part\n");
        avoid = 0;
        robot_finished();
        //turn right
        //turn right
        //go forward until detect tape, then *FINISH*
    } else { //is cliff or hole in ground
        updateCoordinate(coordinateDetails, 0);
        backwards2();
        sleep_msec(100);
        //checkUnexploredRegionUpwardsHill();
        //checkUnexploredRegionUpwardsCliff();
        right();
        sleep_msec(100);
        x = x + 1;

        static char test[100];
        strcpy(test, "Nothing");
        //y = y + 1;
        //updateCoordinate(test, 0);
        //sleep_msec(100);
        forwards();
        sleep_msec(100);
        //forwards();
        right();
        sleep_msec(100);
        y = y - 1;
        //updateCoordinate(coordinateDetails, 0);
        avoid = 1;
        yminus_direction_movement();
    }
}

void yminus_direction_movement() {
    char* coordinateDetails = investigateCoordinate();

    if (strcmp(coordinateDetails, "Nothing") == 0) {
        updateCoordinate(coordinateDetails, 1);
        forward_y_decreasing();
        avoid = 0;
        yminus_direction_movement();
    } else if ((strcmp(coordinateDetails, "3x3BlockRed") == 0) || (strcmp(coordinateDetails, "3x3BlockBlue") == 0) 
        || (strcmp(coordinateDetails, "3x3BlockGreen") == 0) || (strcmp(coordinateDetails, "3x3BlockWhite") == 0)
        || (strcmp(coordinateDetails, "6x6BlockRed") == 0)   || (strcmp(coordinateDetails, "6x6BlockBlue") == 0) 
        || (strcmp(coordinateDetails, "6x6BlockGreen") == 0)) {
            updateCoordinate(coordinateDetails, 1);
            backwardsmain();
            sleep_msec(100);
            left();
            sleep_msec(100);

            //x = x + 1;
            //y = y - 1;
            //updateCoordinate(coordinateDetails, 1);
            right();
            sleep_msec(100);
            forwards();
            sleep_msec(100);
            forwards();
            sleep_msec(100);
            forwards();
            sleep_msec(100);
            forwards();
            sleep_msec(100);
            forwards();
            sleep_msec(100);
            //y = y - 1;
            //updateCoordinate(coordinateDetails, 1);
            right();
            sleep_msec(100);
            //x = x - 1;
            //updateCoordinate(coordinateDetails, 1);
            left();
            sleep_msec(100);

            //now update the y coordinate
            if (y <= 1) {
                y = 1;
            } else {
                y = y - 1;
            }

            avoid = 1;
            //updateCoordinate(coordinateDetails, 1);
            yminus_direction_movement(); //continue with (backwards) movement
    } else {
        updateCoordinate(coordinateDetails, 1);
        backwards2();
        sleep_msec(100);
        checkUnexploredRegionDownwardsHill();
        checkUnexploredRegionDownwardsCliff();
        left();
        sleep_msec(100);
        x = x + 1;

        static char test[100];
        strcpy(test, "Nothing");
        //y = y + 1;
        //updateCoordinate(test, 1);
        //x = x + 1;
        //y = y - 1;
        //updateCoordinate(coordinateDetails, 1);
        //sleep_msec(100);
        forwards();
        sleep_msec(100);
        //forwards();
        left();
        sleep_msec(100);
        y = y + 1;

        avoid = 1;
        //updateCoordinate(coordinateDetails, 1);
        yplus_direction_movement();
    }
}

void alg() {
    yplus_direction_movement();
}

void robot_finished() {
    disable_stepper();
    iic_destroy(IIC0);
    adc_destroy();
    pynq_destroy();
}

//Actually execute everything:
int main(void) {

    enable_stepper();
    adc_init();
    setupColorSensor();
    setupDistanceSensors();
    setupCommunication();
    
    while (START != true) {
        recieveData();
    }

    printf("Robot sterted!\n");
    alg(); //RUN THE DESIRED ALGORITHM
    //disable_stepper();
    //pynq_destroy();
    return EXIT_SUCCESS;
}
