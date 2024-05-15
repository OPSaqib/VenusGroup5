#include <libpynq.h>
#include <string.h>

#define MAXCHARS 2

int main (void)
{
    // Initialise all I/O
    pynq_init();

    // Set I/Os for UART
    switchbox_set_pin(IO_AR0,SWB_UART0_RX);
    switchbox_set_pin(IO_AR1,SWB_UART0_TX);

    // Initialize UART 0
    uart_init(UART0);
    uart_reset_fifos(UART0);

    //  !!!IMPORTANT!!! --> uint8_t stands for unsigned integer of 8 bits, which means that this is a type BYTE!!!!
    // void uart_send(const int uart, const uint8_t data) ==> data of type uint_8t

    uint8_t byte[] = "Hello!\n";                        // Define string as an array of type byte! ==> for later, might need to use sprintf();
    uint32_t length[] = strlen(byte);              // Need to send length of the message first (dunno if this is the correct initialisation of length)
    // Send length and than string (byte)
    int i = 0;
    while (length[i] != "\0") {
        // Send each bite (uint8_t) separately!!!!!!  -----> example: https://pynq.tue.nl/libpynq/5EID0-2023-v3.0.0/group__UART.html
        uart_send(UART0, length[i])
        printf("Sending length of message: '%s'\n", length);
        i++;
    }
    i = 0;
    while (byte[i] != "\0") {
        uart_send(UART0, byte[i]);                               
        printf("Sending message: '%s'\n", byte);        
        printf("\n");
        i++;
    }
    
    // Clean up after use
    pynq_destroy();
    return EXIT_SUCCESS;
} 
