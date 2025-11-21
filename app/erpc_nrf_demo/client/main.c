#include <stdio.h>
#include "xtimer.h"
#include "periph/uart.h"
#include "erpc_client_setup.h"
#include "erpc_mbf_setup.h"
#include "c_calculator_client.h"

// UART configuration
// UART_DEV(1) is usually mapped to pins P1.01 (RX) and P1.02 (TX) on nRF52840DK
#define UART_DEV_ID UART_DEV(1)
#define BAUD_RATE 115200

// External transport factory
void *erpc_uart_transport_init(uart_t uart_dev, uint32_t baud_rate);

int main(void) {
    puts("eRPC nRF52840DK Client Demo");

    // Initialize transport
    erpc_transport_t transport = erpc_uart_transport_init(UART_DEV_ID, BAUD_RATE);
    if (!transport) {
        puts("Failed to init transport");
        return 1;
    }

    // Initialize message buffer factory
    erpc_mbf_t mbf = erpc_mbf_dynamic_init();

    // Initialize client
    erpc_client_t client = erpc_client_init(transport, mbf);
    initCalculator_client(client);

    puts("Client initialized. Starting requests...");

    int32_t a = 10;
    int32_t b = 5;

    while (1) {
        printf("Calling add(%ld, %ld)...\n", (long)a, (long)b);
        int32_t sum = add(a, b);
        printf("Result: %ld\n", (long)sum);

        printf("Calling multiply(%ld, %ld)...\n", (long)a, (long)b);
        int32_t prod = multiply(a, b);
        printf("Result: %ld\n", (long)prod);

        a++;
        b++;
        xtimer_sleep(2);
    }

    return 0;
}
