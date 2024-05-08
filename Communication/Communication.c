#include <libpynq.h>

void setupUARTpins() {
    switchbox_set_pin(IO_AR0, SWB_UART0_RX);
    switchbox_set_pin(IO_AR1, SWB_UART0_TX);
}

void uart_read_array(const int uart, uint8_t *buf, uint8_t l) {
    for (uint8_t x = 0; x < l; x++) {
        buf[x] = uart_recv(uart);
    }
}

int main() {
    //start pynq
    pynq_init();

    //setup UART
    setupUARTpins();
    uart_init(UART0);
    uart_reset_fifos(UART0);

    //setup steppers
    //stepper_init()
    //stepper_enable()
    //stepper_set_speed(..., ...)           //set initial speed 
    //stepper_set_steps(..., ...)           //set initial steps
    ///////////////////

    //define variables
    int l_speed = 0, r_speed = 0;
    int l_steps = 0, r_steps = 0;
    //..
    //..
    ///////////////////

    //infinite loop for the robot
    while (1)
    {
        if (uart_has_data(UART0)) {
            uint32_t size = 0;
            uart_read_array(UART0, &size, 4);                           //first part of the message is the size of the message
            char array[size];
            uart_read_array(UART0, &array, size);                       //second part of the message is the message itself
            printf("Data recieved: %.*s\n", size, array);               //print for check!
            printf("Size: %i\n", size);
            printf("Array: %s\n", array);
            //set x_speed and x_steps variables to the values from the message (need to see how the recieved message actually looks like)
            
            //enter the rest of the code here
        }


        //UART send data
        //send data regarding the rocks and/or clift and mountains
    }

    //need to be included so the steppers can move!
    while (!stepper_steps_done())
    {
        
    }

    //end
    stepper_destroy();
    pynq_destroy();
    return EXIT_SUCCESS;
}
