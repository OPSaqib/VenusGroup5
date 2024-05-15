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

/*void compareMSG(char *msg, int *l_speed, int *r_speed, int *l_steps, int *r_steps) {
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
        *l_steps = = int_in_msg;
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
}*/

int main() {
    // Start pynq
    pynq_init();

    // Setup UART
    setupUARTpins();
    uart_init(UART0);
    uart_reset_fifos(UART0);

    // Setup steppers
    //stepper_init()
    //stepper_enable()
    //stepper_set_speed(..., ...)           //set initial speed (probably 0)
    //stepper_set_steps(..., ...)           //set initial steps (probably 0)
    ///////////////////

    // Define variables
    //int l_speed = 0, r_speed = 0;
    //int l_steps = 0, r_steps = 0;
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
            printf("Data recieved: %.*s\n", size, array);               //print for check!
            
            // Convert message to different var
            char msg[size];
            int i = 0;
            while (array[i] != '\0')    //need to rewrite array to msg, because there are random values at the end of array (for printf("%s", array))
            {
                msg[i] = array[i];
                i++;
            }
            printf("Message: '%s'\n", msg);                             //print for check!

            // Process message and convert to motors instructions
            /*compareMSG(msg, &l_speed, &r_speed, &l_steps, &r_steps);

            //prints for check:
            printf("l_speed = %i\n", l_speed);
            printf("l_steps = %i\n", l_steps);
            printf("r_speed = %i\n", r_speed);
            printf("r_steps = %i\n", r_steps);*/
            
            /*enter the rest of the code here*/
            //.....
            //////////

            /*enter code for steppers here*/
        }
        
        //UART send data
        //send data regarding the rocks and/or clift and mountains
        uint8_t byte[] = "Hello!";                        // Define string as an array of type byte! ==> for later, might need to use sprintf();
        uint8_t length; // = strlen(byte);                   // Need to send length of the message first (dunno if this is the correct initialisation of length)
        // Send length and than string (byte)
        int s = 0;
        for (int k = 0; byte[k] != '\0'; k++)
        {
            s++;
        }
        length = i;
        printf("Length: %i\n", length);
        uart_send(UART0, length);
        s = 0;
        while (byte[s] != '\0') {
            uart_send(UART0, byte[s]);                               
            printf("Sending message: '%c'\n", byte[s]);        
            printf("\n");
            s++;
        }
    }

    //need to be included so the steppers can move!
    /*while (!stepper_steps_done())
    {
        
    }*/

    // End
    //stepper_destroy();
    pynq_destroy();
    return EXIT_SUCCESS;
}
