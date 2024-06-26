#include <libpynq.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void setupUARTpins() {
    switchbox_set_pin(IO_AR0, SWB_UART0_RX);
    switchbox_set_pin(IO_AR1, SWB_UART0_TX);
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

void sendData(char *string, int number) {
    size_t number_length = snprintf(NULL, 0, "%d", number);     //calc the length of the number 
    size_t byte_size = strlen(string) + number_length + 2;      //calc size of message  //+2 for space and null terminator
    uint8_t *byte = (uint8_t *)malloc(byte_size);               //use malloc for messages of variable length (dynamic memory)
    if (byte == NULL) {
        fprintf(stderr, "Memory allocation failed\n");          //error check
        return;
    }
    snprintf((char *)byte, byte_size, "%s %d", string, number); //set the string and number to uint8_t which is going to be send
    printf("Uint8_t: %s\n", byte);
    
    // Extract length
    int byte_size_int = (int)byte_size;
    uint32_t num = (byte_size_int*8);
    uint8_t length[4];

    // Extract each byte
    length[0] = (uint8_t)(num & 0xFF);                  // Least significant byte
    length[1] = (uint8_t)((num >> 8) & 0xFF);           // Second byte
    length[2] = (uint8_t)((num >> 16) & 0xFF);          // Third byte
    length[3] = (uint8_t)((num >> 24) & 0xFF);          // Most significant byte
            
    uart_send_array(UART0, &length[0], 4);              //send length of msg first       
    uart_send_array(UART0, &byte[0], num);              //send message after

    //printf("size = %zu\n",byte_size);
    free(byte);                                         //free the dynamic memory
    return;
}

void compareMSG(char *msg, int *l_speed, int *r_speed, int *l_steps, int *r_steps) {
    // Use sscanf(msg, "%s %i", str_in_msg, int_in_msg)    ---> see: https://www.tutorialspoint.com/c_standard_library/c_function_sscanf.htm
    int int_in_msg;
    char str_in_msg[7];                                         //the instruction contains 7 letters (0 to 6 --> 7 == space for the NULL)   ===>> NEED to check!!!
    sscanf(msg, "%s %i", str_in_msg, &int_in_msg);              //reads the words and numbers from the message separatelly and sets them to the correct variable type
    if (strcmp(str_in_msg, "l_speed") == 0)
    {
        *l_speed = int_in_msg;
    }
    else if (strcmp(str_in_msg, "l_steps") == 0)
    {
        *l_steps = int_in_msg;
    }
    else if (strcmp(str_in_msg, "r_speed") == 0)
    {
        *r_speed = int_in_msg;
    } 
    else if (strcmp(str_in_msg, "r_steps") == 0)
    {
        *r_steps = int_in_msg;
    }
    return;
}

int main() {
    // Start pynq
    pynq_init();

    // Setup UART
    setupUARTpins();
    uart_init(UART0);
    uart_reset_fifos(UART0);

    // Define variables
    int l_speed = 0, r_speed = 0;
    int l_steps = 0, r_steps = 0;
    //.....
    ///////////////////

    // Infinite loop for the robot
    while (1)
    {
        if (uart_has_data(UART0)) {
            // Recieve message
            uint8_t size = 0;
            uart_read_array(UART0, &size, 4);                           //first part of the message is the size of the message
            unsigned char array[size];
            uart_read_array(UART0, &array[0], size);                    //second part of the message is the message itself
            //printf("Data recieved: %.*s\n", size, array);               //print for check!
            
            // Convert message to different var (for later use)
            char msg[size];
            int i = 0;
            while (array[i] != '\0')
            {
                msg[i] = array[i];
                i++;
            }

            // Process message and convert to motors instructions
            compareMSG(msg, &l_speed, &r_speed, &l_steps, &r_steps);
            
            // Testing for recieving messages which do not contain instructions
            i = 0;
            printf("Recieved message: ");
            while (i != size)
            {
                printf("%c",msg[i]);
                i++;
            }
            printf("\n");

            // Prints for check:
            printf("l_speed = %i\n", l_speed);
            printf("l_steps = %i\n", l_steps);
            printf("r_speed = %i\n", r_speed);
            printf("r_steps = %i\n", r_steps);
            printf("\n");

            // Send back after recieving a message
            if (uart_has_space(UART0))
            {
                char *string = "Hello!";
                int number = 41;
                sendData(string,number);
            }
        } 
    }

    pynq_destroy();
    return EXIT_SUCCESS;
}
