#include <libpynq.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

bool START = false;

void setupCommunication() {
    // Set UART pins
    switchbox_set_pin(IO_AR0, SWB_UART0_RX);
    switchbox_set_pin(IO_AR1, SWB_UART0_TX);

    // Initialize UART
    uart_init(UART0);
    uart_reset_fifos(UART0);
}

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

// Compare recieved msg --> START for starting the program, rest for coordinates regarding the position of obstacles
// Position of obtacles not needed? 
void compareMSG(char *msg, int *x_coordinate, int *y_coordinate) {
    //for input use sth. like x_coordinate and y_coordinate or whatever
    int int_in_msgA, int_in_msgB;
    char str_in_msg[50];                                         
    sscanf(msg, "%s %i %i", str_in_msg, &int_in_msgA, &int_in_msgB);              //reads the words and numbers from the message separatelly and sets them to the correct variable type
    if (strncmp(str_in_msg, "START", 6) == 0)                                     // needed for starting the robot remotely
    {
        START = true;
    }
    else if (strncmp(str_in_msg, "3x3BlockRed", 12) == 0 || strncmp(str_in_msg, "3x3BlockGreen", 14) == 0 || strncmp(str_in_msg, "3x3BlockBlue", 13) == 0)
    {
        *x_coordinate = int_in_msgA;
        *y_coordinate = int_in_msgB;
        printf("X: %i\n", *x_coordinate);
        printf("Y: %i\n", *y_coordinate);
    }
    else if (strncmp(str_in_msg, "6x6BlockRed", 12) == 0 || strncmp(str_in_msg, "6x6BlockGreen", 14) == 0 || strncmp(str_in_msg, "6x6BlockBlue", 13) == 0)
    {
        *x_coordinate = int_in_msgA;
        *y_coordinate = int_in_msgB;
        printf("X: %i\n", *x_coordinate);
        printf("Y: %i\n", *y_coordinate);
    }
    else if (strcmp(str_in_msg, "Cliff") == 0)
    {
        *x_coordinate = int_in_msgA;
        *y_coordinate = int_in_msgB;
        printf("X: %i\n", *x_coordinate);
        printf("Y: %i\n", *y_coordinate);
    } 
    else if (strcmp(str_in_msg, "Hill") == 0)
    {
        *x_coordinate = int_in_msgA;
        *y_coordinate = int_in_msgB;
        printf("X: %i\n", *x_coordinate);
        printf("Y: %i\n", *y_coordinate);
    }
    return;
}

// 
void recieveData(int *x_coordinate, int *y_coordinate) {
    if (uart_has_data(UART0))
    {
        uint8_t size = 0;
        uart_read_array(UART0, &size, 4);                           //first part of the message is the size of the message
        unsigned char array[size];
        uart_read_array(UART0, &array[0], size);                    //second part of the message is the message itself
        printf("Data recieved: %.*s\n", size, array);               //print for check!
            
        // Convert message to different var (for later use)
        char msg[size];
        int i = 0;
        while (array[i] != '\0')
        {
            msg[i] = array[i];
            i++;
        }

        // Process message and convert to motors instructions
        compareMSG(msg, x_coordinate, y_coordinate);
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
        return;
    } else {
        sendData(string);  // Try again if uart is full and cannot send data!
    }
    return;
}
    
int main(void) {
    //setupColorSensor();
    //setupDistanceSensors();
    pynq_init();
    setupCommunication();
    
    //example of manual start
    // buttons_init();
    // wait_until_any_button_pushed();
    // printf("Button pushed!\n");

    //example of remote start (both robots at the same time)
    int x_coordinate = 0, y_coordinate = 0;
    while (START != true)
    {
        recieveData(&x_coordinate, &y_coordinate);
    }
    printf("Robot sterted!\n");
    
    //alg(); //RUN THE DESIRED ALGORITHM
    
    //char *string = "HELLO 777";
    while (true)
    {
        //sendData(string);
        recieveData(&x_coordinate, &y_coordinate);
        //sleep_msec(2000);
    }
    
    //buttons_destroy();
    pynq_destroy();
    return EXIT_SUCCESS;
}
