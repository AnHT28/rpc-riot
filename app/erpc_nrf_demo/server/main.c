#include <stdio.h>
#include "periph/uart.h"
#include "erpc_server_setup.h"
#include "erpc_mbf_setup.h"
#include "c_calculator_server.h"

// UART configuration
// UART_DEV(1) is usually mapped to pins P1.01 (RX) and P1.02 (TX) on nRF52840DK
#define UART_DEV_ID UART_DEV(1)
#define BAUD_RATE 115200

// External transport factory
void *erpc_uart_transport_init(uart_t uart_dev, uint32_t baud_rate);
// External service factory (from c_calculator_server.cpp)
erpc_service_t create_Calculator_service(void);

// Implementation of service functions
int32_t add(int32_t a, int32_t b) {
    printf("Server: add(%ld, %ld)\n", (long)a, (long)b);
    return a + b;
}

int32_t subtract(int32_t a, int32_t b) {
    printf("Server: subtract(%ld, %ld)\n", (long)a, (long)b);
    return a - b;
}

int32_t multiply(int32_t a, int32_t b) {
    printf("Server: multiply(%ld, %ld)\n", (long)a, (long)b);
    return a * b;
}

float divide(int32_t a, int32_t b) {
    printf("Server: divide(%ld, %ld)\n", (long)a, (long)b);
    return (b != 0) ? (float)a / b : 0.0f;
}

int main(void) {
    puts("eRPC nRF52840DK Server Demo");

    // Initialize transport
    erpc_transport_t transport = erpc_uart_transport_init(UART_DEV_ID, BAUD_RATE);
    if (!transport) {
        puts("Failed to init transport");
        return 1;
    }

    // Initialize message buffer factory
    erpc_mbf_t mbf = erpc_mbf_dynamic_init();

    // Initialize server
    erpc_server_init(transport, mbf);

    // Create and add service
    erpc_service_t service = create_Calculator_service();
    erpc_add_service_to_server(service);

    puts("Server started, waiting for requests...");
    erpc_server_run();

    return 0;
}
